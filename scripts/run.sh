#!/bin/sh
set -e
cd build && build/orderbook /tmp/orderbook.log /tmp/orderbook.data
