#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
//#include <Windows.h>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <sys/stat.h>
#include <thread>
#include <iostream>

// A thread-safe vector to accumulate file paths
extern std::vector<std::string> files;
extern std::mutex filesMutex;
extern std::queue<std::string> workQueue;
extern std::mutex queueMutex;
extern std::condition_variable queueCond;
extern std::atomic<bool> doneProcessing;

int getRecommendedThreadCount();
void worker();
std::vector<std::string> getFilesInDirectory(const std::string& rootPath, bool recursive, int numThreads);
std::vector<uint8_t> stringToVector(const std::string& str);
std::string getMiddleSubstring(std::string myString, size_t startPos, size_t endPos, std::string replaceC, std::string replaceC2);
void getFilesInDirectory(const std::string& directoryPath, const std::string& basePath, std::vector<std::string>& fileNames, bool recursive);
std::vector<std::string> getFilesInDirectory(const std::string& directoryPath, bool recursive);
std::size_t getFileSize(const std::string& filename);
std::vector<uint8_t> readFileWithStream(const std::string& filename);
std::vector<uint8_t> readFile(const std::string& filename);
std::vector<uint8_t> readFileWithMmap(const std::string& filename);
std::vector<uint8_t> readFileRange(const std::string& filename, std::streampos start, std::streampos end);
std::vector<uint8_t> readFileMain(const std::string& filename, std::size_t size = 5 * 1024 * 1024);
size_t getRecommendedChunkSize(size_t fileSize, int type = 0);