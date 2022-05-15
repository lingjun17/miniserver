/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#include <regex>
#include "BigDecimal.h"

namespace orderbook {

BigDecimal::BigDecimal(const std::string &number) : mValue(number) {}

BigDecimal::BigDecimal(uint64_t number) : mValue(number) {}

BigDecimal::BigDecimal(boost::multiprecision::cpp_dec_float_100 number) : mValue(std::move(number)) {}

BigDecimal::BigDecimal() : mValue(0) {}


std::string BigDecimal::toString() const {
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << mValue;
  return stream.str();
}

bool BigDecimal::isValid(const std::string &number) {
  try {
    auto value = boost::multiprecision::cpp_dec_float_50(number);
    value = value + 1;
    return true;
  }
  catch (std::runtime_error&) {
    return false;
  }
}


}  /// namespace orderbook
