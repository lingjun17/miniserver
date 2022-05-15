/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#ifndef SRC_MODEL_ORDER_H_
#define SRC_MODEL_ORDER_H_

#include <cstdint>
#include <utility>
#include "../utils/BigDecimal.h"

namespace orderbook {

enum class OrderStatus {
  NEW,
  PARTIALLY_FILLED,
  FULLY_FILLED,
  CANCELED,
  REJECTED,
};

enum class OrderSide {
  BUY,
  SELL,
};

enum class OrderAction {
  ADD,
  CANCEL,
  REPLACE,
  PRINT,
};

std::string toString(OrderStatus status);
std::string toString(OrderSide side);

class Order {
 public:
  Order() = default;
  Order(const uint64_t &orderId, OrderStatus status, OrderSide side, const std::string &price, const uint64_t &quantity)
    :mOrderId(orderId), mOrderStatus(status), mOrderSide(side), mPrice(price), mQuantity(quantity) {}
  ~Order() = default;
  void print();

 public:
  uint64_t mOrderId{};
  OrderStatus mOrderStatus;
  OrderSide mOrderSide;
  BigDecimal mPrice;
  uint64_t mQuantity{};
  OrderAction mAction;
};

struct OrderKey {
  uint64_t mOrderId{};
  BigDecimal mPrice;
};

class OrderCmp {
 public:
  explicit OrderCmp(bool isSellOrder):mIsSellOrder(isSellOrder) {}
  ~OrderCmp() = default;
  bool operator()(const OrderKey &lhs, const OrderKey &rhs) const {
    if (lhs.mPrice != rhs.mPrice && mIsSellOrder) {
      // Lower Price First
      return lhs.mPrice < rhs.mPrice;
    } else if (lhs.mPrice != rhs.mPrice && !mIsSellOrder) {
      // Higher Price First
      return lhs.mPrice > rhs.mPrice;
    } else {
      return lhs.mOrderId < rhs.mOrderId;
    }
  }
 private:
  bool mIsSellOrder;
};

}  // namespace orderbook

#endif  // SRC_MODEL_ORDER_H_
