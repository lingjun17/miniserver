#!/bin/bash
# run this script under project's dir
set -x
DIR=$(pwd)
# this script sets up all dependent submodules
SUMMARY=$(git submodule summary)
# cppint
CPPLINT="third_party/cpplint"
if [[ "$SUMMARY" == *"$CPPLINT"* ]]; then
  echo "Skipping $CPPLINT"
else
  cd "$DIR" || return
  git submodule add -f https://github.com/cpplint/cpplint "$CPPLINT"
  cd "$CPPLINT" || return
  git checkout 46b8c79
fi
# gtest
GTEST="third_party/gtest"
if [[ "$SUMMARY" == *"$GTEST"* ]]; then
  echo "Skipping $GTEST"
else
  cd "$DIR" || return
  git submodule add -f https://github.com/google/googletest.git "$GTEST"
  cd "$GTEST" || return
  git checkout e2239ee
fi
# spdlog
SPDLOG="third_party/spdlog"
if [[ "$SUMMARY" == *"$SPDLOG"* ]]; then
  echo "Skipping $SPDLOG"
else
  cd "$DIR" || return
  git submodule add -f https://github.com/gabime/spdlog.git "$SPDLOG"
  cd "$SPDLOG" || return
  git checkout 76fb40d
fi
# abseil-cpp
ABSL="third_party/abseil-cpp"
if [[ "$SUMMARY" == *"$ABSL"* ]]; then
  echo "Skipping $ABSL"
else
  cd "$DIR" || return
  git submodule add -f https://github.com/abseil/abseil-cpp.git "$ABSL"
  cd "$ABSL" || return
  git checkout 2151058
fi