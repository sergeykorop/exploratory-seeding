GTEST_INCLUDE=/opt/gtest-1.8.1/include
GTEST_LIB=/opt/gtest-1.8.1/lib

CXXFLAGS=-std=c++11 -g -O3

all: explseed

clean:
	-rm *.o *.exe

check: explseed
	./explseed

explseed: explseed.o
	g++ $(CXXFLAGS) -o explseed explseed.o -L$(GTEST_LIB) -lgtest -lgtest_main

explseed.o: explseed.cc int_mod.h lagged_fibonacci.h park_miller_mcg.h tr_utils.h xorshift128.h
	g++ $(CXXFLAGS:c++11=gnu++11) -c explseed.cc -I$(GTEST_INCLUDE)
