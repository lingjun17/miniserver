/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#include "App.h"

namespace orderbook {

App::App(const std::string &storePath) : mIsShutdown(false), mStorePath(storePath) {
  init();
}

App::~App() {
  SPDLOG_INFO("deleting app");
}

void App::init() {
  mCommandQueue = std::make_shared<MpscDoubleBufferQueue<std::shared_ptr<Order>>>();
  mEventQueue = std::make_shared<MpscDoubleBufferQueue<std::shared_ptr<MatchEvent>>>();

  mOrderProcessorLoop = std::make_unique<OrderProcessor>(mStorePath);
  mOrderProcessorLoop->init(mCommandQueue, mEventQueue);

  mEventStoreLoop = std::make_unique<EventStore>(mStorePath);
  mEventStoreLoop->init(mEventQueue);

  mClientLoop = std::make_unique<Client>();
  mClientLoop->init(mCommandQueue);
}

void App::startOrderProcessLoop() {
  mOrderProcessLoopThread = std::thread([this]() {
    pthread_setname_np(pthread_self(), "OrderProcessLoop");
    mOrderProcessorLoop->run();
  });
}

void App::startEventStoreLoop() {
  mEventStoreLoopThread = std::thread([this]() {
    pthread_setname_np(pthread_self(), "EventStoreLoop");
    mEventStoreLoop->run();
  });
}

void App::startClientLoop() {
  mClientLoopThread = std::thread([this]() {
    pthread_setname_np(pthread_self(), "ClientLoop");
    mClientLoop->run();
  });
}

void App::run() {
  if (mIsShutdown) {
    SPDLOG_WARN("App is already down. Will not run again.");
  } else {
    startOrderProcessLoop();
    startEventStoreLoop();
    startClientLoop();

    // wait for all threads to exit
    mOrderProcessLoopThread.join();
    mEventStoreLoopThread.join();
    mClientLoopThread.join();
  }
}

void App::shutdown() {
  if (mIsShutdown) {
    SPDLOG_INFO("App is already down");
  } else {
    mIsShutdown = true;

    // shutdown all threads
    mCommandQueue->shutdown();
    mEventQueue->shutdown();
    mOrderProcessorLoop->shutdown();
    mClientLoop->shutdown();
    mEventStoreLoop->shutdown();
  }
}

}  /// namespace orderbook
