#pragma once
#ifndef DECOMPRESS_H
#include <vector>
#include <thread>
#include <future>
#include <iostream>

vec8 decompress_memoryV(std::vector<uint8_t>& compressedData, size_t uncompressedSize);

vec8 decompress_memoryVChunk(std::vector<uint8_t>& compressedData,
    const std::vector<size_t>& chunkSizes, // Vector containing sizes of each compressed chunk
    size_t uncompressedSize // Known total size of the original data before compression
);
#endif
