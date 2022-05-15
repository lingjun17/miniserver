/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#ifndef SRC_EXECUTOR_ORDERPROCESSOR_H_
#define SRC_EXECUTOR_ORDERPROCESSOR_H_

#include <fstream>

#include "../model/MatchEvent.h"
#include "../model/Order.h"
#include "../model/OrderBook.h"
#include "../utils/BigDecimal.h"
#include "../utils/MpscDoubleBufferQueue.h"

namespace orderbook {

class OrderProcessor {
 public:
  explicit OrderProcessor(const std::string &storePath) :mPath(storePath) {
    this->mBuyerBook = std::make_shared<OrderBook>(OrderSide::BUY);
    this->mSellerBook = std::make_shared<OrderBook>(OrderSide::SELL);
  }
  ~OrderProcessor() = default;
  void execute(std::shared_ptr<Order> order);
  int run();
  void shutdown();
  void init(std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<Order>>> orderQueue,
            std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<MatchEvent>>> eventQueue);
  void recoverBook(std::shared_ptr<Order> order, std::shared_ptr<OrderBook> book);
  bool recover();

  // for ut
  std::shared_ptr<OrderBook> getBook(OrderSide side);

 private:
  void processOrder(std::shared_ptr<Order> order,
                    std::shared_ptr<OrderBook> matchBook,
                    std::shared_ptr<OrderBook> dstBook);
  uint64_t updateOrder(std::shared_ptr<Order> order, const uint64_t &matchQty) const;
  std::shared_ptr<OrderBook> mBuyerBook;
  std::shared_ptr<OrderBook> mSellerBook;
  BigDecimal mMarketPrice;
  std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<Order>>> mOrderQueuePtr;
  std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<MatchEvent>>> mEventQueuePtr;
  std::atomic_bool mIsShutdown = false;

  std::fstream mFile;
  std::string mPath;
};

}  // namespace orderbook

#endif  // SRC_EXECUTOR_ORDERPROCESSOR_H_
