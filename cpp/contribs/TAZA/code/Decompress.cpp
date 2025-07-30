#ifndef DECOMPRESS_H
#include "Decompress.h"
#include <brotli/decode.h>

void decompress_memoryV(std::vector<uint8_t>& compressedData, size_t uncompressedSize) {
    std::vector<uint8_t> output(uncompressedSize);

    size_t outSize = uncompressedSize;
    if (BrotliDecoderDecompress(compressedData.size(), compressedData.data(), &outSize, output.data()) != BROTLI_DECODER_RESULT_SUCCESS) {
        throw std::runtime_error("Brotli decompression failed");
    }

    output.resize(outSize);
    compressedData = std::move(output);
}

void decompress_memoryVChunk(std::vector<uint8_t>& compressedData,
    const std::vector<size_t>& chunkSizes, // Vector containing sizes of each compressed chunk
    size_t uncompressedSize // Known total size of the original data before compression
) {
    std::vector<uint8_t> output(uncompressedSize); // Preallocate buffer
    size_t totalChunks = chunkSizes.size();

    std::vector<std::future<std::vector<uint8_t>>> futures;
    futures.reserve(totalChunks);

    size_t offset = 0;

    // Process each chunk asynchronously
    for (size_t i = 0; i < totalChunks; i++) {
        size_t chunkSize = chunkSizes[i];
        if (offset >= compressedData.size()) {
            std::cerr << "Offset exceeds available compressed data" << std::endl;
            break;
        }

        size_t actualChunkSize = std::min(chunkSize, compressedData.size() - offset);
        size_t localOffset = offset; // Capture by value for lambda
        offset += actualChunkSize;

        futures.push_back(std::async(std::launch::async, [&, localOffset, actualChunkSize]() -> std::vector<uint8_t> {
            std::vector<uint8_t> chunkOutput(uncompressedSize); // Allocate space for output

            size_t available_in = actualChunkSize;
            const uint8_t* next_in = compressedData.data() + localOffset;

            size_t available_out = chunkOutput.size();
            uint8_t* next_out = chunkOutput.data();

            BrotliDecoderResult result = BrotliDecoderDecompress(
                    available_in,
                    next_in,
                    &available_out,
                    next_out
            );

            if (result != BROTLI_DECODER_RESULT_SUCCESS) {
                std::cerr << "Brotli decompression failed with code: " << static_cast<int>(result) << std::endl;
                return {};
            }

            chunkOutput.resize(available_out); // Shrink to actual size
            return chunkOutput;
        }));

    }

    // Collect results
    size_t outOffset = 0;
    for (size_t i = 0; i < totalChunks; i++) {
        std::vector<uint8_t> chunk = futures[i].get();
        if (!chunk.empty()) {
            std::copy(chunk.begin(), chunk.end(), output.begin() + outOffset);
            outOffset += chunk.size();
        }
    }

    compressedData = std::move(output);
}
#endif