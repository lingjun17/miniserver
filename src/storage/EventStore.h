/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#ifndef SRC_STORAGE_EVENTSTORE_H_
#define SRC_STORAGE_EVENTSTORE_H_

#include <fstream>

#include "../model/MatchEvent.h"
#include "../model/OrderBook.h"
#include "../utils/MpscDoubleBufferQueue.h"

namespace orderbook {

class EventStore {
 public:
  explicit EventStore(std::string path);
  ~EventStore();
  bool persist(const std::shared_ptr<MatchEvent>& event);
  bool persist(const std::vector<MatchEvent> &events);
  int run();
  void shutdown();
  void init(std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<MatchEvent>>> eventQueue);
 private:
  /// field for data
  std::fstream mFile;
  std::string mPath;
  std::atomic<bool> mIsShutdown = false;
  std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<MatchEvent>>> mEventQueuePtr;
};

}  // namespace orderbook

#endif  // SRC_STORAGE_EVENTSTORE_H_
