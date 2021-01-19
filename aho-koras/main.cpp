#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

#include "Bohr.hpp"

int main() {
  using MyBohr = Bohr<uint32_t>;
  using symbol_t = MyBohr::symbol_t;
  using pattern_t = MyBohr::pattern_t;
  MyBohr b; 
  std::vector<pattern_t> patterns{{1,2,3,0}, {1,2,0}, {2,0}, {3,1}, {2,3,4}};
  std::vector<symbol_t> text{1,2,0,4,1,2,3,0,1,2,3,0,};

  std::for_each(patterns.cbegin(), patterns.cend(), [&b](const pattern_t &p) {
    b.add_pattern(p);
    std::cout << "added_pattern:\t";
    std::copy(p.begin(), p.end(), std::ostream_iterator<symbol_t>(std::cout, " "));
    std::cout << std::endl;
  });
  std::cout << "--------------------------------\n";

  std::vector<pattern_t> false_patterns_to_check{{4,5}, {1,0}, {1,2,3}, {3,4}, {2,3,1}};

  std::cout << "false patterns:\n";

  std::for_each(false_patterns_to_check.begin(), false_patterns_to_check.end(), [&b](const pattern_t &p) {
    std::ostringstream ss;
    std::copy(p.begin(), p.end(), std::ostream_iterator<symbol_t>(ss, " "));
    std::cout << std::setw(15) << std::left << ss.str() << ":\t" << std::boolalpha << b.contains(p) << std::endl;
  });

  std::cout << "\ntrue patterns:\n";

  std::for_each(patterns.begin(), patterns.end(), [&b](const pattern_t &p) {
    std::ostringstream ss;
    std::copy(p.begin(), p.end(), std::ostream_iterator<symbol_t>(ss, " "));
    std::cout << std::setw(15) << std::left << ss.str() << ":\t" << std::boolalpha << b.contains(p) << std::endl;
  });

  auto res = b.find(text);
  std::cout << "--------------------------------\n";
  std::cout << "text: ";
  std::copy(text.begin(), text.end(), std::ostream_iterator<symbol_t>(std::cout, " "));
  std::cout << std::endl;
  for (const auto &el : res) {
    std::cout << "{ pattern_number: " << el.first << ", i: " << el.second << " }" << std::endl;
  }
  return 0;
}
