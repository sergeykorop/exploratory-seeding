// -*- coding: utf-8; mode: c++ -*-

#ifndef EXPLSEED_INT_MOD_H
#define EXPLSEED_INT_MOD_H

#include <cstdint>

template<size_t M>
class int_mod
{
public:
  int_mod(uint32_t v = 0): val(v) {}

  operator uint32_t() const { return val; }

  int_mod operator *(const int_mod &rhs) const
  {
    uint64_t prod = static_cast<uint64_t>(val) * static_cast<uint64_t>(rhs.val);

    return {static_cast<uint32_t>(prod % M)};
  }

  int_mod &operator += (const int_mod &rhs)
  {
    uint64_t sum = static_cast<uint64_t>(val) + static_cast<uint64_t>(rhs.val);

    val = static_cast<uint32_t>(sum % M);
    
    return *this;
  }
  
private:
  uint32_t val;
};

#endif
