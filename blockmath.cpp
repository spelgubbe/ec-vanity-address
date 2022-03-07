#include "blockmath.h"
#include <cassert>
#include <stdexcept>
#include <iostream>


int blockwise_lzcount(const uint32_t * a, int nblocks)
{
    int idx = 0;
    int lzc = 0;
    while (a[idx] == 0 && idx < nblocks) {
        lzc += sizeof(uint32_t)*8;
        idx++;
    }
    if (idx < nblocks) {
        return lzc + __builtin_clz(a[idx]);
    } else {
        return lzc;
    }
}

int blockwise_cmp(const uint32_t * a, const uint32_t * b, int nblocks)
{
    for(int i = 0; i < nblocks; i++)
    {
        if (a[i] < b[i])
        {
            return -1;
        } else if (b[i] < a[i])
        {
            return 1;
        }
    }
    return 0;
}

void blockwise_shl(uint32_t * a, uint32_t k, int nblocks)
{
    uint32_t res[nblocks] = {0};
    std::copy(a, a + nblocks, res);
    int blocksize = sizeof(*a) * 8;

    if (k > nblocks*blocksize) {
        throw new std::runtime_error("Can't shift left more bits than exist in the number");
    }

    int blockshifts = k/blocksize;
    int rest = k - blockshifts * blocksize;

    // first shift entire blocks if possible
    if (blockshifts) {
        for(int i = 0; i < nblocks; i++) {
            if (i + blockshifts < nblocks) {
                res[i] = a[i + blockshifts];
            } else {
                res[i] = 0;
            }
        }
    }
    

    // then do smaller adjustments
    if (rest) {
        for(int i = 0; i < nblocks; i++) {
            if (i < nblocks - 1) {
                res[i] <<= rest;
                res[i] |= (res[i+1] >> (blocksize-rest));
            } else {
                res[i] <<= rest;
            }
        }
    }
    
    for(int i = 0; i < nblocks; i++) {
        a[i] = res[i];
    }
}

void blockwise_shr(uint32_t * a, uint32_t k, int nblocks)
{
    uint32_t res[nblocks] = {0};
    std::copy(a, a + nblocks, res);
    int blocksize = sizeof(*a) * 8;

    if (k > nblocks*blocksize) {
        throw new std::runtime_error("Can't shift left more bits than exist in the number");
    }

    int blockshifts = k/blocksize;
    int rest = k - blockshifts * blocksize;
    
    // first shift entire blocks if possible
    if (blockshifts) {
        for(int i = nblocks - 1; i >= 0; i--) {
            if (i - blockshifts >= 0) {
                res[i] = a[i - blockshifts];
            } else {
                res[i] = 0;
            }
        }
    }

    // then do smaller adjustments
    if (rest) {
        for(int i = nblocks - 1; i >= 0; i--) {
            if (i > 0) {
                res[i] >>= rest;
                res[i] |= (res[i-1] << (blocksize-rest));
            } else {
                res[i] >>= rest;
            }
        }
    }
    
    for(int i = 0; i < nblocks; i++) {
        a[i] = res[i];
    }
}

void blockwise_sub(uint32_t * a, const uint32_t * b, int nblocks)
{
    uint32_t res[nblocks] = {0};
    std::copy(a, a + nblocks, res);
    uint32_t carry = 0;
    for(int i = nblocks-1; i >= 0 ; i--)
    {
        res[i] -= b[i] + carry;
        if (carry == 1 && a[i] - b[i] - carry >= a[i]) {
            // underflow
        } else if (a[i] - b[i] > a[i]) {
            // underflow
            carry = 1;
        } else {
            carry = 0;
        }

        if (i == 0 && carry == 1) {
            throw new std::runtime_error("Subtraction underflow");
        }
    }

    for(int i = 0; i < nblocks; i++)
    {
        a[i] = res[i];
    }
}

void blockwise_add(uint32_t * a, const uint32_t * b, int nblocks)
{
    uint32_t res[nblocks] = {0};
    std::copy(a, a + nblocks, res);
    uint32_t carry = 0;
    for(int i = nblocks-1; i >= 0; i--)
    {
        res[i] += b[i] + carry;
        if (carry == 1 && a[i] + b[i] + carry <= a[i]) {
            // overflow
        } else if (a[i] + b[i] < a[i]) {
            // overflow
            carry = 1;
        } else {
            carry = 0;
        }

        if (i == 0 && carry == 1) {
            throw new std::runtime_error("Addition overflow");
        }
    }

    for(int i = 0; i < nblocks; i++)
    {
        a[i] = res[i];
    }
}


void blockwise_add_two(uint32_t * a, uint32_t part2, uint32_t part1, int part1_block, int nblocks)
{
    int part2_block = part1_block - 1;
    int carry1 = a[part1_block] + part1 < a[part1_block];

    a[part1_block] += part1;

    int carry2 = 0;
    if (part2 + carry1 > 0) {
        carry2 = a[part2_block] + part2 + carry1 <= a[part2_block];
        a[part2_block] += part2 + carry1;
    }

    // should probably be its own function
    if (carry2) {
        // do carry rest
        for(int i = part1_block-2; i >= 0 ; i--)
        {
            if (a[i] + carry2 < a[i]) {
                a[i] += carry2;
            } else {
                a[i] += carry2;
                carry2 = 0;
                break;
            }

            if (i == 0 && carry2 == 1) {
                throw new std::runtime_error("Addition overflow");
            }
        }
    }
}

void blockwise_mult(uint32_t * res, const uint32_t * a, const uint32_t * b, int nblocks)
{
    for(int i = nblocks - 1; i >= 0 ; i--)
    {
        for(int j = nblocks - 1; j >= 0; j--)
        {
            int block_base_idx = i+j+1;
            
            uint64_t tmp = (uint64_t)a[i] * (uint64_t)b[j];

            uint32_t msb = tmp >> 32;
            uint32_t lsb = (tmp & (uint64_t)0xFFFFFFFF);

            blockwise_add_two(res, msb, lsb, block_base_idx, 2*nblocks);
        }
    }
}