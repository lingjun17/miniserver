/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#ifndef SRC_MODEL_ORDERBOOK_H_
#define SRC_MODEL_ORDERBOOK_H_

#include <functional>
#include <map>
#include <memory>

#include "Order.h"

namespace orderbook {

class OrderBook {
 public:
  using OrderMap = std::map<OrderKey, std::shared_ptr<Order>, OrderCmp>;
  explicit OrderBook(OrderSide side):mSide(side) {
    switch (mSide) {
      case OrderSide::BUY:
        mBookPtr = std::make_shared<OrderMap>(OrderCmp(false));
        break;
      case OrderSide::SELL:
        mBookPtr = std::make_shared<OrderMap>(OrderCmp(true));
        break;
      default:
        break;
    }
  }
  void print();
  ~OrderBook() = default;
  bool remove(const OrderKey &key);
  bool add(std::shared_ptr<Order> order);
  bool replace(std::shared_ptr<Order> order);
  uint64_t size();
  std::shared_ptr<OrderMap> getBook();
  std::shared_ptr<Order> getFirstOrder();
  std::shared_ptr<Order> getOrder(const OrderKey &key);

 private:
  OrderSide mSide;
  std::shared_ptr<OrderMap> mBookPtr;
};

}  // namespace orderbook

#endif  // SRC_MODEL_ORDERBOOK_H_
