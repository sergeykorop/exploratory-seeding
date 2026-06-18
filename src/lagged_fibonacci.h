// -*- coding: utf-8; mode: c++ -*-

#ifndef EXPLSEED_LAGGED_FIBONACCI_H
#define EXPLSEED_LAGGED_FIBONACCI_H

#include <array>

template<class T> class rng_traits ;

class lagged_fibonacci
{
  friend class rng_traits<lagged_fibonacci>;

public:
  lagged_fibonacci() { Y.fill(0); }
  
  void init(std::array<uint32_t, 55> st)
  {
    for(size_t i = 0; i < st.size(); i++)
      Y[i+1] = st[i];
    
    k = 55;
    j = 24;
  }
  
  void step_fwd()
  {
    // Algorithm A / TAOCP, Vol. 2 (3rd ed.), pg. 27:

    Y[k--] += Y[j--];

    if(j == 0)
      j = 55;
    else if(k == 0)
      k = 55;
  }
  
private:
  std::array<uint32_t, 56> Y;

  size_t k = 55;
  size_t j = 24;
};



template<> class rng_traits<lagged_fibonacci>
{
public:
  enum { N = 55 } ;

  using state_t = std::array<uint32_t, N>;

  static state_t zero_state()
  {
    state_t st;

    st.fill(0);
    
    return st;
  }
  
  static state_t get_state(const lagged_fibonacci &rng)
  {
    state_t st;

    int k = rng.k;
    
    for(int i = st.size() - 1; i >= 0; i--)
    {
      st[i] = rng.Y[k];
      
      k = (k != 1 ? k - 1 : 55);
    }

    return st;
  }

  static set_state(lagged_fibonacci &rng, const state_t &st)
  {
    for(size_t i = 0; i < st.size(); i++)
      rng.Y[i+1] = st[i];
    
    rng.k = 55;
    rng.j = 24;
  }
};

#endif
