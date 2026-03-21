// BORA_INCLUDED_IN_SDK
// ^^^ Never remove this! This indicates that this should be included in production builds of libbora
// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.
/* 
 * FileName: Hashing.h
 * Purpose: Storing hash utility files, to be continued.
 */
#pragma once
#include <iostream>

inline uint64_t BoraHash64(const char* str, size_t len, uint64_t seed = 0x9E3779B97F4A7C15ULL) {
    uint64_t hash = seed;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(str);

    for (size_t i = 0; i < len; ++i) {
        // MurmurHash3 style mixer
        uint64_t k = data[i];
        k *= 0xff51afd7ed558ccdULL;
        k ^= k >> 33;
        k *= 0xc4ceb9fe1a85ec53ULL;
        k ^= k >> 33;

        hash ^= k;
        hash = (hash << 31) | (hash >> 33); // Rotate left 31
        hash = hash * 5 + 0xe6546b64;
    }

    // Finalize
    hash ^= len;
    hash ^= hash >> 33;
    hash *= 0xff51afd7ed558ccdULL;
    hash ^= hash >> 33;
    hash *= 0xc4ceb9fe1a85ec53ULL;
    hash ^= hash >> 33;

    return hash;
}

#pragma pack(push, 1)
struct SerializedCommandHeader {
    uint64_t nameHash;
    uint64_t size;
};
#pragma pack(pop)