// -*- coding: utf-8; mode: c++ -*-

#ifndef EXPLSEED_PARK_MILLER_MCG_H
#define EXPLSEED_PARK_MILLER_MCG_H

#include <array>

#include "int_mod.h"

template<class T> class rng_traits ;

class park_miller_mcg
{
  friend class rng_traits<park_miller_mcg>;

public:
  void init(int32_t s0)
  {
    s = s0;
  }

  void step_fwd()
  {
    auto next_s = (static_cast<uint64_t>(s) * 16807) % 2147483647;

    s = static_cast<uint32_t>(next_s);
  }
  
private:  
  uint32_t s = 0;
};

template<> class rng_traits<park_miller_mcg>
{
public:
  enum { N = 1 } ;

  using state_t = std::array<int_mod<2147483647>, N>;

  static state_t zero_state()
  {
    return {};
  }
  
  static state_t get_state(const park_miller_mcg &rng)
  {
    state_t st;

    st[0] = rng.s;

    return st;
  }

  static set_state(park_miller_mcg &rng, const state_t &st)
  {
    rng.s = st[0];
  }
};

#endif
