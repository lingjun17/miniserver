/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#include "OrderBook.h"

#include <iostream>

namespace orderbook {

bool OrderBook::remove(const OrderKey &orderKey) {
  auto size = mBookPtr->erase(orderKey);
  return size != 0;
}

bool OrderBook::add(std::shared_ptr<Order> order) {
  auto res = mBookPtr->emplace(OrderKey{order->mOrderId, order->mPrice}, order);
  return res.second;
}

bool OrderBook::replace(std::shared_ptr<Order> order) {
  remove(OrderKey{order->mOrderId, order->mPrice});
  add(order);
  return true;
}

uint64_t OrderBook::size() {
  return mBookPtr->size();
}

std::shared_ptr<OrderBook::OrderMap> OrderBook::getBook() {
  return mBookPtr;
}

// Single thread in core processor, no wild pointer issue
std::shared_ptr<Order> OrderBook::getOrder(const OrderKey &key) {
  auto it = mBookPtr->find(key);
  if (mBookPtr->find(key) == mBookPtr->end()) {
    return nullptr;
  }
  return it->second;
}

std::shared_ptr<Order> OrderBook::getFirstOrder() {
  if (mBookPtr->empty()) {
    return nullptr;
  }
  return mBookPtr->begin()->second;
}

void OrderBook::print() {
  if (mSide == OrderSide::BUY) {
    std::cout << "bid:";
  } else {
    std::cout << "ask:";
  }
  BigDecimal currentPrice;
  for (auto it = mBookPtr->begin(); it != mBookPtr->end(); it++) {
    auto order = it->second;
    if (currentPrice != order->mPrice) {
      std::cout << std::endl;
      currentPrice = order->mPrice;
    }
    std::cout << order->mQuantity << "@" << order->mPrice.toString() << " ";
  }
  std::cout << std::endl;
}

}  // namespace orderbook
