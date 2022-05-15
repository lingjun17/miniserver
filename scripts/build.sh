#!/bin/sh
set -e
mkdir -p build
cd build && CXX=g++-9 CC=gcc-9 cmake -DCMAKE_CXX_FLAGS="-O2" -DCMAKE_C_FLAGS="-O2" .. &&
  echo "running cpplint" && make check &&
  echo "building targets" && make -j4 &&
  build/TestRunner
