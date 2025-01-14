#include "necessary-and-unnecessary-tiles/src/calsht_dw.hpp"
#include "settile.hpp"
#include "win_prob1.hpp"
#include "win_prob2.hpp"
#include <array>
#include <chrono>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#ifndef INDEX_FILE_PATH
#define INDEX_FILE_PATH std::filesystem::current_path()
#endif
#ifndef WIN_PROB2
#define WIN_PROB2 false
#endif

int main()
{
  constexpr int NUM_TIDS = 34;
  constexpr int NUM_TILES = 14;
  constexpr int MODE = 7;
  std::string str;
  std::vector<int> hand(NUM_TIDS, 0);
  const auto calsht = std::make_shared<CalshtDW>();
  const std::conditional<WIN_PROB2,
                         win_prob::win_prob2::WinProb2,
                         win_prob::win_prob1::WinProb1>::type win_prob(calsht);
  const std::array<std::string, NUM_TIDS> label = {
      "1m", "2m", "3m", "4m", "5m", "6m", "7m", "8m", "9m",
      "1p", "2p", "3p", "4p", "5p", "6p", "7p", "8p", "9p",
      "1s", "2s", "3s", "4s", "5s", "6s", "7s", "8s", "9s",
      "1z", "2z", "3z", "4z", "5z", "6z", "7z"};

  calsht->initialize(INDEX_FILE_PATH);

  win_prob::Params params{
      .t_min = 1,
      .t_max = 18,
      .mode = MODE,
  };

  std::cout << std::format("Enter {} tiles\n", NUM_TILES);
  std::cin >> str;

  if constexpr (WIN_PROB2) {
    std::cout << "Enter the number of extra tiles" << std::endl;
    std::cin >> params.extra;
  }

  std::cout << "Enter whether three-player(1/0)" << std::endl;
  std::cin >> params.three_player;

  params.sum = params.three_player ? 95 : 123;

  set_tile(str, hand, params.three_player);

  const auto [sht, mode, disc, wait] = (*calsht)(hand, NUM_TILES / 3, MODE, false, params.three_player);

  std::cout << "The shanten number is " << sht - 1 << std::endl;

  const auto start = std::chrono::system_clock::now();
  const auto [stats, searched] = win_prob(hand, params);
  const auto end = std::chrono::system_clock::now();

  std::cout << "=== Params ===\n";
  std::cout << std::format("t_min:        {:d}\n", params.t_min);
  std::cout << std::format("t_max:        {:d}\n", params.t_max);
  std::cout << std::format("sum:          {:d}\n", params.sum);
  std::cout << std::format("extra:        {:d}\n", params.extra);
  std::cout << std::format("mode:         {:d}\n", params.mode);
  std::cout << std::format("three-player: {:s}\n", params.three_player);
  std::cout << "=== Result ===\n";
  std::cout << std::format("{:<8s}", "Turn");

  for (const auto& stat : stats) {
    std::cout << std::format("{:<8s}", label[stat.tile]);
  }

  std::cout << "\n";

  for (int t = params.t_min; t <= params.t_max; ++t) {
    std::cout << std::format("{:<8d}", t);

    for (const auto& stat : stats) {
      std::cout << std::format("{:<8.4f}", stat.prob[t]);
    }

    std::cout << "\n";
  }

  std::cout << "==== Info ====\n";
  std::cout << std::format("Time (msec.): {:d}\n",
                           std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
  std::cout << std::format("Searched:     {:d}\n", searched);

  return 0;
}
