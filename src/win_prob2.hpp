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

struct Hash {
  std::size_t operator()(const std::vector<int>& hand) const
  {
    return boost::hash_range(hand.begin(), hand.end());
  }
};

using Desc = std::unordered_map<std::vector<int>, Graph::vertex_descriptor, Hash>;

class WinProb2 {
private:
  const CalshtDW& calsht;

  Graph::vertex_descriptor select1(Graph& graph,
                                   Desc& desc1,
                                   Desc& desc2,
                                   std::vector<int>& hand,
                                   int num,
                                   const std::vector<int>& origin,
                                   int sht_org,
                                   const Params& params);
  Graph::vertex_descriptor select2(Graph& graph,
                                   Desc& desc1,
                                   Desc& desc2,
                                   std::vector<int>& hand,
                                   int num,
                                   const std::vector<int>& origin,
                                   int sht_org,
                                   const Params& params);
  void update(Graph& graph, const Desc& desc1, const Desc& desc2, const Params& params);

public:
  WinProb2(const CalshtDW& _calsht)
      : calsht(_calsht) {}
  std::tuple<std::vector<Stat>, std::size_t> operator()(std::vector<int>& hand, const Params& params);
};

#endif
