#include "settile.hpp"
#include <cassert>
#include <regex>
#include <stdexcept>
#include <string>
constexpr std::size_t NUM_TIDS = 34;
const std::regex re3("^(?:[19]+m|[1-9]+[pPsS]|[1-7]+z)+$");
const std::regex re4("^(?:[1-9]+[mMpPsS]|[1-7]+z)+$");

void set_tile(const std::string& str,
              std::vector<int>& t,
              const bool three_player)
{
  if (three_player) {
    if (!std::regex_match(str, re3)) {
      throw std::invalid_argument("Invalid hand expression");
    }
  }
  else {
    if (!std::regex_match(str, re4)) {
      throw std::invalid_argument("Invalid hand expression");
    }
  }

  int n = 0;

  for (auto itr = str.crbegin(); itr != str.crend(); ++itr) {
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
      default:
        throw std::invalid_argument("Invalid hand expression");
      }
    }
  }

  for (std::size_t i = 0; i < NUM_TIDS; ++i) {
    if (t[i] < 0 || t[i] > 4) {
      throw std::invalid_argument("Invalid hand expression");
    }
  }
}

std::string get_tile(const std::vector<int>& t)
{
  assert(t.size() == NUM_TIDS);

  std::string str;

  for (std::size_t i = 0; i < NUM_TIDS; ++i) {
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
