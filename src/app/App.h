/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#ifndef SRC_APP_APP_H_
#define SRC_APP_APP_H_

#include <spdlog/spdlog.h>
#include <tuple>

#include "../admin/Client.h"
#include "../executor/OrderProcessor.h"
#include "../model/MatchEvent.h"
#include "../model/Order.h"
#include "../utils/MpscDoubleBufferQueue.h"
#include "../storage/EventStore.h"

namespace orderbook {

class App final {
 public:
  App(const std::string &storePath);
  ~App();

  // disallow copy ctor and copy assignment
  App(const App &) = delete;
  App &operator=(const App &) = delete;

  // disallow move ctor and move assignment
  App(App &&) = delete;
  App &operator=(App &&) = delete;

  void init();
  void run();
  void shutdown();

 private:
  void startEventStoreLoop();
  void startOrderProcessLoop();
  void startClientLoop();

 private:
  std::thread mClientLoopThread;
  std::thread mOrderProcessLoopThread;
  std::thread mEventStoreLoopThread;

  std::unique_ptr<OrderProcessor> mOrderProcessorLoop;
  std::unique_ptr<EventStore> mEventStoreLoop;
  std::unique_ptr<Client> mClientLoop;

  std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<Order>>> mCommandQueue;
  std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<MatchEvent>>> mEventQueue;
  std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<MatchEvent>>> mReplyQueue;

  bool mIsShutdown;
  std::string mStorePath;
};

}  // namespace orderbook

#endif  // SRC_APP_APP_H_
