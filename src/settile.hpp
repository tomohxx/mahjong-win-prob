#ifndef SETTILE_HPP
#define SETTILE_HPP

#include <string>
#include <vector>

void set_tile(const std::string& str,
              std::vector<int>& t,
              bool three_player = false);
std::string get_tile(const std::vector<int>& t);

#endif
