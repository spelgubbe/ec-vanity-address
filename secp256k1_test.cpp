// use cxxtest

#include <cxxtest/TestSuite.h>
#include <sstream>
#include <string>
#include <cassert>
#include <algorithm>
#include <iostream>
#include "secp256k1.h"
#include "testconstants.h"
#include <random>
#include <vector>
using std::abs;
using std::size_t;


class MyTestSuite : public CxxTest::TestSuite
{
public:
  void testAssignmentOperator()
  {
    secp256k1_scalar bigNumber = ONE_BILLION;
    secp256k1_scalar smallerNumber = bigNumber;
    TS_ASSERT_EQUALS(bigNumber, smallerNumber);
  }

  // OPERATOR TESTS

  void testLessThan()
  {
    TS_ASSERT_LESS_THAN(ONE_HUNDRED, TEN_THOUSAND);
  }

  void testGreaterThan()
  {
    TS_ASSERT(TEN_THOUSAND > ONE_HUNDRED);
    TS_ASSERT(!(ZERO > ONE));
    TS_ASSERT(ONE > ZERO);
    TS_ASSERT(MAX > ONE_BILLION);
  }

  void testEquals()
  {
    TS_ASSERT_EQUALS(ONE_BILLION, ONE_BILLION);
    TS_ASSERT_EQUALS(ZERO, ZERO);
    TS_ASSERT_EQUALS(ONE, ONE);
    TS_ASSERT_EQUALS(MAX, MAX);
  }

  void testGEQ()
  {
    TS_ASSERT(ONE_BILLION >= ONE_BILLION);
    TS_ASSERT(ONE_TRILLION >= ONE_BILLION);
    TS_ASSERT(ONE >= ONE);
    TS_ASSERT(MAX >= MAX);
  }

  void testLEQ()
  {
    TS_ASSERT(ONE_BILLION <= ONE_BILLION);
    TS_ASSERT(ONE_HUNDRED <= ONE_BILLION);
    TS_ASSERT(ONE <= ONE);
    TS_ASSERT(MAX <= MAX);
  }

  void testAddition()
  {
    secp256k1_scalar bigNumber = ZERO;
    secp256k1_scalar biggerNumber = ONE_MILLION;
    for(int i = 0; i < 100; i++)
    {
      bigNumber += TEN_THOUSAND;
    }

    TS_ASSERT_EQUALS(bigNumber, biggerNumber);
  }

  void testAdditionToMaxWorks()
  {
    secp256k1_scalar smallNumber = ONE;
    secp256k1_scalar accumulator = ZERO;
    for(int i = 0; i < 256; i++) // 2^x += 2^x = 2^(x+1)
    {
      
      // smallerNumber = 2^i
      accumulator += smallNumber;

      if (i < 255) {
        smallNumber += smallNumber;
      }
      
      // smallerNumber = 2^(i+1)
      // expect accumulator = 2^255 + 2^254 + ... 2^1 + 2^0 == MAX
    }

    TS_ASSERT_EQUALS(accumulator, MAX);
  }

  void testAdditionTo256()
  {
    secp256k1_scalar smallNumber = SIXTEEN;
    secp256k1_scalar accumulator = ZERO;
    for(int i = 0; i < 16; i++) // 16*16 = 256
    {
      accumulator += smallNumber;
    }

    TS_ASSERT_EQUALS(accumulator, TWOFIVESIX);
  }

  void testSubtractionToZeroWorks()
  {
    secp256k1_scalar bigNumber = ONE_BILLION;
    secp256k1_scalar smallerNumber = bigNumber;
    smallerNumber -= bigNumber;

    TS_ASSERT_EQUALS(smallerNumber, ZERO);
  }

  void testSubtractionUnderflowThrows()
  {
    secp256k1_scalar bigNumber = ONE_BILLION;
    secp256k1_scalar biggerNumber = ONE_TRILLION; 
    TS_ASSERT_THROWS_ANYTHING(bigNumber -= biggerNumber);
  }

  void testShiftOfZeroIsInvertible()
  {
    secp256k1_scalar bigNumber = ONE_BILLION;
    secp256k1_scalar smallerNumber = bigNumber;
    TS_ASSERT_EQUALS(bigNumber, smallerNumber);

    smallerNumber <<= 0;
    smallerNumber >>= 0;

    TS_ASSERT_EQUALS(bigNumber, smallerNumber);
  }

  void testShiftOfOneIsSometimesInvertible()
  {
    secp256k1_scalar bigNumber = ONE_BILLION;
    secp256k1_scalar smallerNumber = bigNumber;
    TS_ASSERT_EQUALS(bigNumber, smallerNumber);

    smallerNumber <<= 1;
    smallerNumber >>= 1;

    TS_ASSERT_EQUALS(bigNumber, smallerNumber);
  }

  void testShiftOfBlocksizeIsSometimesInvertible()
  {
    secp256k1_scalar bigNumber = ONE_BILLION;
    secp256k1_scalar smallerNumber = bigNumber;
    smallerNumber <<= 32;
    smallerNumber >>= 32;

    TS_ASSERT_EQUALS(bigNumber, smallerNumber);
  }

  void testShiftIsSometimesInvertible()
  {
    secp256k1_scalar bigNumber = ONE_BILLION;
    secp256k1_scalar smallerNumber = bigNumber;
    smallerNumber <<= 33;
    smallerNumber >>= 33;

    TS_ASSERT_EQUALS(bigNumber, smallerNumber);
  }

  void testShiftMax()
  {
    secp256k1_scalar bigNumber = MAX;
    secp256k1_scalar smallerNumber = ONE;
    smallerNumber <<= 255;
    smallerNumber >>= 255;

    TS_ASSERT_EQUALS(ONE, smallerNumber);
  }

  void testShiftWholeNumberIsZero()
  {
    secp256k1_scalar bigNumber = MAX;
    secp256k1_scalar smallerNumber = MAX;
    smallerNumber <<= 256;
    TS_ASSERT_EQUALS(ZERO, smallerNumber);
    smallerNumber >>= 256;
    TS_ASSERT_EQUALS(ZERO, smallerNumber);
    
  }

  // END OPERATOR TESTS

  // Tests which depend on many operators working...

  void testLeadingZerosOnZero()
  {
    TS_ASSERT_EQUALS(lzcount(ZERO), sizeof(secp256k1_scalar)*8);
  }

  void testLeadingZerosOn512bitZero()
  {
    TS_ASSERT_EQUALS(lzcount(padto512(ZERO)), sizeof(secp256k1_mult_result)*8);
  }

  void testLeadingZerosOnMaxScalarIsZero()
  {
    TS_ASSERT_EQUALS(lzcount(MAX), 0);
  }

  void testLeadingZerosOnMaxScalarPaddedIs256()
  {
    TS_ASSERT_EQUALS(lzcount(padto512(MAX)), 256);
  }

  void testModGeneral()
  {
    secp256k1_mult_result x = MAXpow2;
    secp256k1_mult_result m = padto512(MAX);
    secp256k1_mult_result x_mod_m = mod(x, m);

    //TS_ASSERT_EQUALS(ZERO, x_mod_m);
  }

  void testShrinkLeavesLowerBitsUnchanged()
  {
    secp256k1_mult_result max = mult(ONE, MAX);
    TS_ASSERT_EQUALS(MAX, shrinkto256(max));
  }

  void testPadLeavesLowerBitsUnchanged()
  {
    secp256k1_mult_result max = mult(ONE, MAX);
    TS_ASSERT_EQUALS(padto512(MAX), max);
  }

  void testMult1()
  {
    secp256k1_mult_result million = mult(ONE_HUNDRED, TEN_THOUSAND);
    TS_ASSERT_EQUALS(million, ONE_MILLION);
  }

  void testMultZeroIsZero()
  {
    secp256k1_mult_result res = mult(ZERO, ZERO);
    TS_ASSERT_EQUALS(ZERO, res);
  }

  void testMultAnythingWithZeroIsZero()
  {
    secp256k1_mult_result res = mult(MAX, ZERO);
    TS_ASSERT_EQUALS(ZERO, res);
  }

  void testMult16by16Is256()
  {
    secp256k1_mult_result res = mult(SIXTEEN, SIXTEEN);
    TS_ASSERT_EQUALS(TWOFIVESIX, res);
  }

  void testMultScalarMaxIsCorrect()
  {
    secp256k1_mult_result multmax = mult(MAX, MAX);
    TS_ASSERT_EQUALS(multmax, MAXpow2);
  }

  void testReduceWorksOnSmallerScalar()
  {
    secp256k1_mult_result max100 = mult(ONE_HUNDRED, ONE_HUNDRED);

    secp256k1_scalar reduced = reduce(max100);

    TS_ASSERT_EQUALS(TEN_THOUSAND, reduced);
  }

  void testReduceOnPIsZero()
  {
    secp256k1_mult_result p = mult(SECP256K1_P, ONE);
    TS_ASSERT_EQUALS(p, SECP256K1_P);

    secp256k1_scalar reduced = reduce(p);

    TS_ASSERT_LESS_THAN(reduced, MAX);
    TS_ASSERT_EQUALS(reduced, ZERO);
  }

};
