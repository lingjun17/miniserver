/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#ifndef SRC_MODEL_MATCHEVENT_H_
#define SRC_MODEL_MATCHEVENT_H_

#include <cstdint>
#include <iostream>
#include <utility>

#include "Order.h"

namespace orderbook {

class MatchEvent {
 public:
  MatchEvent() {}
  MatchEvent(const uint64_t &orderId,
             OrderSide side,
             OrderStatus status,
             std::string price,
             std::string filledPrice,
             const uint64_t &filledQuantity,
             const uint64_t &quantity) :mOrderId(orderId), mSide(side),
             mOrderStatus(status), mPrice(std::move(price)), mFilledPrice(std::move(filledPrice)),
             mFilledQuantity(filledQuantity), mLeftQuantity(quantity) {}
  ~MatchEvent() = default;
  std::string encodeToString() const;
  static MatchEvent decodeFromString(const std::string &eventStr);
  void print();

 public:
  uint64_t mOrderId;
  OrderSide mSide;
  OrderStatus mOrderStatus;
  std::string mPrice;
  std::string mFilledPrice;
  uint64_t mFilledQuantity;
  uint64_t mLeftQuantity;
};

}  // namespace orderbook

#endif  // SRC_MODEL_MATCHEVENT_H_
