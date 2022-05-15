/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/
#include "Order.h"

#include <string>
#include "spdlog/spdlog.h"

namespace orderbook {

std::string toString(OrderStatus status) {
  switch (status) {
    case OrderStatus::NEW:
      return "New";
    case OrderStatus::PARTIALLY_FILLED:
      return "PartiallyFilled";
    case OrderStatus::FULLY_FILLED:
      return "FullyFilled";
    case OrderStatus::CANCELED:
      return "Canceled";
    case OrderStatus::REJECTED:
      return "Rejected";
    default:
      return "";
  }
}

std::string toString(OrderSide side) {
  switch (side) {
    case OrderSide::BUY:
      return "BUY";
    case OrderSide::SELL:
      return "SELL";
    default:
      return "";
  }
}

void Order::print() {
  SPDLOG_INFO("Order ID={} {} {} {} {}", mOrderId, toString(mOrderStatus), toString(mOrderSide),
              mPrice.toString(), mQuantity);
}

}  // namespace orderbook
