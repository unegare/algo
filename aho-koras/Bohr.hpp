#ifndef BOHR_HPP_
#define BOHR_HPP_

#include <iostream>
#include <map>
#include <vector>
#include <optional>
#include <iterator>

template<typename _symbol_t>
class Bohr {
public:
  using symbol_t = _symbol_t;
  using index_t = std::size_t;

  using pattern_t = std::vector<symbol_t>;
  using children_t = std::map<symbol_t, index_t>;
private:
  class BohrVrtx {
    children_t children;
    children_t cache_move;
    std::optional<index_t> suff_link;
    std::optional<index_t> suff_best_link;
    std::optional<index_t> parent;
    std::optional<symbol_t> sym;
    std::size_t pattern_number;
    bool is_pattern;
  public:
    explicit BohrVrtx();
    explicit BohrVrtx(index_t _parent, symbol_t _sym);

    friend Bohr;
    template<typename T>
    friend std::ostream& operator<< (std::ostream &os, const typename Bohr<T>::BohrVrtx &vrtx);
  };

  std::vector<BohrVrtx> vs;
  std::vector<pattern_t> ps;

  index_t get_suff_link(index_t vrtx_ind);
  index_t get_cache_move(index_t vrtx_ind, symbol_t sym);
  
  index_t get_suff_best_link(index_t vrtx_ind);
public:
  explicit Bohr();
  void add_pattern(const pattern_t &p);
  bool contains(const pattern_t &p) const;

  std::vector<std::pair<std::size_t/*patten_number*/, std::size_t/*position*/>> find(std::vector<symbol_t> text);

  template<typename T>
  friend std::ostream& operator<< (std::ostream &os, const Bohr<T> &b);
};

template<typename _symbol_t>
Bohr<_symbol_t>::BohrVrtx::BohrVrtx(): suff_link{std::nullopt},
  suff_best_link{std::nullopt}, parent{std::nullopt}, sym{std::nullopt}, is_pattern{false} {}

template<typename _symbol_t>
Bohr<_symbol_t>::BohrVrtx::BohrVrtx(index_t _parent, symbol_t _sym): suff_link{std::nullopt}, 
    suff_best_link{std::nullopt}, parent{_parent}, sym{_sym}, is_pattern{false} {}

template<typename _symbol_t>
Bohr<_symbol_t>::Bohr(): vs{BohrVrtx{}} {}

template<typename _symbol_t>
std::ostream& operator<< (std::ostream &os, const Bohr<_symbol_t> &b) {
  for (std::size_t i{0}; i < b.vs.size(); i++) {
    std::cout << i << ":\n" << b.vs[i];
  }
  return os;
}

template<typename _symbol_t>
std::ostream& operator<< (std::ostream &os, const typename Bohr<_symbol_t>::BohrVrtx &vrtx) {
  std::ostringstream ss;
  ss <<
      "\tsym: " << vrtx.sym.value_or(-1) << "\n" <<
      "\tparent: " << vrtx.parent.value_or(-1) << "\n" <<
      "\tsuff_link: " << vrtx.suff_link.value_or(-1) << "\n" <<
      "\tsuff_best_link: " << vrtx.suff_best_link.value_or(-1) << "\n" <<
      "\tchildren:\n";
    for (const auto &el : vrtx.children) {
      std::cout << "\t\t{ symbol: " << el.first << ", index: " << el.second << " }\n";
    }

    std::cout << "\tcache_move:\n";
    for (const auto &el : vrtx.cache_move) {
      std::cout << "\t\t{ symbol: " << el.first << ", index: " << el.second << " }\n";
    }
  return os << ss.str();
}

template<typename _symbol_t>
typename Bohr<_symbol_t>::index_t Bohr<_symbol_t>::get_suff_link(index_t vrtx_ind) {
  if (!vs[vrtx_ind].suff_link.has_value()) {
    if (vrtx_ind && vs[vrtx_ind].parent.has_value() && vs[vrtx_ind].parent.value()) {
      vs[vrtx_ind].suff_link = get_cache_move(get_suff_link(vs[vrtx_ind].parent.value()), vs[vrtx_ind].sym.value());
    } else {
      vs[vrtx_ind].suff_link = 0;
    }
  }
  return vs[vrtx_ind].suff_link.value();
}

template<typename _symbol_t>
typename Bohr<_symbol_t>::index_t Bohr<_symbol_t>::get_cache_move(index_t vrtx_ind, symbol_t sym) {
  if (typename children_t::const_iterator it{vs[vrtx_ind].cache_move.find(sym)}; it == vs[vrtx_ind].cache_move.cend()) {
    if (typename children_t::const_iterator ch = vs[vrtx_ind].children.find(sym); ch == vs[vrtx_ind].children.cend()) {
      if (vrtx_ind) {
        vs[vrtx_ind].cache_move[sym] = get_cache_move(get_suff_link(vrtx_ind), sym);
      } else {
        vs[vrtx_ind].cache_move[sym] = 0;
      }
    } else {
      vs[vrtx_ind].cache_move[sym] = ch->second;
    }
    return vs[vrtx_ind].cache_move[sym];
  } else {
    return it->second;
  }
}

template<typename _symbol_t>
typename Bohr<_symbol_t>::index_t Bohr<_symbol_t>::get_suff_best_link(index_t vrtx_ind) {
  if (!vs[vrtx_ind].suff_best_link) {
    index_t suff = get_suff_link(vrtx_ind);
    if (suff) {
      vs[vrtx_ind].suff_best_link = vs[suff].is_pattern ? suff : get_suff_best_link(suff);
    } else {
      vs[vrtx_ind].suff_best_link = 0;
    }
  }
  return vs[vrtx_ind].suff_best_link.value();
}

template<typename _symbol_t>
std::vector<std::pair<std::size_t, std::size_t>> Bohr<_symbol_t>::find(std::vector<symbol_t> text) {
  std::vector<std::pair<std::size_t, std::size_t>> v;
  index_t suff{0};
  for (std::size_t i{0}; i < text.size(); i++) {
    suff = get_cache_move(suff, text[i]);
    for (index_t _suff{suff}; _suff != 0; _suff = get_suff_best_link(_suff)) {
      if (vs[_suff].is_pattern) {
        v.emplace_back(vs[_suff].pattern_number, i - ps[vs[_suff].pattern_number].size() + 1);
      }
    }
  }
  return v;
}

template<typename _symbol_t>
void Bohr<_symbol_t>::add_pattern(const pattern_t &p) {
  index_t num = 0;
  for (const symbol_t s : p) {
    if (typename children_t::const_iterator it{vs[num].children.find(s)}; it == vs[num].children.cend()) {
      vs.emplace_back(num, s);
      vs[num].children[s] = vs.size() - 1;
      num = vs.size() - 1;
    } else {
      num = it->second;
    }
  }
  vs[num].is_pattern = true;
  ps.emplace_back(p);
  vs[num].pattern_number = ps.size() - 1;
}

template<typename _symbol_t>
bool Bohr<_symbol_t>::contains(const pattern_t &p) const {
  index_t num = 0;
  for (const symbol_t s : p) {
    if (typename children_t::const_iterator it{vs[num].children.find(s)}; it == vs[num].children.cend()) {
      return false;
    } else {
      num = it->second;
    }
  }
  return vs[num].is_pattern;
}

#endif
