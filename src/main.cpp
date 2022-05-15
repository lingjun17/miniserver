/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#include "app/App.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/spdlog.h"


int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout << "Usage: " << std::endl;
    std::cout << "build/orderbook <path of log> <path of store>" << std::endl;
    std::cout << "eg. build/orderbook /tmp/log /tmp/data" << std::endl;
    return -1;
  }
  spdlog::stdout_logger_mt("stdout");
  spdlog::flush_on(spdlog::level::info);
  spdlog::set_pattern("[%D %H:%M:%S.%F] [%s:%# %!] [%l] [thread %t] %v");
  /// create a rotating file logger with 1GB size max and 10 rotated files
  if (strcmp(argv[1], "stdout") != 0) {
    auto logger = spdlog::rotating_logger_mt("app_logger", argv[1], 1024 * 1024 * 1024 * 1, 100);
    spdlog::set_default_logger(logger);
  }

  SPDLOG_INFO("pid={}", getpid());
  orderbook::App(argv[2]).run();
  return 0;
}
