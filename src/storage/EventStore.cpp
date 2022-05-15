/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#include "EventStore.h"

#include <iostream>
#include <utility>

#include "spdlog/spdlog.h"


namespace orderbook {

EventStore::EventStore(std::string path) : mPath(std::move(path)) {
  mFile.open(mPath.c_str(), std::ios_base::app);
}

bool EventStore::persist(const std::shared_ptr<MatchEvent>& eventPtr) {
  if (eventPtr == nullptr) {
    return false;
  }
  std::string str = eventPtr->encodeToString();
  if (mFile.is_open()) {
    mFile.write(str.data(), str.length());
  }
  mFile.flush();
  mFile.sync();
  return true;
}

bool EventStore::persist(const std::vector<MatchEvent> &events) {
  /// copy to data file
  for (const auto &event : events) {
    std::string str = event.encodeToString();
    if (mFile.is_open()) {
      mFile.write(str.data(), str.length());
    }
  }
  mFile.flush();
  mFile.sync();
  return true;
}

int EventStore::run() {
  while (1) {
    if (mIsShutdown) {
      SPDLOG_WARN("EventStore is already down. Will not run again.");
      return 0;
    }
    auto event = mEventQueuePtr->dequeue();
    persist(event);
    event->print();
    SPDLOG_INFO("ENCODE TO: {}", event->encodeToString());
  }
}


void EventStore::shutdown() {
  SPDLOG_INFO("Shutting down OrderProcessor");
  if (mIsShutdown) {
    SPDLOG_INFO("Server is already down");
  } else {
    mIsShutdown = true;
  }
}

void EventStore::init(std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<MatchEvent>>> eventQueue) {
  this->mEventQueuePtr = std::move(eventQueue);
}

EventStore::~EventStore() {
  mFile.close();
}


}  // namespace orderbook
