#include <utility>
#include <cstdint>

#ifndef SECP256K1_H
#define SECP256K1_H

// some data types inspired by Bitcoin Core repo
struct secp256k1_scalar
{
    uint32_t d[8];
};

struct secp256k1_point
{
    secp256k1_scalar x;
    secp256k1_scalar y;
};

struct secp256k1_mult_result
{
    uint32_t d[16];
};

struct secp256k1_key_uncompressed
{
    unsigned char bytes[65];
};

struct secp256k1_key_compressed
{
    unsigned char bytes[33];
};

const secp256k1_scalar SECP256K1_P = {
    0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
    0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFE,0xFFFFFC2F
};

const secp256k1_point SECP256K1_GENERATOR = {
    {0x79BE667E,0xF9DCBBAC,0x55A06295,0xCE870B07,
    0x029BFCDB,0x2DCE28D9,0x59F2815B,0x16F81798},
    {0x483ADA77,0x26A3C465,0x5DA4FBFC,0x0E1108A8,
    0xFD17B448,0xA6855419,0x9C47D08F,0xFB10D4B8}
};

const secp256k1_scalar SECP256K1_ORDER = {
    0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFE,
    0xBAAEDCE6,0xAF48A03B,0xBFD25E8C,0xD0364141
};

bool operator<(const secp256k1_scalar &a, const secp256k1_scalar &b);
bool operator<(const secp256k1_mult_result &a, const secp256k1_mult_result &b);

bool operator>(const secp256k1_scalar &a, const secp256k1_scalar &b);
bool operator>(const secp256k1_mult_result &a, const secp256k1_mult_result &b);

bool operator==(const secp256k1_scalar &a, const secp256k1_scalar &b);
bool operator==(const secp256k1_mult_result &a, const secp256k1_mult_result &b);

bool operator==(const secp256k1_scalar &a, const secp256k1_mult_result &b);
bool operator==(const secp256k1_mult_result &a, const secp256k1_scalar &b);

bool operator<=(const secp256k1_scalar &a, const secp256k1_scalar &b);
bool operator<=(const secp256k1_mult_result &a, const secp256k1_mult_result &b);

bool operator>=(const secp256k1_scalar &a, const secp256k1_scalar &b);
bool operator>=(const secp256k1_mult_result &a, const secp256k1_mult_result &b);

void operator-=(secp256k1_scalar &a, const secp256k1_scalar &b);
void operator-=(secp256k1_mult_result &a, const secp256k1_mult_result &b);

void operator+=(secp256k1_scalar &a, const secp256k1_scalar &b);
void operator+=(secp256k1_mult_result &a, const secp256k1_mult_result &b);

void operator<<=(secp256k1_scalar &a, uint32_t k);
void operator<<=(secp256k1_mult_result &a, uint32_t k);

void operator>>=(secp256k1_scalar &a, uint32_t k);
void operator>>=(secp256k1_mult_result &a, uint32_t k);

int lzcount(const secp256k1_scalar &a);
int lzcount(const secp256k1_mult_result &a);

secp256k1_mult_result padto512(const secp256k1_scalar &a);
secp256k1_scalar shrinkto256(const secp256k1_mult_result &a);

secp256k1_mult_result mult(const secp256k1_scalar &a, const secp256k1_scalar &b);
secp256k1_scalar reduce(const secp256k1_mult_result &a);
secp256k1_mult_result mod(const secp256k1_mult_result &a, const secp256k1_mult_result &m);
secp256k1_scalar fastreduce(const secp256k1_mult_result &a);
secp256k1_scalar ext_euclidian(const secp256k1_mult_result &a);
secp256k1_scalar modinv(const secp256k1_mult_result &a);

secp256k1_point point_add();
secp256k1_point point_doubling();
secp256k1_point double_and_add();

#endif
