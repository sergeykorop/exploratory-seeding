#include "lagged_fibonacci.h"
#include "park_miller_mcg.h"
#include "tr_utils.h"
#include "xorshift128.h"

#include <gtest/gtest.h>

namespace
{
  const size_t num_steps = 1000;
}

TEST(ExplSeed, ParkMillerMCG)
{
  using traits = rng_traits<park_miller_mcg>;
  using utils = tr_utils<park_miller_mcg>;

  park_miller_mcg rng;

  rng.init(0x20230904);
  
  auto T = utils::make_T();
  auto s = traits::get_state(rng);

  for(int i = 0 ; i < num_steps; i++)
  {
    rng.step_fwd();

    s = utils::matmul(T, s);

    ASSERT_EQ(s, traits::get_state(rng))
      << "Failed at step " << i ;
  }
}

TEST(ExplSeed, XorShift128)
{
  using traits = rng_traits<xorshift128>;
  using utils = tr_utils<xorshift128>;
  
  xorshift128 rng;

  rng.init({
    123456789,
    362436069,
    521288629,
    88675123
  });

  auto T = utils::make_T();
  auto s = traits::get_state(rng);

  for(int i = 0 ; i < num_steps; i++)
  {
    rng.step_fwd();

    s = utils::matmul(T, s);

    ASSERT_EQ(s, traits::get_state(rng))
       << "Failed at step " << i;
  }
}

TEST(ExplSeed, LaggedFibonacci)
{
  using traits = rng_traits<lagged_fibonacci>;
  using utils = tr_utils<lagged_fibonacci>;
  
  lagged_fibonacci rng;

  rng.init({
    0xbe1b4d32,
    0xdf14f891,
    0xd8f80469,
    0x58c104c6,
    0x76626c89,
    0x5e36d06d,
    0x72a7b974,
    0xfab6be13,
    0x7dbaedb2,
    0x0704f05a,
    0xa8881094,
    0xea590ab1,
    0x62a72202,
    0x91972bb8,
    0xc04d9272,
    0x9b692d18,
    0x3dfa6648,
    0xf5e26a6c,
    0x75a10e6f,
    0xf8060a85,
    0x0f8e248b,
    0xa99d2a33,
    0xfe26e07b,
    0x35933046,
    0x560dfa1b,
    0xcbf8be2c,
    0xcf3bebfd,
    0xc782ce45,
    0x7bf244af,
    0xf76874d5,
    0x0da9fcea,
    0xb308ce1e,
    0x78a63331,
    0xf70e5c16,
    0x1893fe34,
    0xab551be5,
    0x948c0f99,
    0x26e9d1fe,
    0xcaaab6d0,
    0x60e6b1ff,
    0xf3649360,
    0x69605a40,
    0x61ab7c7f,
    0x6c3412a3,
    0x1a0b431e,
    0x5738a4e2,
    0x2bd0a4c3,
    0x2ce05ee2,
    0x72d35375,
    0xeced66b9,
    0xe7716b54,
    0x50916150,
    0x31e39b0d,
    0x85562930,
    0xfd902096,
  });

  auto T = utils::make_T();
  auto s = traits::get_state(rng);

  for(int i = 0 ; i < num_steps; i++)
  {
    rng.step_fwd();

    s = utils::matmul(T, s);

    ASSERT_EQ(s, traits::get_state(rng))
      << "Failed at step " << i;
  }  
}
