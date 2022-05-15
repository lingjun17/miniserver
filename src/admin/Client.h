/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#ifndef SRC_ADMIN_CLIENT_H_
#define SRC_ADMIN_CLIENT_H_

#include <map>
#include <memory>

#include "../model/MatchEvent.h"
#include "../model/Order.h"
#include "../utils/MpscDoubleBufferQueue.h"

namespace orderbook {

class Client {
 public:
  Client();
  ~Client() = default;
  int run();
  void shutdown();
  void init(std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<Order>>> commandQueue);
  void sendCmd();
  std::shared_ptr<Order> checkCmd(const std::string &cmd);

 private:
  std::atomic<bool> mIsShutdown = false;
  std::shared_ptr<MpscDoubleBufferQueue<std::shared_ptr<Order>>> mOrderQueue;

  void usage();
};

}  // namespace orderbook

#endif  // SRC_ADMIN_CLIENT_H_
