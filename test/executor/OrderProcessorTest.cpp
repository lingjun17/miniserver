/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#include <gtest/gtest-spi.h>
#include <memory>

#include "spdlog/spdlog.h"
#include "../../src/executor/OrderProcessor.h"
#include "../../src/storage/EventStore.h"

namespace orderbook {

class OrderProcessorTest : public ::testing::Test {
 protected:
  OrderProcessorTest() {}
  ~OrderProcessorTest() override = default;
  /*
   * > New 1 buy 2@100
   * New Order submitted, ID=1
   * > New 2 buy 2@99
   * New Order submitted, ID=2
   * > New 3 buy 3@99
   * New Order submitted, ID=3
   * > New 4 sell 2@101
   * New Order submitted, ID=4
   * > print book
   * ask:
   * 2@101 bid:
   * 2@100
   * 2@99 3@99
   * > New 5 sell 3@99
   * New Order submitted, ID=5
   * Order ID=1 FullyFilled filledPrice=100 filledQty=2
   * Order ID=5 PartiallyFilled filledPrice=100 filledQty=2 leaves 1@99
   * Order ID=2 PartiallyFilled filledPrice=99 filledQty=1 leaves 1@99
   * Order ID=5 FullyFilled filledPrice=99 filledQty=1
   */
  void SetUp() override {
    SPDLOG_INFO("set up");
    mCommandQueue = std::make_shared<MpscDoubleBufferQueue<std::shared_ptr<Order>>>();
    mEventQueue = std::make_shared<MpscDoubleBufferQueue<std::shared_ptr<MatchEvent>>>();
    /// do not add mPath, because if add, the history data will be saved, and it will recover automatically
    /// so that the UT can only pass the first time, only if you delete the file.
    mPath = "";
    mOrderProcessorLoop = std::make_unique<OrderProcessor>(mPath);
    mOrderProcessorLoop->init(mCommandQueue, mEventQueue);

    mEventStoreLoop = std::make_unique<EventStore>(mPath);
    mEventStoreLoop->init(mEventQueue);

    mOrderVec.emplace_back(std::make_shared<Order>(1, OrderStatus::NEW, OrderSide::BUY, "100.00", 2));
    mOrderVec.emplace_back(std::make_shared<Order>(2, OrderStatus::NEW, OrderSide::BUY, "99.00", 2));
    mOrderVec.emplace_back(std::make_shared<Order>(3, OrderStatus::NEW, OrderSide::BUY, "99.00", 3));
    mOrderVec.emplace_back(std::make_shared<Order>(4, OrderStatus::NEW, OrderSide::SELL, "101.00", 2));
    mOrderVec.emplace_back(std::make_shared<Order>(5, OrderStatus::NEW, OrderSide::SELL, "99.00", 3));
  }

  void TearDown() override {
    mCommandQueue->shutdown();
    mEventQueue->shutdown();
    mOrderProcessorLoop->shutdown();
    mEventStoreLoop->shutdown();
    SPDLOG_INFO("tear down");
  }

  std::unique_ptr<OrderProcessor> mOrderProcessorLoop;
  std::unique_ptr<EventStore> mEventStoreLoop;

  std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<Order>>> mCommandQueue;
  std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<MatchEvent>>> mEventQueue;

  std::vector<std::shared_ptr<Order>> mOrderVec;

  std::string mPath;
};

// UT for the data provide in the question
TEST_F(OrderProcessorTest, DoExecuteRequired) {
  for (auto order : mOrderVec) {
    mCommandQueue->enqueue(order);
  }

  while(!mCommandQueue->empty()) {
    mOrderProcessorLoop->execute(mCommandQueue->dequeue());
  }

  while(!mEventQueue->empty()) {
    mEventStoreLoop->persist(mEventQueue->dequeue());
  }

  auto buyBook = mOrderProcessorLoop->getBook(OrderSide::BUY);
  EXPECT_EQ(buyBook->getBook()->size(), uint64_t(2));
  auto it = buyBook->getBook()->begin();
  auto order1 = it->second;
  EXPECT_EQ(order1->mOrderId, uint64_t(2));
  EXPECT_EQ(order1->mOrderStatus, OrderStatus::PARTIALLY_FILLED);
  EXPECT_EQ(order1->mQuantity, uint64_t(1));
  EXPECT_EQ(order1->mPrice, BigDecimal(99));
  EXPECT_EQ(order1->mOrderSide, OrderSide::BUY);
  it++;
  auto order2 = it->second;
  EXPECT_EQ(order2->mOrderId, uint64_t(3));
  EXPECT_EQ(order2->mOrderStatus, OrderStatus::NEW);
  EXPECT_EQ(order2->mQuantity, uint64_t(3));
  EXPECT_EQ(order2->mPrice, BigDecimal(99));
  EXPECT_EQ(order2->mOrderSide, OrderSide::BUY);

  auto sellBook = mOrderProcessorLoop->getBook(OrderSide::SELL);
  auto order = sellBook->getBook()->begin()->second;
  EXPECT_EQ(sellBook->getBook()->size(), uint64_t(1));
  EXPECT_EQ(order->mOrderId, uint64_t(4));
  EXPECT_EQ(order->mOrderStatus, OrderStatus::NEW);
  EXPECT_EQ(order->mQuantity, uint64_t(2));
  EXPECT_EQ(order->mPrice, BigDecimal(101));
  EXPECT_EQ(order->mOrderSide, OrderSide::SELL);
}


/*
 * The UT is for the data stored in test/data
 * Command: test.command
 * RecoverData: test.recover
 * Expected: test.expected
 *
 * print book
 * ask:
 * 2@102.00
 * 1@120.00 2@120.00
 * bid:
 * 5@101.00
 * 2@100.00
 */
class RecoverTest : public ::testing::Test {
protected:
  RecoverTest():mPath("../test/data/test.recover"){}
  ~RecoverTest() override = default;
  void SetUp() override {
    SPDLOG_INFO("set up");
    mOrderProcessorLoop = std::make_shared<OrderProcessor>(mPath);
  }

  void TearDown() override {
    mOrderProcessorLoop->shutdown();
    SPDLOG_INFO("tear down");
  }

  std::shared_ptr<OrderProcessor> mOrderProcessorLoop;
  std::vector<Order> mOrderVec;
  std::string mPath;
};

TEST_F(RecoverTest, DoRecover) {
  mOrderProcessorLoop->recover();
  auto buyBook = mOrderProcessorLoop->getBook(OrderSide::BUY);
  EXPECT_EQ(buyBook->getBook()->size(), uint64_t(2));
  auto buyIt = buyBook->getBook()->begin();
  auto buyOrder1 = buyIt++->second;
  EXPECT_EQ(buyOrder1->mQuantity, uint64_t(5));
  EXPECT_EQ(buyOrder1->mPrice, BigDecimal(101));
  auto buyOrder2 = buyIt->second;
  EXPECT_EQ(buyOrder2->mQuantity, uint64_t(2));
  EXPECT_EQ(buyOrder2->mPrice, BigDecimal(100));

  auto sellBook = mOrderProcessorLoop->getBook(OrderSide::SELL);
  EXPECT_EQ(sellBook->getBook()->size(), uint64_t(3));
  auto sellIt = sellBook->getBook()->begin();
  auto sellOrder1 = sellIt++->second;
  EXPECT_EQ(sellOrder1->mQuantity, uint64_t(2));
  EXPECT_EQ(sellOrder1->mPrice, BigDecimal(102));
  auto sellOrder2 = sellIt++->second;
  EXPECT_EQ(sellOrder2->mQuantity, uint64_t(1));
  EXPECT_EQ(sellOrder2->mPrice, BigDecimal(120));
  auto sellOrder3 = sellIt->second;
  EXPECT_EQ(sellOrder3->mQuantity, uint64_t(2));
  EXPECT_EQ(sellOrder3->mPrice, BigDecimal(120));
}

}  // namespace orderbook
