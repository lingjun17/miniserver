/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#include "MatchEvent.h"

#include <iostream>
#include <string>

#include "absl/strings/numbers.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_split.h"

namespace orderbook {

std::string MatchEvent::encodeToString() const {
  return absl::StrFormat("%ld|%ld|%ld|%s|%s|%ld|%ld\n", mOrderId, mSide, static_cast<int>(mOrderStatus),
                         mPrice, mFilledPrice, mFilledQuantity, mLeftQuantity);
}

MatchEvent MatchEvent::decodeFromString(const std::string &eventStr) {
  std::vector<absl::string_view> items = absl::StrSplit(eventStr, '|');
  MatchEvent event;
  assert(absl::SimpleAtoi(items[0], &event.mOrderId));
  uint64_t side = 0;
  assert(absl::SimpleAtoi(items[1], &side));
  event.mSide = static_cast<OrderSide>(side);
  uint64_t status = 0;
  assert(absl::SimpleAtoi(items[2], &status));
  event.mOrderStatus = static_cast<OrderStatus>(status);
  event.mPrice = items[3];
  event.mFilledPrice = items[4];
  assert(absl::SimpleAtoi(items[5], &event.mFilledQuantity));
  assert(absl::SimpleAtoi(items[6], &event.mLeftQuantity));
  return event;
}

void MatchEvent::print() {
  if (mOrderStatus == OrderStatus::PARTIALLY_FILLED) {
    std::cout << absl::StrFormat("Order ID=%ld PartiallyFilled filledPrice=%s filledQty=%ld leaves %ld@%s",
                                 mOrderId, mFilledPrice, mFilledQuantity, mLeftQuantity, mPrice) << std::endl;
  } else if (mOrderStatus == OrderStatus::FULLY_FILLED) {
    std::cout << absl::StrFormat("Order ID=%ld FullyFilled filledPrice=%s filledQty=%ld",
                                 mOrderId, mFilledPrice, mFilledQuantity) << std::endl;
  } else if  (mOrderStatus == OrderStatus::NEW) {
    std::cout << absl::StrFormat("New Order submitted, ID=%ld", mOrderId) << std::endl;
  } else if (mOrderStatus ==  OrderStatus::CANCELED) {
    std::cout << absl::StrFormat("Order-Canceled, ID=%ld", mOrderId) << std::endl;
  } else {
    std::cout << encodeToString() << std::endl;
  }
}
}  // namespace orderbook
