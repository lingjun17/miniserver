/************************************************************************
Copyright (c) 2022 JunLing. All rights reserved.
Use of this source code is governed by a MIT license that can be
found in the LICENSE file.
************************************************************************/

#ifndef SRC_UTILS_BIGDECIMAL_H_
#define SRC_UTILS_BIGDECIMAL_H_

#include <boost/multiprecision/cpp_dec_float.hpp>

namespace orderbook {

class BigDecimal {
 public:
  explicit BigDecimal(const std::string &number);
  explicit BigDecimal(uint64_t number);
  explicit BigDecimal(boost::multiprecision::cpp_dec_float_100 number);
  BigDecimal();

  std::string toString() const;

  boost::multiprecision::cpp_dec_float_100 getValue() const {
    return mValue;
  }

  static bool isValid(const std::string &number);

 private:
  boost::multiprecision::cpp_dec_float_100 mValue;
};

inline BigDecimal operator+(const BigDecimal &a, const BigDecimal &b) {
  return BigDecimal(a.getValue() + b.getValue());
}

inline BigDecimal operator-(const BigDecimal &a, const BigDecimal &b) {
  return BigDecimal(a.getValue() - b.getValue());
}

inline BigDecimal operator*(const BigDecimal &a, const BigDecimal &b) {
  return BigDecimal(a.getValue() * b.getValue());
}

inline BigDecimal operator/(const BigDecimal &a, const BigDecimal &b) {
  return BigDecimal(a.getValue() / b.getValue());
}

inline BigDecimal operator-(const BigDecimal &a) {
  return BigDecimal(-a.getValue());
}

inline bool operator<(const BigDecimal &a, const BigDecimal &b) {
  return a.getValue() < b.getValue();
}

inline bool operator>(const BigDecimal &a, const BigDecimal &b) {
  return a.getValue() > b.getValue();
}

inline bool operator==(const BigDecimal &a, const BigDecimal &b) {
  return a.getValue() == b.getValue();
}

inline bool operator!=(const BigDecimal &a, const BigDecimal &b) {
  return a.getValue() != b.getValue();
}

inline bool operator<=(const BigDecimal &a, const BigDecimal &b) {
  return a.getValue() <= b.getValue();
}

inline bool operator>=(const BigDecimal &a, const BigDecimal &b) {
  return a.getValue() >= b.getValue();
}

}  /// namespace orderbook

#endif  // SRC_UTILS_BIGDECIMAL_H_
