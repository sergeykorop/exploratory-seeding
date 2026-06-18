// -*- coding: utf-8; mode: c++ -*-

#ifndef EXPLSEED_TR_UTILS_H
#define EXPLSEED_TR_UTILS_H

#include <array>
#include <iostream>

template<class T> class rng_traits ;

template<class R>
class tr_utils
{
  using traits = rng_traits<R>;
  
  enum { N = traits::N };
  
public:
  using state_t = typename traits::state_t ;
  using T_t = std::array<std::array<typename state_t::value_type, N>, N> ;
  
  static T_t make_T()
  {
    T_t T;

    R rng;

    for(size_t i = 0; i < N; i++)
    {
      state_t st = traits::zero_state();

      st[i] = 1;

      traits::set_state(rng, st);

      rng.step_fwd();

      st = traits::get_state(rng);

      for(size_t j = 0; j < N; j++)
        T[j][i] = st[j];
    }

    return T;
  }

  static state_t matmul(const T_t &T, const state_t &s)
  {
    state_t res;

    for(size_t i = 0; i < N; i++)
    {
      res[i] = 0;
      
      for(size_t j = 0; j < N; j++)
      {
	res[i] += T[i][j] * s[j];
      }
    }
  
    return res;
  }

  static void print_T(std::ostream &out, const T_t &T)
  {
    for(size_t i = 0; i < N; i++)
    {
      for(size_t j = 0; j < N; j++)
        out << (j == 0 ? "" : " ") << T[i][j];

      out << std::endl;
    }
  }  
};

#endif
