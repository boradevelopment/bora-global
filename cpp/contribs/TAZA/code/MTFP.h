#pragma once

#ifndef MTFP_H
#include <thread>
#include <mutex> 
#include <vector>
#include "Utilities.h"
#include <iostream>
#include <fstream>
#include "V2File.h"
#include "V2Header.h"
#include "Compress.h"
#include "Encrypt.h"

extern std::mutex allC_mutex;

void processFilesBatch(const std::vector<std::string>& files, const std::string& directoryPath, const std::string& zipname, FILE* outputFile, V2Header& header, bool encrypt, const char* ivStr, const char* keyStr, bool nocompress, bool noencrypt, int chunkmode, int compressionMode);
void multiThreadedProcessFiles(const std::vector<std::string>& files, const std::string& directoryPath, const std::string& zipname, FILE* outputFile, V2Header& header, bool encryptAll, const char* ivStr, const char* keyStr, bool nocompress, bool noencrypt, int chunkmode, int compressionMode);
void processFile(V2File& fileMeta, std::vector<uint8_t>& buffer, FILE* outputFile, V2Header& header, bool encrypt, const char* ivStr, const char* keyStr, bool nocompress, bool noencrypt, int chunkmode, int compressionMode);
#endif