#ifndef WIN_PROB2_HPP
#define WIN_PROB2_HPP

#include <tuple>
#include <vector>
#include <boost/container_hash/hash.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "calsht_dw.hpp"
#include "utils.hpp"
using Graph = boost::adjacency_list<boost::listS,
                                    boost::vecS,
                                    boost::bidirectionalS,
                                    std::valarray<double>,
                                    int>;

class WinProb2 {
private:
  struct Hash {
    std::size_t operator()(const std::vector<int>& hand) const
    {
      return boost::hash_range(hand.begin(), hand.end());
    }
  };

  const CalshtDW& calsht;
  const int mode_in;

  Graph graph;
  std::unordered_map<std::vector<int>, Graph::vertex_descriptor, Hash> desc1;
  std::unordered_map<std::vector<int>, Graph::vertex_descriptor, Hash> desc2;

  Graph::vertex_descriptor select1(std::vector<int>& hand,
                                   int num,
                                   const std::vector<int>& origin,
                                   int sht_org,
                                   const Params& params);
  Graph::vertex_descriptor select2(std::vector<int>& hand,
                                   int num,
                                   const std::vector<int>& origin,
                                   int sht_org,
                                   const Params& params);
  void update(const Params& params);

public:
  WinProb2(const CalshtDW& _calsht, const int _mode_in)
      : calsht(_calsht), mode_in(_mode_in) {}
  std::tuple<std::vector<Stat>, std::size_t> operator()(std::vector<int>& hand, const Params& params);
};

#endif
