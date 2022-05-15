/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#include "OrderProcessor.h"

#include <iostream>
#include <utility>
#include "spdlog/spdlog.h"

namespace orderbook {

// Execute order and generate the result(MatchEvent)
void OrderProcessor::execute(std::shared_ptr<Order> order) {
  if (order == nullptr) {
    return;
  }

  // deal with "print book" command
  if (order->mAction == OrderAction::PRINT) {
    mSellerBook->print();
    mBuyerBook->print();
    return;
  }

  switch (order->mOrderSide) {
    case OrderSide::BUY:
      // For buyer, match sellerBook，store into buyerBook:
      processOrder(order, this->mSellerBook, this->mBuyerBook);
      return;
    case OrderSide::SELL:
      // For seller, match buyerBook，store into sellerbook:
      processOrder(order, this->mBuyerBook, this->mSellerBook);
      return;
    default:
      break;
  }
  return;
}

// Core order match logic, match order from top of matchbook and store to dstbook
// TODO(junling): Stdout should be replaced to network response
void OrderProcessor::processOrder(std::shared_ptr<Order> order, std::shared_ptr<OrderBook> matchBook,
                                                     std::shared_ptr<OrderBook> dstBook) {
  OrderKey orderKey {order->mOrderId, order->mPrice};
  if (order->mAction == OrderAction::ADD && dstBook->getOrder(orderKey) != nullptr) {
    std::cout << "New-Order Rejected, ID=" << order->mOrderId << std::endl;
    return;
  }

  if (order->mAction == OrderAction::CANCEL) {
    auto orderPtr = dstBook->getOrder(orderKey);
    if (orderPtr == nullptr) {
      std::cout << "Order-CancelRejected, ID=" << order->mOrderId << std::endl;
      return;
    }

    // Generate canceled event
    mEventQueuePtr->enqueue(std::make_shared<MatchEvent>(orderPtr->mOrderId, orderPtr->mOrderSide,
           OrderStatus::CANCELED, orderPtr->mPrice.toString(), mMarketPrice.toString(), 0, orderPtr->mQuantity));
    dstBook->remove(orderKey);
    return;
  }

  // Replace command will only change qty but now price ?
  if (order->mAction == OrderAction::REPLACE) {
    auto orderPtr = dstBook->getOrder(orderKey);
    if (orderPtr == nullptr) {
      std::cout << "Order-ReplaceRejected, ID=" << order->mOrderId << std::endl;
      return;
    }

    // Generate canceled event
    mEventQueuePtr->enqueue(std::make_shared<MatchEvent>(orderPtr->mOrderId, orderPtr->mOrderSide,
         OrderStatus::CANCELED, orderPtr->mPrice.toString(), mMarketPrice.toString(), 0, orderPtr->mQuantity));
    dstBook->remove(orderKey);
  }

  for (;;) {
    auto firstOrder = matchBook->getFirstOrder();
    if (!firstOrder) {
      break;
    }
    OrderKey firstOrderKey {firstOrder->mOrderId, firstOrder->mPrice};
    if (OrderSide::BUY == order->mOrderSide && order->mPrice < firstOrder->mPrice) {
      // buyer price < first seller price:
      break;
    }
    if (OrderSide::SELL == order->mOrderSide && order->mPrice > firstOrder->mPrice) {
      // seller price > first buyer price:
      break;
    }

    // market price is buyer price.
    if (OrderSide::BUY == order->mOrderSide) {
      this->mMarketPrice = order->mPrice;
    } else {
      this->mMarketPrice = firstOrder->mPrice;
    }
    // match qty = min(order, firstOrder)
    auto matchQty = std::min(order->mQuantity, firstOrder->mQuantity);

    // update the new order and first order
    uint64_t firstOrderLeftQty = updateOrder(firstOrder, matchQty);
    uint64_t newOrderLeftQty = updateOrder(order, matchQty);

    // Add events
    mEventQueuePtr->enqueue(std::make_shared<MatchEvent>(firstOrder->mOrderId,
                                                         firstOrder->mOrderSide,
                                                         firstOrder->mOrderStatus,
                                                         firstOrder->mPrice.toString(),
                                                         mMarketPrice.toString(),
                                                         matchQty,
                                                         firstOrderLeftQty));

    mEventQueuePtr->enqueue(std::make_shared<MatchEvent>(order->mOrderId,
                                                         order->mOrderSide,
                                                         order->mOrderStatus,
                                                         order->mPrice.toString(),
                                                         mMarketPrice.toString(),
                                                         matchQty,
                                                         newOrderLeftQty));

    if (firstOrderLeftQty == 0) {
      matchBook->remove(firstOrderKey);
    }
    if (order->mQuantity == 0) {
      break;
    }
  }

  // new order is not full executed:
  if (order->mQuantity > 0) {
    dstBook->add(order);
    mEventQueuePtr->enqueue(std::make_shared<MatchEvent>(order->mOrderId,
                                                         order->mOrderSide,
                                                         order->mOrderStatus,
                                                         order->mPrice.toString(),
                                                         mMarketPrice.toString(),
                                                         0,
                                                         order->mQuantity));
  }
  return;
}

uint64_t OrderProcessor::updateOrder(std::shared_ptr<Order> order, const uint64_t &matchQty) const {
  auto leftQty = order->mQuantity - matchQty;
  if (leftQty == 0) {
    order->mOrderStatus = OrderStatus::FULLY_FILLED;
  } else {
    order->mOrderStatus = OrderStatus::PARTIALLY_FILLED;
  }
  order->mQuantity = leftQty;
  return leftQty;
}

std::shared_ptr<OrderBook> OrderProcessor::getBook(OrderSide side) {
  switch (side) {
    case OrderSide::BUY:
      return mBuyerBook;
    case OrderSide::SELL:
      return mSellerBook;
    default:
      break;
  }
  return nullptr;
}

int OrderProcessor::run() {
  while (1) {
    if (mIsShutdown) {
      SPDLOG_WARN("OrderProcessor is already down. Will not run again.");
      return 0;
    }
    auto orderPtr = mOrderQueuePtr->dequeue();
    execute(orderPtr);
    SPDLOG_INFO("in order processor");
  }
}


void OrderProcessor::shutdown() {
  SPDLOG_INFO("Shutting down OrderProcessor");
  if (mIsShutdown) {
    SPDLOG_INFO("Server is already down");
  } else {
    mIsShutdown = true;
  }
}

void OrderProcessor::init(std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<Order>>> orderQueue,
                          std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<MatchEvent>>> eventQueue) {
  this->mOrderQueuePtr = std::move(orderQueue);
  this->mEventQueuePtr = std::move(eventQueue);
  recover();
}

bool OrderProcessor::recover() {
  mFile.open(mPath.c_str());
  std::string line;
  while (getline(mFile, line)) {
    SPDLOG_INFO(line);
    const auto &event = MatchEvent::decodeFromString(line);
    std::shared_ptr<Order> order = std::make_shared<Order>(event.mOrderId, event.mOrderStatus,
                                                                 event.mSide, event.mPrice, event.mLeftQuantity);
    if (OrderSide::SELL == event.mSide) {
      recoverBook(order, mSellerBook);
    } else if (OrderSide::BUY == event.mSide) {
      recoverBook(order, mBuyerBook);
    } else {
      SPDLOG_ERROR("Bad order side {}", toString(event.mSide));
      return false;
    }
  }
  return true;
}

void OrderProcessor::recoverBook(std::shared_ptr<Order> order, std::shared_ptr<OrderBook> book) {
  OrderKey orderKey {order->mOrderId, order->mPrice};
  switch (order->mOrderStatus) {
    case OrderStatus::NEW:
      book->add(order);
      break;
    case OrderStatus::PARTIALLY_FILLED:
      book->replace(order);
      break;
    case OrderStatus::FULLY_FILLED:
      book->remove(orderKey);
      break;
    case OrderStatus::CANCELED:
      book->remove(orderKey);
      break;
    case OrderStatus::REJECTED:
      break;
  }
}

}  // namespace orderbook
