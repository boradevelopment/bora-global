#pragma once

#ifndef COMPRESS_H
#include <vector>
#include <thread>
#include <future>
#include <iostream>

void compress_memoryV(std::vector<uint8_t>& data, int compressionLevel = 1);

void compress_memoryVChunk(std::vector<uint8_t>& data,
    uint64_t& compressions,
    std::vector<size_t>& compressionsSizes,
    int compressionLevel = 1,
    size_t chunkSize = 5 * 1024 * 1024 // Default: 5MB
);
#endif // !COMPRESS_H
