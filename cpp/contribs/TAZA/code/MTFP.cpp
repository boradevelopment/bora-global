//#include <zlib/zlib.h>
#include "MTFP.h"
#ifndef MTFP_H

#include "TAZALogger.h"




std::mutex allC_mutex;

void processFile(V2File& fileMeta, std::vector<uint8_t>& buffer, FILE* outputFile, V2Header& header, bool encrypt, const char* ivStr, const char* keyStr, bool nocompress, bool noencrypt, int chunkmode, int compressionMode){
    // Update metadata with file size and compression
    fileMeta.regularsize = buffer.size();

    // I may need to create an parameter for this
    std::size_t MB1 = 10 * 1024 * 1024;
    std::size_t FSIZE = fileMeta.regularsize;

//
    if (!nocompress) {
        if (FSIZE >= MB1) {
            fileMeta.isCompressionChunked = true;
            fileMeta.compressionChunkSize = getRecommendedChunkSize(FSIZE, chunkmode);
            compress_memoryVChunk(buffer, fileMeta.compressionChunks, fileMeta.compressionChunksSizes, compressionMode, fileMeta.compressionChunkSize);
        }
        else {
            fileMeta.isCompressionChunked = false;
            compress_memoryV(buffer, 9);
        }
        fileMeta.compressedSize = buffer.size();
        fileMeta.compressionMethod = L"brotli";
    }
//
//
//
//    if (!noencrypt) {
//        if (FSIZE >= MB1) {
//            fileMeta.isEncryptionChunked = true;
//            fileMeta.encryptionChunkSize = getRecommendedChunkSize(buffer.size(), chunkmode);
//            encryptUint8Chunked(buffer, fileMeta.encryptionChunks, fileMeta.encryptionChunkSizes, ivStr, keyStr, fileMeta.encryptionChunkSize);
//        }
//        else {
//            encryptUint8(buffer, ivStr, keyStr);
//        }
//        fileMeta.encrypted = true;
//    }


    // Set file metadata

    fileMeta.creationDate = std::time(nullptr);
    fileMeta.modificationDate = fileMeta.creationDate;
    fileMeta.accessDate = fileMeta.creationDate;

    // Protect shared data access using a mutex
    std::lock_guard<std::mutex> lock(allC_mutex);

    fileMeta.lineStart = ftellout(outputFile);

    fwrite(buffer.data(), sizeof(uint8_t), buffer.size(), outputFile); // Write compressed and encrypted file data

    // Update file metadata with end position and size

    fileMeta.lineEnd = ftellout(outputFile);

    fileMeta.size = fileMeta.lineEnd - fileMeta.lineStart;
    header.size += fileMeta.size;
    header.sizeRegular += fileMeta.regularsize;

    // Insert the file metadata into the header map
    header.files.insert({ fileMeta.file, fileMeta });

    // Clear temporary buffers to free memory
    buffer.clear();
    TAZA_LOG_NORMAL(wstringToUtf8(L"Finished writing "+fileMeta.file));
}

void processFilesBatch(const std::vector<std::wstring>& files, const std::string& directoryPath, const std::wstring& zipname, FILE* outputFile, V2Header& header, bool encrypt, const char* ivStr, const char* keyStr, bool nocompress, bool noencrypt, int chunkmode, int compressionMode) {
    for (std::wstring fileName : files) {
        //TAZA_LOG_NORMAL("Started writing " + fileName);
        auto cov = fileName;
        auto covE = fileName;
        std::wstring path = utf8ToWstring(directoryPath) + L"\\";
        size_t found = covE.find(path);

        if (found != std::string::npos) {
            covE.erase(found, path.length());
        }

        fileName = covE;

     

        V2File fileMeta;
        fileMeta.file = fileName;
        fileMeta.filePath = cov;

        // Prepare the data in a more memory-efficient way
        std::vector<uint8_t> buffer = readFileMain(cov);
        if (buffer.empty()) {
            continue; // empty buffer
        }

        processFile(fileMeta, buffer, outputFile, header, encrypt, ivStr, keyStr, nocompress, noencrypt, chunkmode, compressionMode);
    }
}

void multiThreadedProcessFiles(const std::vector<std::wstring>& files, const std::string& directoryPath, const std::wstring& zipname, FILE* outputFile, V2Header& header, bool encryptAll, const char* ivStr, const char* keyStr, bool nocompress, bool noencrypt, int chunkmode, int compressionMode) {
    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4; // Default to 4 if hardware_concurrency can't be determined

    size_t numFiles = files.size();
    size_t filesPerThread = numFiles / numThreads;
    size_t remainderFiles = numFiles % numThreads;

    std::vector<std::thread> threads;

    auto it = files.begin();
    for (unsigned int i = 0; i < numThreads; ++i) {
        size_t currentBatchSize = filesPerThread + (i < remainderFiles ? 1 : 0);
        std::vector<std::wstring> batch(it, it + currentBatchSize);
        it += currentBatchSize;

        threads.emplace_back(processFilesBatch, batch, directoryPath, zipname, outputFile, std::ref(header), encryptAll, ivStr, keyStr, nocompress, noencrypt, chunkmode, compressionMode);
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

#endif