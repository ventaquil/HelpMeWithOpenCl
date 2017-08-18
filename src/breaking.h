#ifndef PBKDF2_HMAC_SHA_BREAKER_BREAKING_H
    #define PBKDF2_HMAC_SHA_BREAKER_BREAKING_H

    #include <cstdlib>
    #include <profiling.h>

    enum Algorithm {SHA384, SHA512};

    void breaker(Algorithm algorithm, char *salt, size_t salt_length, char *key, size_t key_length);

    void breaker(Algorithm algorithm, const char *salt, size_t salt_length, const char *key, size_t key_length);
#endif // PBKDF2_HMAC_SHA_BREAKER_BREAKING_H
