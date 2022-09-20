#include <cassert>
#include <numeric>
#include <unordered_map>
#include <boost/container_hash/hash.hpp>
#include "constant.hpp"
#include "win_prob.hpp"

struct Hash {
  std::size_t operator()(const std::vector<int>& hand) const
  {
    return boost::hash_range(hand.begin(), hand.end());
  }
};

std::unordered_map<std::vector<int>, std::valarray<double>, Hash> cache;

std::valarray<double> WinProb::select1(std::vector<int>& hand,
                                       int num,
                                       const int sht,
                                       const int64_t wait,
                                       const Params& params)
{
  if (const auto itr = cache.find(hand); itr != cache.end()) {
    return itr->second;
  }

  int sum = 0;
  std::valarray<double> tmp(0., params.t_max + 1);

  for (int i = 0; i < K; ++i) {
    if (hand[i] < 4 && (wait & (1LL << i))) {
      const int a = 4 - hand[i];

      ++hand[i];
      ++num;

      sum += a;

      if (sht == 1) {
        tmp += a * select2(hand, num, 0, 0LL, params);
      }
      else {
        const auto [_sht, _mode, _disc, _wait] = calsht(hand, num / 3, mode_in);
        tmp += a * select2(hand, num, _sht, _disc, params);
      }

      --hand[i];
      --num;
    }
  }

  std::valarray<double> ret(0., params.t_max + 1);

  for (int t = params.t_max - 1; t >= params.t_min; --t) {
    ret[t] = (1. - static_cast<double>(sum) / (params.sum - t)) * ret[t + 1] + tmp[t + 1] / (params.sum - t);
  }

  return cache[hand] = ret;
}

std::valarray<double> WinProb::select2(std::vector<int>& hand,
                                       int num,
                                       const int sht,
                                       const int64_t disc,
                                       const Params& params)
{
  if (const auto itr = cache.find(hand); itr != cache.end()) {
    return itr->second;
  }

  if (sht == 0) {
    return std::valarray(1., params.t_max + 1);
  }

  std::valarray<double> ret(0., params.t_max + 1);

  for (int i = 0; i < K; ++i) {
    if (hand[i] > 0 && (disc & (1LL << i))) {
      --hand[i];
      --num;

      const auto [_sht, _mode, _disc, _wait] = calsht(hand, num / 3, mode_in);

      ret = std::max(ret,
                     select1(hand, num, _sht, _wait, params),
                     [&params](const auto& x, const auto& y) {
                       return x[params.t_curr] < y[params.t_curr];
                     });

      ++hand[i];
      ++num;
    }
  }

  return cache[hand] = ret;
}

std::tuple<std::vector<Stat>, std::size_t> WinProb::operator()(std::vector<int>& hand, const Params& params)
{
  assert(params.t_min >= 0);
  assert(params.t_min < params.t_max);
  assert(params.t_curr >= params.t_min);
  assert(params.t_curr <= params.t_max);
  assert(params.t_curr >= 1);

  const int num = std::accumulate(hand.begin(), hand.end(), 0);

  assert(num % 3 == 2);

  std::vector<Stat> stats;

  const auto [sht, mode, disc, wait] = calsht(hand, num / 3, mode_in);

  cache.clear();

  for (int i = 0; i < K; ++i) {
    if (hand[i] > 0 && (disc & (1LL << i))) {
      --hand[i];

      const auto [_sht, _mode, _disc, _wait] = calsht(hand, num / 3, mode_in);
      const auto tmp = select1(hand, num - 1, _sht, _wait, params);

      stats.emplace_back(Stat{i, tmp});

      ++hand[i];
    }
  }

  const auto searched = cache.size();

  cache.clear();

  return {stats, searched};
}
