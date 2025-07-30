#pragma once
#ifndef V2FILE_H
#include <ctime>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <cstdint>

struct V2File {
    std::string file = ""; // File name
    std::string filePath = ""; // Path of file from original directory
    bool encrypted = false; // Was it encrypted or is it free?
    uint64_t lineStart = 0;  // Exact locations of where the file data inside the file is, this will allow easier compression and fast reading 
    uint64_t lineEnd = 0;  // Exact locations of where the file data inside the file is, this will allow easier compression and fast reading
    size_t size = 0; // Total size of the file
    std::time_t creationDate = 0;  // File creation date (timestamp)
    std::time_t modificationDate = 0; // Last modification date
    std::time_t accessDate = 0;       // Last access date
    std::string compressionMethod = ""; // Compression method used (e.g., "zlib", "gzip")
    size_t compressedSize = 0;        // Size of the file after compression
    size_t regularsize = 0; // size before compression

    // CHUNKS - COMPRESSION
    bool isCompressionChunked = false; // If the compression is designed to be chunked
    size_t compressionChunkSize = 0; // The calculated chunk size
    uint64_t compressionChunks = 0; // Amount of chunks
    std::vector<size_t> compressionChunksSizes; // List of chunk sizes in their real time file.

    // CHUNKS - ENCRYPTION
    bool isEncryptionChunked = false; // If the encryption was designed to be chunked
    size_t encryptionChunkSize = 0; // The calculated chunk size
    uint64_t encryptionChunks = 0; // Amount of chunks
    std::vector<size_t> encryptionChunkSizes; // List of chunk sizes in their real time file.

    // CUSTOM VARIABLES
    std::unordered_map<std::string, std::variant<std::string, int, float, double, bool, uint64_t>> customVariables;
};

#endif