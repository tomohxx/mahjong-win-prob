#include <bitset>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include "calsht_dw.hpp"
#include "settile.hpp"
#include "win_prob.hpp"

int main()
{
  constexpr int M = 14;
  constexpr int MODE = 7;
  std::string str;
  std::vector<int> hand(K, 0);
  CalshtDW calsht;
  WinProb win_prob(calsht, MODE);
  std::map<int, std::string> label = {
      {0, "1m"},
      {1, "2m"},
      {2, "3m"},
      {3, "4m"},
      {4, "5m"},
      {5, "6m"},
      {6, "7m"},
      {7, "8m"},
      {8, "9m"},
      {9, "1p"},
      {10, "2p"},
      {11, "3p"},
      {12, "4p"},
      {13, "5p"},
      {14, "6p"},
      {15, "7p"},
      {16, "8p"},
      {17, "9p"},
      {18, "1s"},
      {19, "2s"},
      {20, "3s"},
      {21, "4s"},
      {22, "5s"},
      {23, "6s"},
      {24, "7s"},
      {25, "8s"},
      {26, "9s"},
      {27, "1z"},
      {28, "2z"},
      {29, "3z"},
      {30, "4z"},
      {31, "5z"},
      {32, "6z"},
      {33, "7z"},
  };

  calsht.initialize(INDEX_FILE_PATH);

  std::cout << "Enter " << M << " tiles" << std::endl;
  std::cin >> str;

  set_tile(str, hand);

  const auto [sht, mode, disc, wait] = calsht(hand, M / 3, MODE);

  std::cout << "The shanten number is " << sht - 1 << std::endl;

  Params params{
      .t_min = 0,
      .t_max = 18,
      .t_curr = 1,
      .sum = 123,
  };

  const auto start = std::chrono::system_clock::now();
  const auto [stats, searched] = win_prob(hand, params);
  const auto end = std::chrono::system_clock::now();

  std::cout << "=== Params ===" << std::endl;
  std::cout << "t_min:  " << params.t_min << std::endl;
  std::cout << "t_max:  " << params.t_max << std::endl;
  std::cout << "t_curr: " << params.t_curr << std::endl;
  std::cout << "sum:    " << params.sum << std::endl;
  std::cout << "=== Result ===" << std::endl;
  std::cout << "Turn\t";

  for (const auto& stat : stats) {
    std::cout << label[stat.tile] << "\t";
  }

  std::cout << std::endl;
  std::cout << std::fixed;

  for (int t = params.t_curr; t <= params.t_max; ++t) {
    std::cout << t << "\t";

    for (const auto& stat : stats) {
      std::cout << std::setprecision(4) << stat.prob[t] << "\t";
    }

    std::cout << std::endl;
  }

  std::cout << "==== Info ====" << std::endl;
  std::cout << "Time (msec.): "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << std::endl;
  std::cout << "Searched:     " << searched << std::endl;

  return 0;
}
