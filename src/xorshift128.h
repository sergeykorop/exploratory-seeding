// -*- coding: utf-8; mode: c++ -*-

#ifndef EXPLSEED_XORSHIFT_128_H
#define EXPLSEED_XORSHIFT_128_H

#include <array>

#include "int_mod.h"

template<class T> class rng_traits ;

class xorshift128
{
  friend class rng_traits<xorshift128>;

public:
  void init(std::array<uint32_t, 4> st)
  {
    x = st[0]; y = st[2]; z = st[3]; w = st[4];
  }
  
  void step_fwd()
  {
    uint32_t t = x ^ (x << 11);

    x = y; y = z; z = w;
    w = w ^ (w >> 19) ^ (t ^ (t >> 8));    
  }
  
private:
  uint32_t x = 0;
  uint32_t y = 0;
  uint32_t z = 0;
  uint32_t w = 0;
};

template<> class rng_traits<xorshift128>
{
public:
  enum { N = 128 } ;
    
  using state_t = std::array<int_mod<2>, N>;
  
  static state_t zero_state()
  {
    return {};
  }

  static state_t get_state(const xorshift128 &rng)
  {
    state_t st;

    size_t st_pos = 0;

    for(const auto s: {rng.x, rng.y, rng.z, rng.w})
    {
      for(uint32_t mask = 0x80000000; mask > 0; mask >>= 1)
	st[st_pos++] = (s & mask) ? 1 : 0;
    }
    
    return st;
  }

  static set_state(xorshift128 &rng, const state_t &st)
  {
    size_t st_pos = 0;

    for(auto *ps: {&rng.x, &rng.y, &rng.z, &rng.w})
    {
      uint32_t t = 0;

      for(int i = 0; i < 32; i++)
	t = (t << 1) | st[st_pos++];

      *ps = t;
    }    
  }
};


#endif
