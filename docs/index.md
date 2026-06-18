---
title: "Reverse-engineering Linear PRNG with Exploratory Seeding"
description: "How to calculate transition matrix for linear pseudo-random number generator manipulating its internal state."
image: "/thumbnail.png"
---

# {{page.title}}

{{page.description}}

Published: 2026-06-19.

Originally published at [CodeProject](https://web.archive.org/web/20250613081450/https://www.codeproject.com/Articles/5368078/Reverse-engineering-Linear-PRNG-with-Exploratory-S), 2023-09-10.

\[[Source Code](https://github.com/sergeykorop/exploratory-seeding/)\]

## Introduction

Linear pseudo-random number generators (PRNGs) are quite popular in
applications where security is not an issue: they are fast and, often
more importantly, allow doing some tricks beyond simply producing a
stream of random numbers, such as jumping along this stream in both
directions with logarithmic complexity. This can be particularly
useful when we need to distribute and/or parallelize our computations.

Algorithms performing such jumps are based on the common theory which
is explained, for example, in my previous articles:
[`System.Random` and Infinite Monkey Theorem](/monkey-typewriter/)
or
[XorShift Jump 101](/xorshift-jump/).
However, we must do some preparatory steps before applying this common
theory, and those steps are non-trivial and depend on particular PRNG
type.

Naturally, this raises the question: can we do better? Fortunately,
the answer is positive: we indeed can generalize further and make the
entire process less PRNG-dependent.

## Background

It is assumed that the reader is familiar with linear PRNG theory basics explained in
[`System.Random` and Infinite Monkey Theorem](/monkey-typewriter/).
Unlike that article, the demo code for this one is written in C++ 11
so the reader should be fluent in this dialect at the intermediate
level.

The code has been intentionally written as library-independent as possible, except for
[Google Test 1.8.1](https://github.com/google/googletest)
which is used for unit testing infrastructure.

Building the demo requires G++ with accompanying toolchain, such as
Cygwin (which has been actually used) but any modern Linux environment
should also fit well.

## Exploratory Seeding: The Idea

Let's begin from the short recap of the theory. As we could see from
previous articles, _transition function_ \[[2](#ref.2)\] of
linear PRNG can be represented as:

$$
s^{(i+1)} = T s^{(i)},
$$

where $T$ is some matrix which could be named as _transition matrix_
and both $s^{(i+1)}$ and $s^{(i)}$ are $N$-dimensional vectors
belonging to the set $S$ of _states_.

This transition matrix differs from one PRNG algorithm to another and
we must figure it out.  While for
[lagged Fibonacci generator](https://en.wikipedia.org/wiki/Lagged_Fibonacci_generator),
this matrix is quite obvious and could be simply
[written down](/monkey-typewriter/),
doing the same for
[xorshift](https://en.wikipedia.org/wiki/Xorshift)
required [writing a program](/xorshift-jump/matrix-multiplication/)
to perform symbolic transformations over formulas specifying some
particular PRNG from the xorshift family.

Let's consider now a special value of
$s=\left\langle 1, 0, \ldots, 0\right\rangle$.
In this case

$$
\begin{multline*}
T s = \begin{pmatrix}
  t_{11} & t_{12} & \ldots & t_{1N} \\
  t_{21} & t_{22} & \ldots & t_{2N} \\
  \vdots & \vdots & \ddots & \vdots \\
  t_{N1} & t_{N2} & \ldots & t_{NN}
\end{pmatrix}
\begin{pmatrix}
  1 \\ 0 \\
  \vdots \\
  0
\end{pmatrix} = \\
= \begin{pmatrix}
    t_{11} \cdot 1 + t_{12} \cdot 0 + \cdots + t_{1N} \cdot 0 \\
    t_{21} \cdot 1 + t_{22} \cdot 0 + \cdots + t_{2N} \cdot 0 \\
    \vdots \\
    t_{N1} \cdot 1 + t_{N2} \cdot 0 + \cdots + t_{NN} \cdot 0 \\
\end{pmatrix}
= \begin{pmatrix}
    t_{11} \\
    t_{21} \\
    \vdots \\
    t_{N1}
\end{pmatrix}.
\end{multline*}
$$

Doing the same for $s=\left\langle 0, 1, 0, \ldots, 0\right\rangle$,
we can see the pattern:

$$
\begin{multline*}
T s = \begin{pmatrix}
  t_{11} & t_{12} & \ldots & t_{1N} \\
  t_{21} & t_{22} & \ldots & t_{2N} \\
  \vdots & \vdots & \ddots & \vdots \\
  t_{N1} & t_{N2} & \ldots & t_{NN}
\end{pmatrix}
\begin{pmatrix}
  0 \\
  1 \\
  0 \\
  \vdots \\
  0
\end{pmatrix} = \\
 = \begin{pmatrix}
     t_{11} \cdot 0 + t_{12} \cdot 1 + t_{13} \cdot 0 + \cdots + t_{1N} \cdot 0 \\
     t_{21} \cdot 0 + t_{22} \cdot 1 + t_{23} \cdot 0 + \cdots + t_{2N} \cdot 0 \\
     \vdots \\
     t_{N1} \cdot 0 + t_{N2} \cdot 1 + t_{23} \cdot 0 + \cdots + t_{NN} \cdot 0 \\
   \end{pmatrix}
= \begin{pmatrix}
    t_{12} \\
    t_{22} \\
    \vdots \\
    t_{N2}
\end{pmatrix}.
\end{multline*}
$$

That is, if we set the PRNG state to
$\left\langle 1, 0, \ldots, 0\right\rangle$
and apply the transition function to this state, the subsequent state
will be equal to the first column of the corresponding transition
matrix. Repeating this procedure $N$ times for unit value running
across the state vector, we can collect the entire matrix
$T$. Effectively, we calculate each column of the matrix product
$T I = T$, obtaining value of $T$ as a result.

This technique is so obvious that I would never claim I invented it
first. It must be definitely used somewhere else in the past, but,
unfortunately, I couldn't find any academic parer to reference.

Coming back to techspeak, initializing the PRNG with some state is
known as &ldquo;seeding&rdquo; and, since we have done it with a goal
to obtain some knowledge about the PRNG internals, it could be
considered an exploration as well. This explains the reason behind
calling the entire process an _exploratory seeding_.

Comparing this approach to our earlier ways of getting the transition
matrix, we can see that it needs much less information about the PRNG
algorithm: indeed, this PRNG still has to be linear, have an ability
to set its internal state vector and get it back but it is no longer
necessary to know the precise formula for transition function. In
fact, we can even _reverse engineer_ some PRNG presented to us as a
kind of &ldquo;black box&rdquo;. However, in real life, the need to
perform such reverse engineereing is extremely rare: you can always
find some trustworthy PRNG algorithm which is properly
described. Therefore, the main value of exploratory seeding is its
ability to get transition matrix with less efforts rather than unravel
the mysteries.

## Software Implementation

The source code accompanying this article is aimed to provide a short
proof-of-concept demo rather than some elaborate library. Therefore, I
will trade an efficiency for simplicity and clarity when appropriate.

The entire code consists of two parts: for beginning, we will
implement some popular PRNG algorithms to show how the exploratory
seeding approach could be generalized. The second part, the core of
the method, will be a generic implementation of the seeding procedure,
applicable to all of them.

So far, our sample PRNGs are: one of so called multiplicative
congruential PRNGs (MCGs), namely &ldquo;Minimal Standard&rdquo;
MCG proposed by Park and Miller and described in \[[4](#ref.4), § 7.1\],
one representative of the lagged Fibonacci generator family from \[[1](#ref.1), § 3.2.2\]
and, finally, 128-bit xorshift \[[3](#ref.3)\],
our workhorse from my [previous](/xorshift-jump/) article.

A word of caution: these generators are only chosen for this article
due to their simplicity and variety of approaches. _They do not
represent a state-of-art in pseudorandom number generation, have known
issues and, therefore, not recommended to use in production._

The project structure is quite simple: each PRNG type is defined in
separate header file with appropriate name (_park_miller_mcg.h_, and
so on). Header file _tr_utils.h_ contains
an implementation of the exploratory seeding itself. Due to extensive
use of C++ templates, the entire class definitions are present in
these header files. Finally, the only _.cc_ file in this project,
_explseed.cc_, contains unit tests checking
transition matrix correctness for each PRNG family mentioned above.

The PRNG implementations are, to some extent, incomplete. Indeed,
since we are more interested in manipulations with the generator
state, we don't have to implement the _output function_
\[[2](#ref.2)\]. Therefore, the public interface of PRNG classes
is limited to only two functions: `init()`, which initializes the PRNG
state from PRNG-specific data parameter, and `step_fwd()` which
advances this generator internal state by one forward step.

This public PRNG interface, however, is not enough for exploratory
seeding. Indeed, we need a way of reading the PRNG internal state and
setting it back. Even more, this way should be unified so we will be
able to develop generic code. It is natural to define two
complementary functions: `get_state()` and `set_state()`,
respectively. But how should we do that satisfying our requirement?
One possible solution would be adding such functions directly to class
representing each PRNG but this functionality is quite specific and
not aimed for ordinary use. Another approach which allows us to
separate code by its intended audience is known as
[_type traits_](https://www.boost.org/community/generic_programming.html#traits).

The basic idea is very simple: let's move auxiliary functionality to
separate companion class. Note, that it is important to implement an
ability to associate these classes to each other so they can be used
together in templates where PRNG type itself is passed as template
parameter. Standard solution for this problem is _template
specialization_. First, we declare template class representing a
family of specific type traits:

```cpp
template <class R> class rng_traits;
```

And then, for each PRNG class, we specialize this template:

```cpp
class xorshift128
{
  ...
};

template<>
class rng_traits<xorshift128>
{
  ...
};
```

The resulting set of specializations can be later used either with
fixed PRNG types:

```cpp
void do_something()
{
  using traits = rng_traits<xorshift128>;

  xorshift128 rng;

  auto s = traits::get_state(rng);

  ...
}
```

or even with some type which is currently unknown:

```cpp
template <class R>
void do_something()
{
  using traits = rng_traits<R>;

  R rng;

  auto s = traits::get_state(rng);

  ...
}
```

Note that using type traits, we can extend the functionality of some
class multiple times in different ways and do it _non-intrusively_ as
long as we can manipulate the objects of this class via its public
interface. In our case, however, I decided to allow traits to access
the internals of PRNG classes making their public interface
smaller. Also, this approach is very similar to the notion of _type
classes_ available in Haskell.

Demo code implemented for this article simply puts both the PRNG class
and its traits to the same header file but in real life, it would
possibly be better to use separate files and namespace conventions to
isolate these internals from client code.

The code implementing exploratory seeding makes following assumptions regarding PRNG traits:

* PRNG trait defines some vector-like container type `state_t`
  representing the PRNG state. The size of this container is fixed and
  available at the compile time via anonymous enumeration with single
  item named `N`.

* Type of state vector elements could be arbitrary but with behavior
  resembling numbers (namely, integer constants `0` and `1` are
  implicitly converted to corresponding elements of that type,
  operations of multiplication and in-place addition work in a natural
  way).

  Our demo uses state vectors of two types: standard `uint32_t` and
  user-defined `int_mod<M>`, implementing the required subset of
  operations over finite ring of integers modulo $M$.

* State vectors can be read from the PRNG instance with `get_state()`
  and written there with `set_state()`.

* No assumption is made regarding the initial value of the `state_t`
  instance. Therefore, and additional function `zero_state()` is
  needed to create zero state vector used in exploratory seeding
  procedures.


After preparatory work is done, we are ready to implement the goal of
our journey: the generic function `makeT()` calculating transition
matrix of arbitrary linear PRNG. Note that our implementation is
completely abstract and could be used for any size of the PRNG state
and even for any type of state items. This level of flexibility could
be achieved by moving all PRNG-specific implementation details to
corresponding trait classes.

```cpp
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
  ...
};
```

Note that `makeT()` has been made a `static` member of the `tr_utils`
template class. This class also implements some other functions used
together: `matmul()` which simply multiplies the transition matrix by
state vector effectively advancing the PRNG one step forward, and
`print_T()` which outputs transition matrix and can be useful for
debugging. Putting all those functions to the same outer class allows
us to introduce common type aliases making code more clear.

Finally, let's consider _explseed.cc_, the only _.cc_ file in the demo
code. It contains three unit tests which could be used both to check
that everything works properly and, additionally, to provide an
example of using classes and functions described above.

```cpp
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
```

All tests have exactly the same structure except for the used PRNG
type. Could't we probably add one more template function to reduce
source code size even further? I wouldn't do that because in this
case, all line numbers shown in error messages will be the same for
all tests and refer to that helper function rather than to some
particular failed test.

The test procedure itself is quite trivial: we create an instance of
some PRNG type, copy its state and then perform `num_steps` updates of
this state using both transition matrix calculated for that PRNG type
and its member function `step_fwd()`. After each step, the internal
state of the generator is extracted and compared with calculated state
vector reporting an error if they are different.

## Conclusion

In this article, we have discussed one more way to create a transition
matrix for some linear PRNG. Unlike other approaches considered in my
previous articles, this one is simpler and could be, to some extent,
used in a ``black box'' manner, i.e., without full knowledge of the
PRNG internals. Some information, however, still be needed, such as
dimensionality of the state vector and the nature of its components
(are they real or integer, which modulo is used for integer
arithmetics, and so on).

The implementation part of this article could also be used as simple
use case of C++ programming technique known as type traits which is
more suitable for academic purposes rather than referring to the
source code of production grade libraries, such as Boost.

## References

<a id="ref.1">[1]</a> Donald E. Knuth.
_The Art of Computer Programming, Volume 2 (3rd Ed.): Seminumerical Algorithms._
Addison-Wesley Longman Publishing Co., Inc., Boston, MA, USA, 1997.

<a id="ref.2">[2]</a> P. L’Ecuyer. Uniform random number generation.
_Annals of Operations Research_, 53:77–120, 1994.

<a id="ref.3">[3]</a> George Marsaglia. Xorshift RNGs.
_Journal of Statistical Software, Articles,_ 8(14):1–6, 2003.
(Available [online](http://www.jstatsoft.org/v08/i14/paper).)

<a id="ref.4">[4]</a> William H. Press, Saul A. Teukolsky, William T. Vetterling, and Brian P. Flannery.
_Numerical Recipes in C (2nd Ed.): The Art of Scientific Computing._
Cambridge University Press, USA, 1992.

