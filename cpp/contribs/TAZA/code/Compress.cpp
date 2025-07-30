#include "Compress.h"
#ifndef COMPRESS_H
#include <brotli/encode.h>


void compress_memoryV(std::vector<uint8_t>& data, int compressionLevel) {
    size_t maxOutSize = BrotliEncoderMaxCompressedSize(data.size());
    std::vector<uint8_t> output(maxOutSize);

    size_t outSize = maxOutSize;
    if (!BrotliEncoderCompress(
            BROTLI_DEFAULT_QUALITY, BROTLI_DEFAULT_WINDOW, BROTLI_MODE_GENERIC,
            data.size(), data.data(),
            &outSize, output.data())) {
        throw std::runtime_error("Brotli compression failed");
    }

    output.resize(outSize);
    data = std::move(output);
}

void compress_memoryVChunk(std::vector<uint8_t>& data,
    uint64_t& compressions,
    std::vector<size_t>& compressionsSizes,
    int compressionLevel,
    size_t chunkSize // Default: 5MB
) {
    size_t dataSize = data.size();
    size_t totalChunks = (dataSize + chunkSize - 1) / chunkSize;

    std::vector<std::future<std::vector<uint8_t>>> futures;
    futures.reserve(totalChunks);

    std::vector<size_t> localCompressionSizes(totalChunks);

    for (size_t i = 0; i < totalChunks; i++) {
        size_t offset = i * chunkSize;
        size_t currentChunkSize = std::min(chunkSize, dataSize - offset);

        futures.push_back(std::async(std::launch::async, [&, offset, currentChunkSize, compressionLevel]() -> std::vector<uint8_t> {
            const uint8_t* input = data.data() + offset;
            size_t inputSize = currentChunkSize;

            // Brotli max compressed size (conservative estimate)
            size_t maxCompressedSize = BrotliEncoderMaxCompressedSize(inputSize);
            std::vector<uint8_t> compressedChunk(maxCompressedSize);

            size_t encodedSize = maxCompressedSize;

            BROTLI_BOOL result = BrotliEncoderCompress(
                    compressionLevel,            // quality: 0-11
                    BROTLI_DEFAULT_WINDOW,       // lgwin (default 22)
                    BROTLI_MODE_GENERIC,         // mode
                    inputSize,                   // input size
                    input,                       // input buffer
                    &encodedSize,                // in/out: output size
                    compressedChunk.data()       // output buffer
            );

            if (result == BROTLI_FALSE) {
                std::cerr << "Brotli compression failed." << std::endl;
                return {};
            }

            compressedChunk.resize(encodedSize);
            return compressedChunk;
        }));
    }

    std::vector<uint8_t> output;
    for (size_t i = 0; i < totalChunks; i++) {
        std::vector<uint8_t> chunk = futures[i].get();
        if (!chunk.empty()) {
            compressions++;
            compressionsSizes.push_back(chunk.size());
            output.insert(output.end(), chunk.begin(), chunk.end());
        }
    }

    data = std::move(output);
}

#endif // !COMPRESS_H
