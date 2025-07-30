#include "Utilities.h"
//#include <dirent/dirent.h> - DEPRECATED DUE TO PLATFORM ISSUES
#if WIN32
#include "Windows.h"
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#endif
#include <filesystem>
namespace fs = std::filesystem;

std::vector<std::string> files;
std::mutex filesMutex;
std::queue<std::string> workQueue;
std::mutex queueMutex;
std::condition_variable queueCond;
std::atomic<bool> doneProcessing(false);

int getRecommendedThreadCount() {
    // Get the number of logical cores
    unsigned int numThreads = std::thread::hardware_concurrency();

    // If it fails to retrieve, return a default value
    if (numThreads == 0) {
        return 4;  // You can set any default number of threads, or calculate based on your needs
    }

    return numThreads;  // Return the logical core count as the recommended thread count
}
void worker() {
    while (true) {
        std::string directoryPath;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (workQueue.empty()) {
                if (doneProcessing) break;
                queueCond.wait(lock);
                continue;
            }
            directoryPath = workQueue.front();
            workQueue.pop();
        }

        fs::path dirPath(directoryPath);
        if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
            std::cerr << "Error: Not a valid directory: " << directoryPath << std::endl;
            continue;
        }

        for (const auto& entry : fs::directory_iterator(dirPath)) {
            fs::path fullPath = entry.path();

            if (entry.is_directory()) {
                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    workQueue.push(fullPath.string());
                }
                queueCond.notify_one();
            } else if (entry.is_regular_file()) {
                {
                    std::lock_guard<std::mutex> lock(filesMutex);
                    files.push_back(fullPath.string());
                }
            }
        }
    }
}
std::vector<std::string> getFilesInDirectory(const std::string& rootPath, bool recursive, int numThreads) {
    // Initialize the thread pool
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread(worker));  // Launch worker threads
    }

    // Add the root path to the queue
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        workQueue.push(rootPath);  // rootPath is initially blank or set to the desired starting point
    }
    queueCond.notify_one();  // Notify workers that work is available

    // Wait for all work to be completed
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        doneProcessing = true;
    }

    // Notify all worker threads to finish
    queueCond.notify_all();

    // Join all threads
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Return the accumulated file paths
    return files;
}

std::vector<uint8_t> stringToVector(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}
std::string getMiddleSubstring(std::string myString, size_t startPos, size_t endPos, std::string replaceC, std::string replaceC2) {
    if (startPos >= myString.length() || endPos >= myString.length() || startPos > endPos) {
        // Invalid positions, handle the error (e.g., throw an exception)
        // For now, return an empty string
        return "";
    }





    std::string nSt = myString.substr(startPos, endPos - startPos);



    size_t pos = nSt.find(replaceC);
    if (pos != std::string::npos)
    {
        nSt.replace(pos, replaceC.length(), "");
    }



    size_t pos2 = nSt.find(replaceC2);
    if (pos2 != std::string::npos)
    {
        nSt.replace(pos2, replaceC.length(), "");
    }

    size_t pos3 = nSt.find_first_of("\n");

    if (pos3 != std::string::npos)
        nSt = nSt.substr(pos3, nSt.length());


    //std::cout << nSt;

    return nSt;

}
void getFilesInDirectory(const std::string& directoryPath, const std::string& basePath, std::vector<std::string>& fileNames, bool recursive) {
    fs::path rootPath = fs::u8path(directoryPath);

    if (!fs::exists(rootPath) || !fs::is_directory(rootPath)) {
        std::cerr << "Error opening directory: " << directoryPath << std::endl;
        return;
    }

    auto processEntry = [&](const fs::directory_entry& entry) {
        if (!entry.is_regular_file())
            return;

        fs::path fullPath = entry.path();
        fs::path relativePath = fs::relative(fullPath, rootPath);

        std::string combinedPath = basePath.empty()
                                   ? relativePath.generic_string()
                                   : (fs::u8path(basePath) / relativePath).generic_string();

        fileNames.push_back(combinedPath);
    };

    if (recursive) {
        for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
            processEntry(entry);
        }
    } else {
        for (const auto& entry : fs::directory_iterator(rootPath)) {
            processEntry(entry);
        }
    }
}
std::vector<std::string> getFilesInDirectory(const std::string& directoryPath, bool recursive) {
    std::vector<std::string> fileNames;
    getFilesInDirectory(directoryPath, "", fileNames, recursive);
    return fileNames;
}

std::size_t getFileSize(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        return 0;
    }
    return static_cast<std::size_t>(file.tellg());
}
std::vector<uint8_t> readFileWithStream(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    file.seekg(0, std::ios::end);
    std::size_t fileSize = static_cast<std::size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        throw std::runtime_error("Failed to read file using ifstream.");
    }
    return buffer;
}
std::vector<uint8_t> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate); // Open in binary mode and seek to end
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::streamsize fileSize = file.tellg(); // Get the file size
    file.seekg(0, std::ios::beg); // Go back to the beginning of the file

    std::vector<uint8_t> buffer(static_cast<size_t>(fileSize));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        throw std::runtime_error("Failed to read file: " + filename);
    }

    return buffer;
}
std::vector<uint8_t> readFileWithMmap(const std::string& filename) {
#if WIN32
    // Convert std::string to LPCWSTR (wide string for Windows API)
    std::wstring wFilename(filename.begin(), filename.end());

    // Open the file
    HANDLE hFile = CreateFileW(
        wFilename.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    if (hFile == INVALID_HANDLE_VALUE) {
        return std::vector<uint8_t>();
    }

    // Get the file size
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to get file size.");
    }

    // Create a file mapping object
    HANDLE hMap = CreateFileMapping(
        hFile,
        nullptr,
        PAGE_READONLY,
        static_cast<DWORD>(fileSize.HighPart),
        static_cast<DWORD>(fileSize.LowPart),
        nullptr);

    if (hMap == nullptr) {
        CloseHandle(hFile);
        throw std::runtime_error("Failed to create file mapping.");
    }

    // Map the file into memory
    void* mapView = MapViewOfFile(
        hMap,
        FILE_MAP_READ,
        0,  // High-order DWORD of the file offset
        0,  // Low-order DWORD of the file offset
        0); // Map the entire file

    if (mapView == nullptr) {
        CloseHandle(hMap);
        CloseHandle(hFile);
        throw std::runtime_error("Failed to map view of file.");
    }

    // Copy the file content into a vector
    std::vector<uint8_t> buffer(static_cast<uint8_t*>(mapView), static_cast<uint8_t*>(mapView) + fileSize.QuadPart);

    // Unmap and close handles
    UnmapViewOfFile(mapView);
    CloseHandle(hMap);
    CloseHandle(hFile);

    return buffer;
#else
    // Open the file
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        return {};
    }

    // Get the file size
    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        throw std::runtime_error("Failed to get file size.");
    }

    size_t fileSize = static_cast<size_t>(st.st_size);
    if (fileSize == 0) {
        close(fd);
        return {};
    }

    // Memory-map the file
    void* mapped = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Failed to mmap file.");
    }

    // Copy contents into a vector
    std::vector<uint8_t> buffer(static_cast<uint8_t*>(mapped), static_cast<uint8_t*>(mapped) + fileSize);

    // Unmap and close
    munmap(mapped, fileSize);
    close(fd);

    return buffer;
#endif
}
std::vector<uint8_t> readFileRange(const std::string& filename, std::streampos start, std::streampos end) {
    // Ensure start and end positions are valid
    if (start < 0 || end < start) {
        throw std::invalid_argument("Invalid start or end position.");
    }

    // Open the file in binary mode
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Seek to the start position
    file.seekg(0, std::ios::end); // Get the file size
    std::streampos fileSize = file.tellg();
    if (start >= fileSize) {
        throw std::out_of_range("Start position is beyond the end of the file.");
    }

    // Adjust end position if it exceeds file size
    if (end > fileSize) {
        end = fileSize;
    }

    std::streamsize rangeSize = end - start;
    file.seekg(start);

    // Read the specified range
    std::vector<uint8_t> buffer(static_cast<size_t>(rangeSize));
    if (!file.read(reinterpret_cast<char*>(buffer.data()), rangeSize)) {
        throw std::runtime_error("Failed to read the specified range from the file.");
    }

    return buffer;
}
std::vector<uint8_t> readFileMain(const std::string& filename, std::size_t size) {
    std::size_t fileSize = getFileSize(filename);
    if (fileSize == 0) {
        return std::vector<uint8_t>();
    }

    if (fileSize >= size) {
        return readFileWithMmap(filename);
    }
    else {
        return readFileWithStream(filename);
    }
}
size_t getRecommendedChunkSize(size_t fileSize, int type) {
    if (fileSize < 1 * 1024 * 1024)  // Files smaller than 1MB
        return fileSize;  // No need to chunk

    if (type == 0) {  // Large chunks (efficient but fewer chunks)
        if (fileSize <= 100 * 1024 * 1024)
            return fileSize / 2;  // Use 1/2 of the file size as the chunk
        if (fileSize <= 1000 * 1024 * 1024)
            return fileSize / 4;  // Use 1/4 of the file size
        return fileSize / 8;  // Large files use 1/8 of the file size
    }

    else if (type == 1) {  // Small chunks (faster but more chunks)
        if (fileSize <= 100 * 1024 * 1024)
            return std::max<size_t>(fileSize / 10, 64 * 1024);  // 1/10 of the size, min 64KB
        if (fileSize <= 1000 * 1024 * 1024)
            return std::max<size_t>(fileSize / 20, 256 * 1024);  // 1/20 of the size, min 256KB
        return std::max<size_t>(fileSize / 40, 1 * 1024 * 1024);  // 1/40 of the size, min 1MB
    }

    return fileSize;  // Default fallback if failure
}
