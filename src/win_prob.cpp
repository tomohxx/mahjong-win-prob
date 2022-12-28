#include <cassert>
#include <numeric>
#include "constant.hpp"
#include "win_prob.hpp"

std::valarray<double> WinProb::select1(Cache& cache,
                                       std::vector<int>& hand,
                                       const int num,
                                       const Params& params)
{
  if (const auto itr = cache.find(hand); itr != cache.end()) {
    return itr->second;
  }

  const auto [sht, mode, disc, wait] = calsht(hand, num / 3, mode_in);
  int sum = 0;
  std::valarray<double> tmp(0., params.t_max + 1);

  for (int i = 0; i < K; ++i) {
    if (wait & (1LL << i)) {
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

std::valarray<double> WinProb::select2(Cache& cache,
                                       std::vector<int>& hand,
                                       const int num,
                                       const Params& params)
{
  if (const auto itr = cache.find(hand); itr != cache.end()) {
    return itr->second;
  }

  const auto [sht, mode, disc, wait] = calsht(hand, num / 3, mode_in);
  std::valarray<double> ret(0., params.t_max + 1);

  for (int i = 0; i < K; ++i) {
    if (disc & (1LL << i)) {
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

std::tuple<std::vector<Stat>, std::size_t> WinProb::operator()(std::vector<int>& hand, const Params& params)
{
  assert(params.t_min >= 0);
  assert(params.t_min < params.t_max);

  const int num = std::accumulate(hand.begin(), hand.end(), 0);

  assert(num % 3 == 2);

  std::vector<Stat> stats;

  Cache cache;

  select2(cache, hand, num, params);

  for (int i = 0; i < K; ++i) {
    if (hand[i] > 0) {
      --hand[i];

      if (const auto itr = cache.find(hand); itr != cache.end()) {
        stats.emplace_back(Stat{i, itr->second});
      }
      ++hand[i];
    }
  }

  const auto searched = cache.size();

  return {stats, searched};
}
