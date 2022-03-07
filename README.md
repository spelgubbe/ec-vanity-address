## EC Vanity Address Generator

### Requirements
* Building the project requires make
* g++ are required to the build the project from the makefile

### Purpose
The main purpose of this project is to implement reasonably fast crypto math for fast generation of vanity Secp256k1 public keys.

### MVP
* Implement sufficient correct big number arithmetic operators for 256/512-bit numbers
* Implement Point Addition [on the Secp256k1 curve]
* Implement Point Doubling
* Implement Double-and-add

### Extensions
* Implement fast 256-bit number mod p where p = 2^k - c
* Implement Half Extended Euclidean Algorithm for calculating inverses mod p
* Implement key format that fulfills relevant BIP (for Bitcoin) / EIP (for Ethereum)
* Research further

### Limitations
* Doing crypto math fast is hard
* Not going to prevent side-channels or security concerns

### What the program should do
1. Generate random data
2. Find corresponding public key to that random data, if applicable
3. Evaluate whether the key fulfills some regex chosen by the user
4. Repeat

## Some resources
1. https://www.youtube.com/watch?v=vnpZXJL6QCQ
2. https://www.youtube.com/watch?v=zTt4gvuQ6sY
3. https://crypto.stackexchange.com/questions/14803/fast-modular-reduction
4. https://en.wikipedia.org/wiki/Elliptic_curve_point_multiplication
