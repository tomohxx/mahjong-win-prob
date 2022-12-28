#include <cassert>
#include <numeric>
#include <unordered_map>
#include <boost/graph/graph_utility.hpp>
#include "constant.hpp"
#include "win_prob2.hpp"

int64_t calc_disc2(const std::vector<int>& hand)
{
  int64_t ret = 0LL;

  for (int i = 0; i < K; ++i) {
    if (hand[i] > 0) {
      ret |= 1LL << i;
    }
  }

  return ret;
}

int64_t calc_wait2(const std::vector<int>& hand)
{
  int64_t ret = 0LL;

  for (int i = 0; i < K; ++i) {
    if (hand[i] < 4) {
      ret |= 1LL << i;
    }
  }

  return ret;
}

int distance(const std::vector<int>& hand, const std::vector<int>& origin)
{
  return std::inner_product(hand.begin(), hand.end(),
                            origin.begin(),
                            0,
                            std::plus<int>(),
                            [](const int x, const int y) { return std::max(x - y, 0); });
}

Graph::vertex_descriptor WinProb2::select1(Graph& graph,
                                           Desc& desc1,
                                           Desc& desc2,
                                           std::vector<int>& hand,
                                           const int num,
                                           const std::vector<int>& origin,
                                           const int sht_org,
                                           const Params& params)
{
  if (const auto itr = desc1.find(hand); itr != desc1.end()) {
    return itr->second;
  }

  const auto [sht, mode, disc, wait] = calsht(hand, num / 3, mode_in);
  const auto all = distance(hand, origin) + sht < sht_org + params.extra ? calc_wait2(hand) : wait;
  const auto desc = boost::add_vertex(std::valarray<double>(0., params.t_max + 1), graph);

  desc1[hand] = desc;

  for (int i = 0; i < K; ++i) {
    if (all & (1LL << i)) {
      const int weight = 4 - (hand[i]++);
      const auto target = select2(graph, desc1, desc2, hand, num + 1, origin, sht_org, params);

      if (!boost::edge(desc, target, graph).second) {
        boost::add_edge(desc, target, weight, graph);
      }

      --hand[i];
    }
  }

  return desc;
}

Graph::vertex_descriptor WinProb2::select2(Graph& graph,
                                           Desc& desc1,
                                           Desc& desc2,
                                           std::vector<int>& hand,
                                           const int num,
                                           const std::vector<int>& origin,
                                           const int sht_org,
                                           const Params& params)
{
  if (const auto itr = desc2.find(hand); itr != desc2.end()) {
    return itr->second;
  }

  const auto [sht, mode, disc, wait] = calsht(hand, num / 3, mode_in);
  const auto all = distance(hand, origin) + sht < sht_org + params.extra ? calc_disc2(hand) : disc;
  const auto desc = boost::add_vertex(std::valarray<double>(!sht, params.t_max + 1), graph);

  desc2[hand] = desc;

  for (int i = 0; i < K; ++i) {
    if (all & (1LL << i)) {
      const int weight = 4 - (--hand[i]);
      const auto source = select1(graph, desc1, desc2, hand, num - 1, origin, sht_org, params);

      if (!boost::edge(source, desc, graph).second) {
        boost::add_edge(source, desc, weight, graph);
      }

      ++hand[i];
    }
  }

  return desc;
}

void WinProb2::update(Graph& graph, const Desc& desc1, const Desc& desc2, const Params& params)
{
  for (int t = params.t_max - 1; t >= params.t_min; --t) {
    for (auto& [hand, desc] : desc1) {
      auto& prob = graph[desc];

      for (auto [first, last] = boost::out_edges(desc, graph); first != last; ++first) {
        const auto target = boost::target(*first, graph);

        prob[t] += graph[*first] * (graph[target][t + 1] - prob[t + 1]);
      }

      prob[t] = prob[t + 1] + prob[t] / (params.sum - t);
    }

    for (auto& [hand, desc] : desc2) {
      auto& prob = graph[desc];

      for (auto [first, last] = boost::in_edges(desc, graph); first != last; ++first) {
        const auto source = boost::source(*first, graph);

        prob[t] = std::max(prob[t], graph[source][t]);
      }
    }
  }
}

std::tuple<std::vector<Stat>, std::size_t> WinProb2::operator()(std::vector<int>& hand, const Params& params)
{
  assert(params.t_min >= 0);
  assert(params.t_min < params.t_max);

  const int num = std::accumulate(hand.begin(), hand.end(), 0);

  assert(num % 3 == 2);

  std::vector<Stat> stats;

  const auto [sht, mode, disc, wait] = calsht(hand, num / 3, mode_in);

  Graph graph;
  Desc desc1, desc2;

  select2(graph, desc1, desc2, hand, num, std::vector<int>{hand}, sht, params);
  update(graph, desc1, desc2, params);

  for (int i = 0; i < K; ++i) {
    if (hand[i] > 0) {
      --hand[i];

      if (const auto itr = desc1.find(hand); itr != desc1.end()) {
        stats.emplace_back(Stat{i, graph[itr->second]});
      }
      ++hand[i];
    }
  }

  const auto searched = boost::num_vertices(graph);

  return {stats, searched};
}
