#ifndef MJFFS_HPP
#define MJFFS_HPP

#include <cstdint>
#include <optional>
#include <tuple>
#include <valarray>
#include <vector>
#include "calsht_dw.hpp"

struct Stat {
  const int tile;                   // 打牌
  const std::valarray<double> prob; // 和了確率
};

struct Params {
  int t_min = 0;  // 最小巡目
  int t_max = 0;  // 最大巡目
  int t_curr = 0; // 現在の巡目
  int sum = 0;    // 最小巡目での牌の枚数
};

class WinProb {
private:
  const CalshtDW& calsht;
  const int mode_in;

  std::valarray<double> select1(std::vector<int>& hand,
                                int num,
                                int sht,
                                int64_t wait,
                                const Params& params);
  std::valarray<double> select2(std::vector<int>& hand,
                                int num,
                                int sht,
                                int64_t disc,
                                const Params& params);

public:
  WinProb(const CalshtDW& _calsht, const int _mode_in)
      : calsht(_calsht), mode_in(_mode_in) {}
  std::tuple<std::vector<Stat>, std::size_t> operator()(std::vector<int>& hand, const Params& params);
};

#endif
