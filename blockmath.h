#ifndef BLOCKMATH_H
#define BLOCKMATH_H
#include <cstdint>
int blockwise_lzcount(const uint32_t * a, int nblocks);
int blockwise_cmp(const uint32_t * a, const uint32_t * b, int nblocks);
void blockwise_shl(uint32_t * a, uint32_t k, int nblocks);
void blockwise_shr(uint32_t * a, uint32_t k, int nblocks);
void blockwise_sub(uint32_t * a, const uint32_t * b, int nblocks);
void blockwise_add(uint32_t * a, const uint32_t * b, int nblocks);
void blockwise_add_two(uint32_t * a, uint32_t part2, uint32_t part1, int part1_block, int nblocks);
void blockwise_mult(uint32_t * res, const uint32_t * a, const uint32_t * b, int nblocks);
#endif
