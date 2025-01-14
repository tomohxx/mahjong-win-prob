#include "win_prob1.hpp"
#include <cassert>
#include <numeric>
#define SKIP_WHEN_THREE_PLAYER \
  if (params.three_player && i > 0 && i < 8) continue;
constexpr int NUM_TIDS = 34;

namespace win_prob::win_prob1 {
  std::valarray<double> WinProb1::select1(Cache& cache,
                                          std::vector<int>& hand,
                                          const int num,
                                          const Params& params) const
  {
    if (const auto itr = cache.find(hand); itr != cache.end()) {
      return itr->second;
    }

    const auto [sht, mode, disc, wait] = (*calsht)(hand, num / 3, params.mode, false, params.three_player);
    int sum = 0;
    std::valarray<double> tmp(0., params.t_max + 1);

    for (int i = 0; i < NUM_TIDS; ++i) {
      SKIP_WHEN_THREE_PLAYER

      if (wait & (1ull << i)) {
        const int weight = 4 - hand[i];

        ++hand[i];
        sum += weight;
        tmp += weight * (sht == 1
                             ? std::valarray<double>(1., params.t_max + 1)
                             : select2(cache, hand, num + 1, params));
        --hand[i];
      }
    }

    std::valarray<double> ret(0., params.t_max + 1);

    for (int t = params.t_max - 1; t >= params.t_min; --t) {
      ret[t] = (1. - static_cast<double>(sum) / (params.sum - t)) * ret[t + 1] + tmp[t + 1] / (params.sum - t);
    }

    return cache[hand] = ret;
  }

  std::valarray<double> WinProb1::select2(Cache& cache,
                                          std::vector<int>& hand,
                                          const int num,
                                          const Params& params) const
  {
    if (const auto itr = cache.find(hand); itr != cache.end()) {
      return itr->second;
    }

    const auto [sht, mode, disc, wait] = (*calsht)(hand, num / 3, params.mode, false, params.three_player);
    std::valarray<double> ret(0., params.t_max + 1);

    for (int i = 0; i < NUM_TIDS; ++i) {
      SKIP_WHEN_THREE_PLAYER

      if (disc & (1ull << i)) {
        --hand[i];

        const auto tmp = select1(cache, hand, num - 1, params);

        for (int j = params.t_min; j <= params.t_max; ++j) {
          ret[j] = std::max(ret[j], tmp[j]);
        }

        ++hand[i];
      }
    }

    return cache[hand] = ret;
  }

  std::tuple<std::vector<Stat>, std::size_t> WinProb1::operator()(std::vector<int>& hand,
                                                                  const Params params) const
  {
    assert(params.t_min >= 0);
    assert(params.t_min < params.t_max);

    const int num = std::accumulate(hand.begin(), hand.end(), 0);

    assert(num % 3 == 2);

    std::vector<Stat> stats;

    Cache cache;

    select2(cache, hand, num, params);

    for (int i = 0; i < NUM_TIDS; ++i) {
      if (hand[i] > 0) {
        --hand[i];

        if (const auto itr = cache.find(hand); itr != cache.end()) {
          stats.emplace_back(i, itr->second);
        }

        ++hand[i];
      }
    }

    const auto searched = cache.size();

    return {stats, searched};
  }
}
