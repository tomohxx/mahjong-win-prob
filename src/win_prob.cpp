#include <cassert>
#include <numeric>
#include <unordered_map>
#include <boost/container_hash/hash.hpp>
#include "constant.hpp"
#include "win_prob.hpp"

struct Key {
  const std::vector<int> hand;
  const std::vector<int> wall;
};

struct Hash {
  std::size_t operator()(const Key& key) const
  {
    using boost::hash_combine;
    using boost::hash_range;

    std::size_t seed = 0;

    hash_combine(seed, hash_range(key.hand.begin(), key.hand.end()));
    hash_combine(seed, hash_range(key.wall.begin(), key.wall.end()));

    return seed;
  }
};

bool operator==(const Key& lhs, const Key& rhs)
{
  return lhs.hand == rhs.hand && lhs.wall == rhs.wall;
}

std::unordered_map<Key, std::valarray<double>, Hash> cache;

std::valarray<double> WinProb::select1(std::vector<int>& hand,
                                       std::vector<int>& wall,
                                       const int num,
                                       const int sht,
                                       const int mode,
                                       const int64_t wait,
                                       const Params& params)
{
  const Key key{hand, wall};

  if (const auto itr = cache.find(key); itr != cache.end()) {
    return itr->second;
  }

  int sum = 0;
  std::valarray<double> tmp(0., params.t_max + 1);

  for (int i = 0; i < K; ++i) {
    if (wait & (1LL << i) && wall[i] > 0) {
      const int a = wall[i];

      ++hand[i];
      --wall[i];

      const auto [_sht, _mode, _disc, _wait] = calsht(hand, num / 3, mode_in);

      sum += a;
      tmp += a * select2(hand, wall, num + 1, sht - 1, mode, _disc, params);

      --hand[i];
      ++wall[i];
    }
  }

  std::valarray<double> ret(0., params.t_max + 1);

  for (int t = params.t_max - 1; t >= params.t_min; --t) {
    ret[t] = (1. - static_cast<double>(sum) / (params.sum - t)) * ret[t + 1] + tmp[t + 1] / (params.sum - t);
  }

  return cache[key] = ret;
}

std::valarray<double> WinProb::select2(std::vector<int>& hand,
                                       std::vector<int>& wall,
                                       const int num,
                                       const int sht,
                                       const int mode,
                                       const int64_t disc,
                                       const Params& params)
{
  const Key key{hand, wall};

  if (const auto itr = cache.find(key); itr != cache.end()) {
    return itr->second;
  }

  if (sht == 0) {
    return std::valarray(1., params.t_max + 1);
  }

  std::valarray<double> ret(0., params.t_max + 1);

  for (int i = 0; i < K; ++i) {
    if (disc & (1LL << i)) {
      --hand[i];

      const auto [_sht, _mode, _disc, _wait] = calsht(hand, num / 3, mode_in);

      ret = std::max(ret,
                     select1(hand, wall, num - 1, sht, mode, _wait, params),
                     [&params](const auto& x, const auto& y) { return x[params.t_curr] < y[params.t_curr]; });

      ++hand[i];
    }
  }

  return cache[key] = ret;
}

std::vector<Stat> WinProb::operator()(std::vector<int>& hand, const Params& params)
{
  assert(params.t_min >= 0);
  assert(params.t_min < params.t_max);
  assert(params.t_curr >= params.t_min);
  assert(params.t_curr <= params.t_max);
  assert(params.t_curr >= 1);

  const int num = std::accumulate(hand.begin(), hand.end(), 0);

  assert(num % 3 == 2);

  std::vector<int> wall(K);

  for (int i = 0; i < K; ++i) {
    wall[i] = 4 - hand[i];
  }

  std::vector<Stat> ret;

  const auto [sht, mode, disc, wait] = calsht(hand, num / 3, mode_in);

  for (int i = 0; i < K; ++i) {
    if (disc & (1LL << i)) {
      --hand[i];

      const auto [_sht, _mode, _disc, _wait] = calsht(hand, num / 3, mode_in);
      const auto tmp = select1(hand, wall, num - 1, sht, mode_in, _wait, params);

      ret.emplace_back(Stat{i, tmp});

      ++hand[i];
    }
  }

  cache.clear();

  return ret;
}
