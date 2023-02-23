#ifndef WIN_PROB_HPP
#define WIN_PROB_HPP

#include <tuple>
#include <vector>
#include <unordered_map>
#include <boost/container_hash/hash.hpp>
#include "calsht_dw.hpp"
#include "utils.hpp"

struct Hash {
  std::size_t operator()(const std::vector<int>& hand) const
  {
    return boost::hash_range(hand.begin(), hand.end());
  }
};

using Cache = std::unordered_map<std::vector<int>, std::valarray<double>, Hash>;

class WinProb {
private:
  const CalshtDW& calsht;

  std::valarray<double> select1(Cache& cache,
                                std::vector<int>& hand,
                                int num,
                                const Params& params);
  std::valarray<double> select2(Cache& cache,
                                std::vector<int>& hand,
                                int num,
                                const Params& params);

public:
  WinProb(const CalshtDW& _calsht)
      : calsht(_calsht) {}
  std::tuple<std::vector<Stat>, std::size_t> operator()(std::vector<int>& hand, const Params& params);
};

#endif
