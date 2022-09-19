#ifndef SETTILE_HPP
#define SETTILE_HPP

#include <regex>
#include <stdexcept>
#include <string>
#include "constant.hpp"

template <class T>
inline void set_tile(const std::string& str, T& t)
{
  static std::regex re("^([1-9]+[mMpPsS]|[1-7]+z)+$");

  if (!std::regex_match(str, re)) {
    throw std::logic_error("Illegal hand expression");
  }

  int n = 0;

  for (auto itr = str.rbegin(); itr != str.rend(); ++itr) {
    if (*itr >= '1' && *itr <= '9') {
      ++t[*itr - '1' + n];
    }
    else {
      switch (*itr) {
      case 'm': n = 0; break;
      case 'M': n = 0; break;
      case 'p': n = 9; break;
      case 'P': n = 9; break;
      case 's': n = 18; break;
      case 'S': n = 18; break;
      case 'z': n = 27; break;
      default: throw std::runtime_error("Unexpected character");
      }
    }
  }

  for (int i = 0; i < K; ++i) {
    if (t[i] < 0 || t[i] > 4) {
      throw std::runtime_error("Abnormal number of hand's tiles");
    }
  }
}

template <class T>
inline std::string get_tile(const T& t)
{
  std::string str;

  for (int i = 0; i < K; ++i) {
    for (int j = 0; j < t[i]; ++j) {
      str += i % 9 + '1';

      switch (i / 9) {
      case 0: str += 'm'; break;
      case 1: str += 'p'; break;
      case 2: str += 's'; break;
      case 3: str += 'z'; break;
      }
    }
  }
  return str;
}

#endif
