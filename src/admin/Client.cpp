/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#include "Client.h"

#include <iostream>
#include <utility>
#include <regex>
#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"

namespace orderbook {

int Client::run() {
  sendCmd();
  return 0;
}

void Client::shutdown() {
  SPDLOG_INFO("Shutting down Client");
  if (mIsShutdown) {
    SPDLOG_INFO("Client is already down");
  } else {
    mIsShutdown = true;
  }
}

void Client::init(std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<Order>>> commandQueue) {
  mOrderQueue = std::move(commandQueue);
}

Client::Client() {
}

std::shared_ptr<Order> Client::checkCmd(const std::string &cmd) {
  auto order = std::make_shared<Order>();
  order->mOrderStatus = OrderStatus::NEW;
  if (cmd == "print book") {
    order->mAction = OrderAction::PRINT;
    return order;
  }

  std::vector<std::string> vec = absl::StrSplit(cmd, ' ');
  if (vec.size() != 4) {
    std::cout << "Bad command" << std::endl;
    return nullptr;
  }

  if (vec[0] == "Cancel") {
    order->mAction = OrderAction::CANCEL;
  } else if (vec[0] == "New") {
    order->mAction = OrderAction::ADD;
  } else if (vec[0] == "Replace") {
    order->mAction = OrderAction::REPLACE;
  } else {
    std::cout << "Invalid Command [Cancel|New|Replace] " << vec[0] << std::endl;
    return nullptr;
  }

  if (!absl::SimpleAtoi(vec[1], &order->mOrderId)) {
    std::cout << "Invalid OrderId " << vec[1] << std::endl;
    return nullptr;
  }
  if (vec[2] == "sell") {
    order->mOrderSide = OrderSide::SELL;
  } else if (vec[2] == "buy") {
    order->mOrderSide = OrderSide::BUY;
  } else {
    std::cout << "Invalid OrderType [buy|sell] " << std::endl;
    return nullptr;
  }

  if (order->mAction == OrderAction::CANCEL) {
    if (BigDecimal::isValid(vec[3])) {
      order->mPrice = BigDecimal(vec[3]);
    } else {
      std::cout << "Invalid Price" << std::endl;
    }
  } else if (order->mAction == OrderAction::ADD || order->mAction == OrderAction::REPLACE) {
    std::vector<std::string> tmp = absl::StrSplit(vec[3], '@');
    if (BigDecimal::isValid(tmp[1])) {
      order->mPrice = BigDecimal(tmp[1]);
    } else {
      std::cout << "Invalid Price " << std::endl;
      return nullptr;
    }

    if (!absl::SimpleAtoi(tmp[0], &order->mQuantity)) {
      std::cout << "Invalid Quantity " << tmp[0] << std::endl;
      return nullptr;
    }
  } else {
    std::cout << "Invalid Action " << std::endl;
    return nullptr;
  }

  return order;
}

void Client::usage() {
  std::cout
      << "Support commands: \n"
      << "************************************************\n"
      << "* New CMD: [New orderid side qty@price] \n"
      << "* add new buy order: New 1 buy 2@100 \n"
      << "* add new sell order: New 2 sell 2@100 \n\n"
      << "* Cancel CMD: [Cancel orderid side price] \n"
      << "* cancel buy order: Cancel 1 buy 100 \n"
      << "* cancel sell order: Cancel 2 sell 100 \n\n"
      << "* Replace CMD: [Replace orderid side newQty@price] \n"
      << "* replace buy order: Replace 1 buy 20@100 \n"
      << "* replace sell order: Replace 2 sell 20@100 \n"
      << "************************************************"
      << std::endl;}

void Client::sendCmd() {
  usage();
  std::string cmd;
  while (getline(std::cin, cmd)) {
    cmd = absl::StripTrailingAsciiWhitespace(cmd);
    if (cmd.length() == 0) {
      continue;
    }
    auto orderPtr = checkCmd(cmd);
    if (orderPtr == nullptr) {
      continue;
    }
    mOrderQueue->enqueue(orderPtr);
  }
}

}  // namespace orderbook
