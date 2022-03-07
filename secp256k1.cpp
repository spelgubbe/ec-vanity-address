#include "secp256k1.h"
#include "blockmath.h"
#include <cassert>
#include <stdexcept>
#include <iostream>
using std::cout;
using std::endl;

// API functions

bool operator<(const secp256k1_scalar &a, const secp256k1_scalar &b)
{
    return blockwise_cmp(a.d, b.d, sizeof(secp256k1_scalar)/sizeof(uint32_t)) == -1;
}

bool operator>(const secp256k1_scalar &a, const secp256k1_scalar &b)
{
    return blockwise_cmp(a.d, b.d, sizeof(secp256k1_scalar)/sizeof(uint32_t)) == 1;
}

bool operator==(const secp256k1_scalar &a, const secp256k1_scalar &b)
{
    return blockwise_cmp(a.d, b.d, sizeof(secp256k1_scalar)/sizeof(uint32_t)) == 0;
}

bool operator==(const secp256k1_scalar &a, const secp256k1_mult_result &b)
{
    for(int i = 0; i < 8; i++)
    {
        if (b.d[i] > 0) return false;
    }
    return blockwise_cmp(a.d, b.d + 8, sizeof(secp256k1_scalar)/sizeof(uint32_t)) == 0;
}

bool operator==(const secp256k1_mult_result &a, const secp256k1_scalar &b)
{
    return b == a;
}

bool operator<(const secp256k1_mult_result &a, const secp256k1_mult_result &b)
{
    return blockwise_cmp(a.d, b.d, sizeof(secp256k1_mult_result)/sizeof(uint32_t)) == -1;
}

bool operator<=(const secp256k1_scalar &a, const secp256k1_scalar &b)
{
    return blockwise_cmp(a.d, b.d, sizeof(secp256k1_scalar)/sizeof(uint32_t)) < 1;
}

bool operator>=(const secp256k1_scalar &a, const secp256k1_scalar &b)
{
    return blockwise_cmp(a.d, b.d, sizeof(secp256k1_scalar)/sizeof(uint32_t)) > -1;
}

bool operator<=(const secp256k1_mult_result &a, const secp256k1_mult_result &b)
{
    return blockwise_cmp(a.d, b.d, sizeof(secp256k1_mult_result)/sizeof(uint32_t)) < 1;
}

bool operator>=(const secp256k1_mult_result &a, const secp256k1_mult_result &b)
{
    return blockwise_cmp(a.d, b.d, sizeof(secp256k1_mult_result)/sizeof(uint32_t)) > -1;
}

bool operator>(const secp256k1_mult_result &a, const secp256k1_mult_result &b)
{
    return blockwise_cmp(a.d, b.d, sizeof(secp256k1_mult_result)/sizeof(uint32_t)) == 1;
}

bool operator==(const secp256k1_mult_result &a, const secp256k1_mult_result &b)
{
    return blockwise_cmp(a.d, b.d, sizeof(secp256k1_mult_result)/sizeof(uint32_t)) == 0;
}

void operator-=(secp256k1_scalar &a, const secp256k1_scalar &b)
{
    blockwise_sub(a.d, b.d, sizeof(secp256k1_scalar)/sizeof(uint32_t));
}

void operator-=(secp256k1_mult_result &a, const secp256k1_mult_result &b)
{
    blockwise_sub(a.d, b.d, sizeof(secp256k1_mult_result)/sizeof(uint32_t));
}

void operator+=(secp256k1_scalar &a, const secp256k1_scalar &b)
{
    blockwise_add(a.d, b.d, sizeof(secp256k1_scalar)/sizeof(uint32_t));
}

void operator+=(secp256k1_mult_result &a, const secp256k1_mult_result &b)
{
    blockwise_add(a.d, b.d, sizeof(secp256k1_mult_result)/sizeof(uint32_t));
}

void operator<<=(secp256k1_scalar &a, uint32_t k)
{
    blockwise_shl(a.d, k, sizeof(secp256k1_scalar)/sizeof(uint32_t));
}

void operator<<=(secp256k1_mult_result &a, uint32_t k)
{
    blockwise_shl(a.d, k, sizeof(secp256k1_mult_result)/sizeof(uint32_t));
}

void operator>>=(secp256k1_scalar &a, uint32_t k)
{
    blockwise_shr(a.d, k, sizeof(secp256k1_scalar)/sizeof(uint32_t));
}

void operator>>=(secp256k1_mult_result &a, uint32_t k)
{
    blockwise_shr(a.d, k, sizeof(secp256k1_mult_result)/sizeof(uint32_t));
}

int lzcount(const secp256k1_scalar &a)
{
    return blockwise_lzcount(a.d, sizeof(secp256k1_scalar)/sizeof(uint32_t));
}

int lzcount(const secp256k1_mult_result &a)
{
    return blockwise_lzcount(a.d, sizeof(secp256k1_mult_result)/sizeof(uint32_t));
}


secp256k1_mult_result mult(const secp256k1_scalar &a, const secp256k1_scalar &b)
{
    secp256k1_mult_result res = secp256k1_mult_result();

    blockwise_mult(res.d, a.d, b.d, sizeof(secp256k1_scalar)/sizeof(uint32_t));

    return res;
}

secp256k1_mult_result padto512(const secp256k1_scalar &a)
{
    secp256k1_mult_result res = secp256k1_mult_result();
    for(int i = 0; i < 8; i++)
    {
        res.d[i] = 0;
    }
    for(int i = 0; i < 8; i++)
    {
        res.d[8+i] = a.d[i];
    }
    return res;
}

secp256k1_scalar shrinkto256(const secp256k1_mult_result &a)
{
    secp256k1_scalar res = secp256k1_scalar();
    for(int i = 0; i < 8; i++)
    {
        res.d[i] = a.d[8+i];
    }
    return res;
}

secp256k1_mult_result mod(const secp256k1_mult_result &a, const secp256k1_mult_result &m)
{
    secp256k1_mult_result mod = m;
    secp256k1_mult_result last_mod = mod;
    secp256k1_mult_result tmp = a;

    //bool mod_is_smaller = mod_lz > a_lz || mod < a;

    while (true) {
        int a_lz = lzcount(tmp);
        int mod_lz = lzcount(last_mod);

        int diff = mod_lz - a_lz;

        if (tmp < mod) {
            break;
        }

        // if last_mod is smaller than tmp
        if (diff > 0) {

            // make last_mod bigger, but never bigger than tmp (as mod result would be negative)
            last_mod <<= (diff - 1);

            tmp -= last_mod;

        
        // if last_mod is larger than tmp
        } else if (diff < 0) {
            // make last_mod smaller, and never too small
            // potential risk here is that if -diff = 512, then this is a shift of 513 bits (which is illegal)
            // could only happen if mod is 0
            last_mod >>= (-diff + 1);

            tmp -= last_mod;

        // if diff == 0, and tmp >= mod
        } else {
            
            if (last_mod <= tmp) {
                tmp -= last_mod;
                
            } else {
                // last_mod > tmp and diff == 0
                // make last_mod smaller than tmp, then subtract
                last_mod >>= 1;
                tmp -= last_mod;
            }
        }
    }
    return tmp;
}

// This is not a fast solution
secp256k1_scalar reduce(const secp256k1_mult_result &a)
{
    secp256k1_mult_result mod = padto512(SECP256K1_P);
    secp256k1_mult_result last_mod = mod;
    secp256k1_mult_result tmp = a;

    //bool mod_is_smaller = mod_lz > a_lz || mod < a;

    while (true) {
        int a_lz = lzcount(tmp);
        int mod_lz = lzcount(last_mod);

        int diff = mod_lz - a_lz;

        if (tmp < mod) {
            break;
        }

        // if last_mod is smaller than tmp
        if (diff > 0) {

            // make last_mod bigger, but never bigger than tmp (as mod result would be negative)
            last_mod <<= (diff - 1);

            tmp -= last_mod;

        
        // if last_mod is larger than tmp
        } else if (diff < 0) {
            // make last_mod smaller, and never too small
            last_mod >>= (-diff + 1);

            tmp -= last_mod;

        // if diff == 0, and tmp >= mod
        } else {
            
            if (last_mod <= tmp) {
                tmp -= last_mod;
                
            } else {
                // last_mod > tmp and diff == 0
                // make last_mod smaller than tmp, then subtract
                last_mod >>= 1;
                tmp -= last_mod;
            }
        }
    }
    return shrinkto256(tmp);
}

// TODO
secp256k1_scalar fastreduce(const secp256k1_mult_result &a)
{
    /*
        Use math: a * 2^n + b === ac + b (mod 2^n - c)
        Probably: divide number into two parts, at least one has to be 512 bit
    */

    // Idea: split number into two halves...
    return secp256k1_scalar();
}

// TODO
secp256k1_scalar modinv(const secp256k1_mult_result &a)
{

    return secp256k1_scalar();
}

// TODO
secp256k1_point point_add()
{
    return secp256k1_point();
}

// TODO
secp256k1_point point_doubling()
{
    return secp256k1_point();
}

// TODO
secp256k1_point double_and_add()
{
    return secp256k1_point();
}

// TODO
secp256k1_scalar ext_euclidian(const secp256k1_mult_result &a)
{
    return secp256k1_scalar();
}
