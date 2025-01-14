#ifndef WIN_PROB1_HPP
#define WIN_PROB1_HPP

#include "necessary-and-unnecessary-tiles/src/calsht_dw.hpp"
#include "utils.hpp"
#include <boost/container_hash/hash.hpp>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace win_prob::win_prob1 {
  struct Hash {
    std::size_t operator()(const std::vector<int>& hand) const
    {
      return boost::hash_range(hand.begin(), hand.end());
    }
  };

  using Cache = std::unordered_map<std::vector<int>, std::valarray<double>, Hash>;

  class WinProb1 {
  private:
    const std::shared_ptr<CalshtDW> calsht;

    std::valarray<double> select1(Cache& cache,
                                  std::vector<int>& hand,
                                  int num,
                                  const Params& params) const;
    std::valarray<double> select2(Cache& cache,
                                  std::vector<int>& hand,
                                  int num,
                                  const Params& params) const;

  public:
    explicit WinProb1(std::shared_ptr<CalshtDW> _calsht)
        : calsht(_calsht) {}
    std::tuple<std::vector<Stat>, std::size_t> operator()(std::vector<int>& hand,
                                                          const Params params) const;
  };
}

#endif
