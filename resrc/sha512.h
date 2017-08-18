#ifndef PBKDF2_HMAC_SHA_BREAKER_SHA512_H
    #define PBKDF2_HMAC_SHA_BREAKER_SHA512_H

    #include "types.h"

    #define SHA512_WORD_SIZE_IN_BITS 64

    #define SHA512_BLOCK_SIZE_IN_BITS 1024

    #define SHA512_BLOCK_SIZE_IN_BYTES ((SHA512_BLOCK_SIZE_IN_BITS) / 8)

    #define SHA512_OUTPUT_SIZE_IN_BITS 512

    #define SHA512_OUTPUT_SIZE_IN_BYTES ((SHA512_OUTPUT_SIZE_IN_BITS) / 8)

    #define SHA512_OUTPUT_SIZE_IN_CHARACTERS (SHA512_OUTPUT_SIZE_IN_BYTES * 2)

    #define SHA512_OUTPUT_BLOCKS ((SHA512_OUTPUT_SIZE_IN_BITS) / (SHA512_WORD_SIZE_IN_BITS))

    #define SHA512_PADDING_MESSAGE_LENGTH_IN_BITS 128

    __kernel void _sha512(__global char *message, const uint length, __global char *hash);

    void __attribute__((overloadable)) sha512(__global char *message, const uint64 length, uint64 *H);

    void __attribute__((overloadable)) sha512(__global char *message, const uint64 length, __global char *hash);

    /*inline*/ uint64 sha512Ch(uint64 x, uint64 y, uint64 z);

    /*inline*/ uint64 sha512GetFullBlocksCount(const uint64 length);

    /*inline*/ void sha512Iterate(uint64 *H, uint64 *M);

    /*inline*/ uint64 sha512Maj(uint64 x, uint64 y, uint64 z);

    /*inline*/ void sha512ProcessFullBlocks(__global char *message, const uint64 length, uint64 *H);

    /*inline*/ void sha512ProcessNotFullBlock(__global char *message, const uint64 length, uint64 *H);

//    #define sha512sigma0(x) (sha512ROTR((x), 1) ^ sha512ROTR((x), 8) ^ sha512SHR((x), 7))
//
//    #define sha512sigma1(x) (sha512ROTR((x), 19) ^ sha512ROTR((x), 61) ^ sha512SHR((x), 6))
//
//    #define sha512Sigma0(x) (sha512ROTR((x), 28) ^ sha512ROTR((x), 34) ^ sha512ROTR((x), 39))
//
//    #define sha512Sigma1(x) (sha512ROTR((x), 14) ^ sha512ROTR((x), 18) ^ sha512ROTR((x), 41))

    /*inline*/ uint64 sha512sigma0(uint64 x);

    /*inline*/ uint64 sha512sigma1(uint64 x);

    /*inline*/ uint64 sha512Sigma0(uint64 x);

    /*inline*/ uint64 sha512Sigma1(uint64 x);

    /*inline*/ void sha512SetInitialVector(uint64 *H);

    /*inline*/ void sha512SetWArray(uint64 *W, uint64 *M);

    uint64 sha512SHR(uint64 x, uint64 n);

    uint64 sha512ROTR(uint64 x, uint64 n);

//    #define sha512SHR(x, n) ((x) >> (n))
//
//    #define sha512ROTR(x, n) (((x) >> (n)) | ((x) << ((SHA512_WORD_SIZE_IN_BITS) - (n))))

    void uint64ToHex(uint64 *H, uchar blocks, __global char *string);
#endif // PBKDF2_HMAC_SHA_BREAKER_SHA512_H
