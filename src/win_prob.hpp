#ifndef MJFFS_HPP
#define MJFFS_HPP

#include <cstdint>
#include <optional>
#include <valarray>
#include <vector>
#include "calsht_dw.hpp"

struct Stat {
  const int tile;                   // 打牌
  const std::valarray<double> prob; // 和了確率
};

struct Params {
  const int t_min;  // 最小巡目
  const int t_max;  // 最大巡目
  const int t_curr; // 現在の巡目
  const int sum;    // 最小巡目での牌の枚数
};

class WinProb {
private:
  const CalshtDW& calsht;
  const int mode_in;

  std::valarray<double> select1(std::vector<int>& hand,
                                std::vector<int>& wall,
                                int num,
                                int sht,
                                int mode,
                                int64_t wait,
                                const Params& params);
  std::valarray<double> select2(std::vector<int>& hand,
                                std::vector<int>& wall,
                                int num,
                                int sht,
                                int mode,
                                int64_t disc,
                                const Params& params);

public:
  WinProb(const CalshtDW& _calsht, const int _mode_in)
      : calsht(_calsht), mode_in(_mode_in) {}
  std::vector<Stat> operator()(std::vector<int>& hand, const Params& params);
};

#endif
