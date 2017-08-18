#include "sha384.h"

__kernel void _sha384(__global char *message, const uint length, __global char *hash) {
    sha384(message, length, hash);
}

void __attribute__((overloadable)) sha384(__global char *message, const uint64 length, uint64 *H) {
    sha384SetInitialVector(H);

    sha384ProcessFullBlocks(message, length, H);

    sha384ProcessNotFullBlock(message, length, H);
}

void __attribute__((overloadable)) sha384(__global char *message, const uint64 length, __global char *hash) {
    uint64 H[SHA384_OUTPUT_BLOCKS];

    sha384(message, length, H);

    uint64ToHex(H, SHA384_OUTPUT_BLOCKS, hash);
}

inline uint64 sha384Ch(uint64 x, uint64 y, uint64 z) {
    return sha512Ch(x, y, z);
}

inline uint64 sha384GetFullBlocksCount(const uint64 length) {
    return length / SHA384_BLOCK_SIZE_IN_BYTES;
}

inline void sha384Iterate(uint64 *H, uint64 *M) {
    // @TODO how can I use this?
//    const uint64 SHA384_K[80] = {
//        0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
//        0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
//        0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
//        0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
//        0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
//        0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
//        0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
//        0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
//        0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
//        0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
//        0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
//        0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
//        0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
//        0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
//        0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
//        0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
//        0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
//        0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
//        0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
//        0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
//    };

    uint64 W[80];

    sha384SetWArray(W, M);

    uint64 a, b, c, d, e, f, g, h;

    a = H[0];
    b = H[1];
    c = H[2];
    d = H[3];
    e = H[4];
    f = H[5];
    g = H[6];
    h = H[7];

    for (uchar t = 0; t < 80; ++t) {
        // @TODO another strange place
        uint64 T1 = h /*+ sha384Sigma1(e) + sha384Ch(e, f, g)*/ /*+ SHA384_K[t]*/ + W[t];
//        uint64 T2 = sha384Sigma0(a) /*+ sha384Maj(a, b, c)*/;
        h = g;
        g = f;
        f = e;
//        e = d + T1;
        d = c;
        c = b;
        b = a;
//        a = T1 + T2;
    }

    H[0] += a;
    H[1] += b;
    H[2] += c;
    H[3] += d;
    H[4] += e;
    H[5] += f;
    H[6] += g;
    H[7] += h;
}

inline uint64 sha384Maj(uint64 x, uint64 y, uint64 z) {
    return sha512Maj(x, y, z);
}

inline void sha384ProcessFullBlocks(__global char *message, const uint64 length, uint64 *H) {
    for (uint64 i = 0, blocks = sha384GetFullBlocksCount(length); i < blocks; ++i) {
        uint64 M[SHA384_BLOCK_SIZE_IN_BITS / SHA384_WORD_SIZE_IN_BITS];

        #pragma unroll
        for (uchar j = 0; j < (SHA384_BLOCK_SIZE_IN_BITS / SHA384_WORD_SIZE_IN_BITS); ++j) {
            M[j] = 0;
        }

        uint64 skipped = i * SHA384_BLOCK_SIZE_IN_BYTES;

        #pragma unroll
        for (uchar j = 0; j < SHA384_BLOCK_SIZE_IN_BYTES; ++j) {
            M[j / 8] |= ((uint64) message[skipped + j])
                    << (SHA384_WORD_SIZE_IN_BITS - ((j + 1) * 8)); // (i << 3) <=> (i * 8)
        }

        sha384Iterate(H, M);
    }
}

inline void sha384ProcessNotFullBlock(__global char *message, const uint64 length, uint64 *H) {
    uint freeBytes = ((uint) (length % SHA384_BLOCK_SIZE_IN_BYTES));

    { // Operate on free bytes
        uint64 M[SHA384_BLOCK_SIZE_IN_BITS / SHA384_WORD_SIZE_IN_BITS];

        #pragma unroll
        for (uchar i = 0; i < (SHA384_BLOCK_SIZE_IN_BITS / SHA384_WORD_SIZE_IN_BITS); ++i) {
            M[i] = 0;
        }

        {
            uint shiftIndex = (uint) (length - freeBytes);

            for (uint i = 0; i < freeBytes; ++i) { // Our hidden if
                uint64 character = (uint64) message[i + shiftIndex];
                M[i / 8] |= (character << (SHA384_WORD_SIZE_IN_BITS - (((i % 8) + 1) * 8)));
            }
        }

        M[freeBytes / 8] |= (((uint64) 1) << (SHA384_WORD_SIZE_IN_BITS - ((freeBytes % 16) * 8 + 1)));

        if ((SHA384_BLOCK_SIZE_IN_BITS - ((freeBytes * 8) + 1)) >= SHA384_PADDING_MESSAGE_LENGTH_IN_BITS) { // Can add padding in this block?
            M[15] |= (length * 8); // Add bits length
        }

        sha384Iterate(H, M);
    }

    if ((SHA384_BLOCK_SIZE_IN_BITS - ((freeBytes * 8) + 1)) < SHA384_PADDING_MESSAGE_LENGTH_IN_BITS) { // Add new padding only block if it is needed
        uint64 M[16];

        #pragma unroll
        for (uchar i = 0; i < 16; ++i) {
            M[i] = 0;
        }

        M[15] |= (length * 8); // Add message length in bits

        sha384Iterate(H, M);
    }
}

inline uint64 sha384sigma0(uint64 x) {
    return sha384ROTR(x, 1) ^ sha384ROTR(x, 8) ^ sha384SHR(x, 7);
}

inline uint64 sha384sigma1(uint64 x) {
    return sha384ROTR(x, 19) ^ sha384ROTR(x, 61) ^ sha384SHR(x, 6);
}

inline uint64 sha384Sigma0(uint64 x) {
    return sha384ROTR(x, 28) ^ sha384ROTR(x, 34) ^ sha384ROTR(x, 39);
}

inline uint64 sha384Sigma1(uint64 x) {
    return sha384ROTR(x, 14) ^ sha384ROTR(x, 18) ^ sha384ROTR(x, 41);
}

inline void sha384SetInitialVector(uint64 *H) {
    const uint64 SHA384_IV[8] = {
        0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17, 0x152fecd8f70e5939,
        0x67332667ffc00b31, 0x8eb44a8768581511, 0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4
    };

    #pragma unroll
    for (uchar i = 0; i < SHA512_OUTPUT_BLOCKS; ++i) {
        H[i] = SHA384_IV[i];
    }
}

inline void sha384SetWArray(uint64 *W, uint64 *M) {
    #pragma unroll
    for (uchar t = 0; t < 16; ++t) {
        W[t] = M[t];
    }

    #pragma unroll
    for (uchar t = 16; t < 80; ++t) {
        W[t] = 0; // @TODO what is going on?
//        W[t] = sha384sigma1(W[t - 2]) + W[t - 7] + sha384sigma0(W[t - 15]) + W[t - 16];
    }
}

