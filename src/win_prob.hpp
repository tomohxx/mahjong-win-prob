#ifndef WIN_PROB_HPP
#define WIN_PROB_HPP

#include <tuple>
#include <vector>
#include "calsht_dw.hpp"
#include "utils.hpp"

class WinProb {
private:
  const CalshtDW& calsht;
  const int mode_in;

  std::valarray<double> select1(std::vector<int>& hand,
                                int num,
                                int sht,
                                const Params& params);
  std::valarray<double> select2(std::vector<int>& hand,
                                int num,
                                int sht,
                                const Params& params);

public:
  WinProb(const CalshtDW& _calsht, const int _mode_in)
      : calsht(_calsht), mode_in(_mode_in) {}
  std::tuple<std::vector<Stat>, std::size_t> operator()(std::vector<int>& hand, const Params& params);
};

#endif
