#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include "calsht_dw.hpp"
#include "settile.hpp"
#ifdef WIN_PROB2
#include "win_prob2.hpp"
#else
#include "win_prob.hpp"
#endif

int main()
{
  constexpr int M = 14;
  constexpr int MODE = 7;
  std::string str;
  std::vector<int> hand(K, 0);
  CalshtDW calsht;
#ifdef WIN_PROB2
  WinProb2 win_prob(calsht);
#else
  WinProb win_prob(calsht);
#endif
  std::array<std::string, K> label = {
      "1m", "2m", "3m", "4m", "5m", "6m", "7m", "8m", "9m",
      "1p", "2p", "3p", "4p", "5p", "6p", "7p", "8p", "9p",
      "1s", "2s", "3s", "4s", "5s", "6s", "7s", "8s", "9s",
      "1z", "2z", "3z", "4z", "5z", "6z", "7z"};

  calsht.initialize(INDEX_FILE_PATH);

  Params params{
      .t_min = 1,
      .t_max = 18,
      .sum = 123,
      .mode = MODE,
  };

  std::cout << "Enter " << M << " tiles" << std::endl;
  std::cin >> str;
#ifdef WIN_PROB2
  std::cout << "Enter the number of extra tiles" << std::endl;
  std::cin >> params.extra;
#endif

  set_tile(str, hand);

  const auto [sht, mode, disc, wait] = calsht(hand, M / 3, MODE);

  std::cout << "The shanten number is " << sht - 1 << std::endl;

  const auto start = std::chrono::system_clock::now();
  const auto [stats, searched] = win_prob(hand, params);
  const auto end = std::chrono::system_clock::now();

  std::cout << "=== Params ===\n";
  std::cout << "t_min:  " << params.t_min << "\n";
  std::cout << "t_max:  " << params.t_max << "\n";
  std::cout << "sum:    " << params.sum << "\n";
  std::cout << "extra:  " << params.extra << "\n";
  std::cout << "mode:   " << params.mode << "\n";
  std::cout << "=== Result ===\n";
  std::cout << std::setw(8) << std::left << "Turn";

  for (const auto& stat : stats) {
    std::cout << std::setw(8) << label[stat.tile];
  }

  std::cout << "\n";
  std::cout << std::fixed;

  for (int t = params.t_min; t <= params.t_max; ++t) {
    std::cout << std::setw(8) << t;

    for (const auto& stat : stats) {
      std::cout << std::setw(8) << std::setprecision(4) << stat.prob[t];
    }

    std::cout << "\n";
  }

  std::cout << "==== Info ====\n";
  std::cout << "Time (msec.): "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << "\n";
  std::cout << "Searched:     " << searched << "\n";

  return 0;
}
