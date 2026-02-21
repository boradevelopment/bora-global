#include "V2Header.h"

#ifndef V2HEADER_H
#include "Decrypt.h"
#include "Decompress.h"
#include <filesystem>
#include <thread>
#include <mutex>
#include "TAZALogger.h"
#include <cstring>
#include <sstream>

namespace fs = std::filesystem;

void writeCustomVariables(FILE* f,
                          const std::unordered_map<std::wstring, std::variant<std::string, std::wstring, int, float, double, bool, uint64_t>>& vars)
{
    // Write number of entries
    uint64_t count = vars.size();
    fwrite(&count, sizeof(count), 1, f);

    for (const auto& [key, val] : vars) {
        // Write key length + key string

        auto utf8Key = wstringToUtf8(key);
        uint64_t keyLength = utf8Key.size();
        fwrite(&keyLength, sizeof(keyLength), 1, f);
        fwrite(utf8Key.data(), sizeof(char), keyLength, f);

        // Write type + value
        if (std::holds_alternative<std::string>(val)) {
            uint8_t typeId = 1;
            fwrite(&typeId, 1, 1, f);

            const std::string& s = std::get<std::string>(val);
            uint64_t strLen = s.size();
            fwrite(&strLen, sizeof(strLen), 1, f);
            fwrite(s.data(), 1, strLen, f);

        }
        else if (std::holds_alternative<int>(val)) {
            uint8_t typeId = 2;
            fwrite(&typeId, 1, 1, f);
            int v = std::get<int>(val);
            fwrite(&v, sizeof(v), 1, f);

        } else if (std::holds_alternative<float>(val)) {
            uint8_t typeId = 3;
            fwrite(&typeId, 1, 1, f);
            float v = std::get<float>(val);
            fwrite(&v, sizeof(v), 1, f);

        } else if (std::holds_alternative<double>(val)) {
            uint8_t typeId = 4;
            fwrite(&typeId, 1, 1, f);
            double v = std::get<double>(val);
            fwrite(&v, sizeof(v), 1, f);

        } else if (std::holds_alternative<bool>(val)) {
            uint8_t typeId = 5;
            fwrite(&typeId, 1, 1, f);
            bool v = std::get<bool>(val);
            fwrite(&v, sizeof(v), 1, f);

        } else if (std::holds_alternative<uint64_t>(val)) {
            uint8_t typeId = 6;
            fwrite(&typeId, 1, 1, f);
            uint64_t v = std::get<uint64_t>(val);
            fwrite(&v, sizeof(v), 1, f);

        } if (std::holds_alternative<std::wstring>(val)) {
            uint8_t typeId = 7; // different type ID for wstring
            fwrite(&typeId, 1, 1, f);

            const std::wstring& ws = std::get<std::wstring>(val);
            auto utf8S = wstringToUtf8(ws);
            uint64_t strLen = utf8S.size();
            fwrite(&strLen, sizeof(strLen), 1, f);
            fwrite(utf8S.data(), sizeof(char), strLen, f);
        }
        else {
            // Unknown type [I'll leave it as is since that isn't possible]
        }
    }
}

void readCustomVariables(std::ifstream& f,
                         std::unordered_map<std::wstring, std::variant<std::string, std::wstring, int, float, double, bool, uint64_t>>& vars)
{
    vars.clear();

    uint64_t count = 0;
    f.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (uint64_t i = 0; i < count; ++i) {

        uint64_t keyLen = 0;
        f.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
        std::string utf8Key(keyLen, '\0');
        f.read(&utf8Key[0], keyLen);

        auto key = utf8ToWstring(utf8Key);

        uint8_t typeId = 0;
        f.read(reinterpret_cast<char*>(&typeId), 1);

        switch (typeId) {
            case 1: { // string
                uint64_t strLen = 0;
                f.read(reinterpret_cast<char*>(&strLen), sizeof(strLen));
                std::string val(strLen, '\0');
                f.read(&val[0], strLen);
                vars[key] = val;
                break;
            }
            case 2: { // int
                int val = 0;
                f.read(reinterpret_cast<char*>(&val), sizeof(val));
                vars[key] = val;
                break;
            }
            case 3: { // float
                float val = 0.0f;
                f.read(reinterpret_cast<char*>(&val), sizeof(val));
                vars[key] = val;
                break;
            }
            case 4: { // double
                double val = 0.0;
                f.read(reinterpret_cast<char*>(&val), sizeof(val));
                vars[key] = val;
                break;
            }
            case 5: { // bool
                bool val = false;
                f.read(reinterpret_cast<char*>(&val), sizeof(val));
                vars[key] = val;
                break;
            }
            case 6: { // uint64_t
                uint64_t val = 0;
                f.read(reinterpret_cast<char*>(&val), sizeof(val));
                vars[key] = val;
                break;
            }
            case 7: { // string
                uint64_t strLen = 0;
                f.read(reinterpret_cast<char*>(&strLen), sizeof(strLen));
                std::string valUTF8(strLen, '\0');
                f.read(&valUTF8[0], strLen);
                auto val = utf8ToWstring(valUTF8);
                vars[key] = val;
                break;
            }
            default:
                // Unknown type
                break;
        }
    }
}


std::wstring formatBytes(size_t bytes) {
    const std::vector<std::wstring> units = { L"Bytes", L"KB", L"MB", L"GB", L"TB", L"PB", L"EB", L"ZB", L"YB" };

    if (bytes == 0) return L"0 Bytes";

    size_t unitIndex = 0;
    double formattedSize = static_cast<double>(bytes);

    // Loop to scale the size down to an appropriate unit (KB, MB, etc.)
    while (formattedSize >= 1024 && unitIndex < units.size() - 1) {
        formattedSize /= 1024;
        unitIndex++;
    }

    // Prepare the result in a readable format
    std::wstringstream oss;
    oss.precision(2);
    oss << std::fixed << formattedSize << " " << units[unitIndex];

    return oss.str();
}

void createDirectories(const std::wstring& path) {
    try {
        // Convert the path to a filesystem path object
        fs::path p(path);

        // Extract the directory path (excluding the filename)
        fs::path dirPath = p.parent_path();

        // Create the directories recursively if they don't exist
        if (!fs::exists(dirPath)) {
            if (fs::create_directories(dirPath)) {
                return; // OK
            }
            else {
                return; // BAD
            }
        }
        else {
            return; // BAD
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error creating directories: " << e.what() << std::endl;
    }
}


void readString(std::ifstream& file, std::wstring& str) {
    std::string utf8Str;
    char ch;
    utf8Str.clear();
    while (file.get(ch) && ch != '\0') {
        utf8Str.push_back(ch);
    }
    str = utf8ToWstring(utf8Str);
}

void readLengthPrefixedString(std::ifstream& file, std::wstring& str) {
    std::string utf8Str;
    size_t length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    if (length > 0) {
        utf8Str.resize(length);
        file.read(reinterpret_cast<char*>(&utf8Str[0]), length * sizeof(char));
        str = utf8ToWstring(utf8Str);
    } else {
        utf8Str.clear();
    }
}

std::mutex fileMutex;  // Mutex to protect file access


void writeFilePart(const std::wstring& fileName, const V2File& v2File, FILE* outputFile) {
    std::lock_guard<std::mutex> guard(fileMutex);  // Lock the mutex to ensure only one thread writes at a time

    std::string fileNameLengthUTF8 = wstringToUtf8(fileName);
    size_t fileNameLength = fileNameLengthUTF8.size();
    fwrite(&fileNameLength, sizeof(fileNameLength), 1, outputFile);
    fwrite(fileNameLengthUTF8.data(), sizeof(char), fileNameLength, outputFile);

    std::string fileUTF8 = wstringToUtf8(v2File.file);
    size_t fileLength = fileUTF8.size();
    fwrite(&fileLength, sizeof(fileLength), 1, outputFile);
    fwrite(fileUTF8.data(), sizeof(char), fileLength, outputFile);

    std::string filePathUTF8 = wstringToUtf8(v2File.filePath);
    size_t filePathLength = v2File.filePath.size();
    fwrite(&filePathLength, sizeof(filePathLength), 1, outputFile);
    fwrite(filePathUTF8.data(), sizeof(char), filePathLength, outputFile);

    fwrite(&v2File.encrypted, sizeof(v2File.encrypted), 1, outputFile);
    fwrite(&v2File.lineStart, sizeof(v2File.lineStart), 1, outputFile);
    fwrite(&v2File.lineEnd, sizeof(v2File.lineEnd), 1, outputFile);
    fwrite(&v2File.size, sizeof(v2File.size), 1, outputFile);
    fwrite(&v2File.creationDate, sizeof(v2File.creationDate), 1, outputFile);
    fwrite(&v2File.modificationDate, sizeof(v2File.modificationDate), 1, outputFile);
    fwrite(&v2File.accessDate, sizeof(v2File.accessDate), 1, outputFile);

    std::string compressionMethodUTF8 = wstringToUtf8(v2File.compressionMethod);
    size_t compressionMethodLength = compressionMethodUTF8.size();
    fwrite(&compressionMethodLength, sizeof(compressionMethodLength), 1, outputFile);
    fwrite(compressionMethodUTF8.data(), sizeof(char), compressionMethodLength, outputFile);

    fwrite(&v2File.compressedSize, sizeof(v2File.compressedSize), 1, outputFile);
    fwrite(&v2File.regularsize, sizeof(v2File.regularsize), 1, outputFile);
    // CHUNKS
    fwrite(&v2File.isCompressionChunked, sizeof(v2File.isCompressionChunked), 1, outputFile);
    fwrite(&v2File.compressionChunkSize, sizeof(v2File.compressionChunkSize), 1, outputFile);
    fwrite(&v2File.compressionChunks, sizeof(v2File.compressionChunks), 1, outputFile);
    size_t vec2Size = v2File.compressionChunksSizes.size();
    fwrite(&vec2Size, sizeof(size_t), 1, outputFile);
    fwrite(v2File.compressionChunksSizes.data(), sizeof(size_t), vec2Size, outputFile);

    fwrite(&v2File.isEncryptionChunked, sizeof(v2File.isEncryptionChunked), 1, outputFile);
    fwrite(&v2File.encryptionChunkSize, sizeof(v2File.encryptionChunkSize), 1, outputFile);
    fwrite(&v2File.encryptionChunks, sizeof(v2File.encryptionChunks), 1, outputFile);
    size_t vecSize = v2File.encryptionChunkSizes.size();
    fwrite(&vecSize, sizeof(size_t), 1, outputFile);
    fwrite(v2File.encryptionChunkSizes.data(), sizeof(size_t), vecSize, outputFile);

    // Write File Custom Variables
    writeCustomVariables(outputFile, v2File.customVariables);
}


std::vector<std::wstring> V2Header::getPathesInDirectory(std::wstring directoryPath, bool isRecursive) const {
    std::vector<std::wstring> paths;

    // Normalize directory path to ensure it ends with a slash
    if (!directoryPath.empty() && directoryPath.back() != '/') {
        directoryPath += '/';
    }

    for (const auto& [filePath, file] : files) {
        // Check if the file path starts with the specified directory
        if (filePath.rfind(directoryPath, 0) == 0) {
            std::wstring relativePath = filePath.substr(directoryPath.size());

            // For non-recursive mode, skip paths containing further slashes
            if (!isRecursive && relativePath.find('/') != std::string::npos) {
                continue;
            }

            // Add the matching path
            paths.push_back(filePath);
        }
    }

    return paths;
}

bool V2Header::writeV2HeaderToFile(const V2Header& header, FILE* outputFile, std::string og)
{
#ifndef NOWRITE
    if (!outputFile) {
        return false;
    }

    __int64 headerStart = 0;
    __int64 headerEnd = 0;

    // Mark the start of the V2Header
    headerStart = ftellout(outputFile);



    // Serialize V2Header metadata
    fwrite(wstringToUtf8(header.version).c_str(), sizeof(char), strlen(wstringToUtf8(header.version).c_str()) + 1, outputFile);
    fwrite(wstringToUtf8(header.file).c_str(), sizeof(char), strlen(wstringToUtf8(header.file).c_str()) + 1, outputFile);
    fwrite(wstringToUtf8(header.warn).c_str(), sizeof(char), strlen(wstringToUtf8(header.warn).c_str())+ 1, outputFile);
    fwrite(&header.encrypted, sizeof(header.encrypted), 1, outputFile);
    fwrite(&header.size, sizeof(uint64_t), 1, outputFile);
    fwrite(&header.sizeRegular, sizeof(uint64_t), 1, outputFile);
    fwrite(&header.creationDate, sizeof(header.creationDate), 1, outputFile);

    auto authorUTF8 = wstringToUtf8(header.author);
    size_t authorLength = authorUTF8.size();
    fwrite(&authorLength, sizeof(authorLength), 1, outputFile);
    fwrite(authorUTF8.data(), sizeof(char), authorLength, outputFile);

    auto commentUTF8 = wstringToUtf8(header.comment);
    size_t commentLength = commentUTF8.size();
    fwrite(&commentLength, sizeof(commentLength), 1, outputFile);
    fwrite(commentUTF8.data(), sizeof(char), commentLength, outputFile);

    fwrite(&header.totalFiles, sizeof(header.totalFiles), 1, outputFile);

    auto compressionMethodUTF8 = wstringToUtf8(header.compressionMethod);
    size_t compressionMethodLength = compressionMethodUTF8.size();
    fwrite(&compressionMethodLength, sizeof(compressionMethodLength), 1, outputFile);
    fwrite(compressionMethodUTF8.data(), sizeof(char), compressionMethodLength, outputFile);

    // Serialize each V2File in the map
    size_t filesCount = header.files.size();
    fwrite(&filesCount, sizeof(filesCount), 1, outputFile);
    
    std::vector<std::thread> threads;

    for (const auto& [fileName, v2File] : header.files) {
        threads.push_back(std::thread(writeFilePart, fileName, v2File, outputFile));
    }

    for (auto& t : threads) {
        t.join();
    }

    // Finally Write Header Custom Variables
    writeCustomVariables(outputFile, customVariables);

    // Mark the end of the V2Header
    headerEnd = ftellout(outputFile);

    // GO TO THE PADDING and overwrite it with the actual header start and header length
    
    // Move file pointer to start of second line (after warn)
    fseek(outputFile, strlen(wstringToUtf8(this->warn).c_str()), SEEK_SET);

    // Overwrite with actual start/end
    fprintf(outputFile, "#%020lld$%020lld\n", headerStart, headerEnd);

    fclose(outputFile);

    std::filesystem::rename(header.tempPath, header.filePath);

    return true;
#endif
}

bool V2Header::readV2HeaderFromFile(std::ifstream& file, __int64 startPos, __int64 endPos)
{
#ifndef NOREAD
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        if (!file.is_open()) {
            std::cerr << "File not open!" << std::endl;
            return false;
        }

        __int64 dataSize = endPos - startPos;
        if (dataSize <= 0) {
            std::cerr << "Invalid range specified!" << std::endl;
            return false;
        }

        // Seek to the start position
        file.seekg(startPos, std::ios::beg);

        readString(file, this->version);
        readString(file, this->file);
        readString(file, this->warn);

        // Read fixed-size data
        file.read(reinterpret_cast<char*>(&this->encrypted), sizeof(this->encrypted));
        file.read(reinterpret_cast<char*>(&this->size), sizeof(this->size));
        file.read(reinterpret_cast<char*>(&this->sizeRegular), sizeof(this->sizeRegular));
        file.read(reinterpret_cast<char*>(&this->creationDate), sizeof(this->creationDate));

        // Read length-prefixed strings
        readLengthPrefixedString(file, this->author);
        readLengthPrefixedString(file, this->comment);

        file.read(reinterpret_cast<char*>(&this->totalFiles), sizeof(this->totalFiles));
        readLengthPrefixedString(file, this->compressionMethod);

        // File header data
        size_t filesCount;
        file.read(reinterpret_cast<char*>(&filesCount), sizeof(filesCount));

        for (size_t i = 0; i < filesCount; i++) {
            std::wstring fileName;
            readLengthPrefixedString(file, fileName);

            V2File v2File;
            readLengthPrefixedString(file, v2File.file);
            readLengthPrefixedString(file, v2File.filePath);

            file.read(reinterpret_cast<char*>(&v2File.encrypted), sizeof(v2File.encrypted));
            file.read(reinterpret_cast<char*>(&v2File.lineStart), sizeof(v2File.lineStart));
            file.read(reinterpret_cast<char*>(&v2File.lineEnd), sizeof(v2File.lineEnd));
            file.read(reinterpret_cast<char*>(&v2File.size), sizeof(v2File.size));
            file.read(reinterpret_cast<char*>(&v2File.creationDate), sizeof(v2File.creationDate));
            file.read(reinterpret_cast<char*>(&v2File.modificationDate), sizeof(v2File.modificationDate));
            file.read(reinterpret_cast<char*>(&v2File.accessDate), sizeof(v2File.accessDate));

            readLengthPrefixedString(file, v2File.compressionMethod);

            file.read(reinterpret_cast<char*>(&v2File.compressedSize), sizeof(v2File.compressedSize));
            file.read(reinterpret_cast<char*>(&v2File.regularsize), sizeof(v2File.regularsize));

            // Read CHUNKS
            file.read(reinterpret_cast<char*>(&v2File.isCompressionChunked), sizeof(v2File.isCompressionChunked));
            file.read(reinterpret_cast<char*>(&v2File.compressionChunkSize), sizeof(v2File.compressionChunkSize));
            file.read(reinterpret_cast<char*>(&v2File.compressionChunks), sizeof(v2File.compressionChunks));
            size_t vec2Size;
            file.read(reinterpret_cast<char*>(&vec2Size), sizeof(size_t));
            v2File.compressionChunksSizes.resize(vec2Size);
            file.read(reinterpret_cast<char*>(v2File.compressionChunksSizes.data()), vec2Size * sizeof(size_t));

            file.read(reinterpret_cast<char*>(&v2File.isEncryptionChunked), sizeof(v2File.isEncryptionChunked));
            file.read(reinterpret_cast<char*>(&v2File.encryptionChunkSize), sizeof(v2File.encryptionChunkSize));
            file.read(reinterpret_cast<char*>(&v2File.encryptionChunks), sizeof(v2File.encryptionChunks));
            
            size_t vecSize;
            file.read(reinterpret_cast<char*>(&vecSize), sizeof(size_t));
            v2File.encryptionChunkSizes.resize(vecSize);
            file.read(reinterpret_cast<char*>(v2File.encryptionChunkSizes.data()), vecSize * sizeof(size_t));

            readCustomVariables(file, v2File.customVariables);

            this->files[v2File.file] = v2File;
        }

        readCustomVariables(file, customVariables);
    }
    catch (const std::ifstream::failure& e) {
        std::cerr << "File I/O error: " << e.what() << std::endl;
        return false;
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return false;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        return false;
    }

    return true;
#endif
}

bool V2Header::readV2FileFromFile(std::ifstream& file, V2File v2file, const char* ivStr, const char* keyStr)
{
#if !NOREAD
    try {
        // Get the data
        size_t liStart = v2file.lineStart;
        size_t liEnd = v2file.lineEnd;
        size_t properSize = liEnd - liStart;

#ifdef DETAILEDLOGGING
        TAZA_LOG_NORMAL(v2file.file + " Start Position:" + std::to_string(liStart));
        TAZA_LOG_NORMAL(v2file.file + " End Position:" + std::to_string(liEnd));
        TAZA_LOG_NORMAL(v2file.file + " Proper Size:" + std::to_string(properSize));
#endif

        file.seekg(liStart, std::ios::beg);
        std::vector<uint8_t> buffer(properSize);
        file.read(reinterpret_cast<char*>(buffer.data()), properSize);
       // Make sure its unencrypted and uncompressed
//        if (v2file.encrypted) {
//            if (v2file.isEncryptionChunked) {
//                decryptUint8Chunked(buffer, ivStr, keyStr, v2file.encryptionChunkSizes);
//            }
//            else {
//                decryptUint8(buffer, ivStr, keyStr);
//            }
//        }
        if (v2file.isCompressionChunked) {
            decompress_memoryVChunk(buffer, v2file.compressionChunksSizes, v2file.regularsize);
        }
        else {
            decompress_memoryV(buffer, v2file.regularsize);
        }

        std::wstring opath = L"TAZ_"+ this->file + L"\\" + v2file.file;
        createDirectories(opath);

#if WIN32
         std::ofstream outFile(L"./" + opath, std::ios::binary);
#else
        std::ofstream outFile("./" + wstringToUtf8(opath), std::ios::binary);
#endif
        if (!outFile.is_open()) {
            return false;
        }

        // Write the extracted data to the output file
        outFile.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        if (!outFile) {
            return false;
        }

        // Close the output file
        outFile.close();
    }
    catch (const std::ifstream::failure& e) {
        std::cerr << "File I/O error: " << e.what() << std::endl;
        return false;
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return false;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
    catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
        return false;
    }

    return true;
#endif
}

std::vector<uint8_t> V2Header::getV2File(std::ifstream& file, V2File v2file, const char* ivStr, const char* keyStr)
{
#if !NOREAD
    try {
        // Get the data
        uint64_t liStart = v2file.lineStart;
        uint64_t liEnd = v2file.lineEnd;
        uint64_t properSize = liEnd - liStart;

        
#ifdef DETAILEDLOGGING
        TAZA_LOG_NORMAL(v2file.file + " Start Position:" + std::to_string(liStart));
        TAZA_LOG_NORMAL(v2file.file + " End Position:" + std::to_string(liEnd));
        TAZA_LOG_NORMAL(v2file.file + " Proper Size:" + std::to_string(properSize));
#endif


        file.seekg(liStart, std::ios::beg);
        std::vector<uint8_t> buffer(properSize);
        file.read(reinterpret_cast<char*>(buffer.data()), properSize);

        if(v2file.encrypted){
            printf("This is a very old version of TAZA, which was originally encrypted. This is not supported to fix cross platform issues \n");
            buffer.clear(); // Encrypted TAZA Formats are not supported due to cross platform issues.
            return buffer;
        }

        if (v2file.isCompressionChunked) {
            decompress_memoryVChunk(buffer, v2file.compressionChunksSizes, v2file.regularsize);
        }
        else {
            decompress_memoryV(buffer, v2file.regularsize);
        }
        
        return buffer;
    }
    catch (const std::ifstream::failure& e) {
        std::cerr << "[TAZA] File I/O error: " << e.what() << std::endl;
        return std::vector<uint8_t>();
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "[TAZA] Invalid argument: " << e.what() << std::endl;
        return std::vector<uint8_t>();
    }
    catch (const std::runtime_error& e) {
        std::cerr << "[TAZA] Runtime error: " << e.what() << std::endl;
        return std::vector<uint8_t>();
    }
    catch (const std::exception& e) {
        std::cerr << "[TAZA] Error: " << e.what() << std::endl;
        return std::vector<uint8_t>();
    }
    catch (...) {
        std::cerr << "[TAZA] Unknown error occurred!" << std::endl;
        return std::vector<uint8_t>();
    }

#endif
}

void V2Header::logV2Header()
{

    tm fileTM;
    std::time_t fileTime = std::time(nullptr);
#if WIN32
    localtime_s(&fileTM, &fileTime);
#elif __linux__|| __APPLE__
    localtime_r(&fileTime, &fileTM);
#endif

    auto originalCout = std::wcout.rdbuf();  // Save original cout buffer

    std::ostringstream oss;
    oss << "v2header." << std::put_time(&fileTM, "%Y-%m-%d_%H-%M-%S") << ".log";

    std::wofstream logFile(oss.str(), std::ios::out | std::ios::trunc);
    if (!logFile) {
        TAZA_LOG_ERROR("UNABLE TO OPEN LOG FILE");
    }

    if (logFile.is_open()) {

        std::wcout.rdbuf(logFile.rdbuf());

        tm timeInfo;

#if WIN32
        localtime_s(&timeInfo, &creationDate);
#elif __linux__ || __APPLE__
        localtime_r(&creationDate, &timeInfo);
#endif

    std::wcout << L"==== V2Header Metadata ====\n";
    std::wcout << L"Version: " << version << "\n";
    std::wcout << L"File Name: " << file << "\n";
    std::wcout << L"Encrypted: " << (encrypted ? "Yes" : "No") << "\n";
    std::wcout << L"Archive Size: " << formatBytes(size) << "\n";
    std::wcout << L"Predicted Export Size [Regular Size of Archive]: " << formatBytes(sizeRegular) << "\n";
    std::wcout << L"Creation Date: " << std::put_time(&timeInfo, L"%Y-%m-%d %H:%M:%S") << "\n";
    std::wcout << L"Author: " << author << "\n";
    std::wcout << L"Comment: " << comment << "\n";
    std::wcout << L"Total Files: " << totalFiles << "\n";
    std::wcout << L"Compression Method: " << compressionMethod << "\n";
    std::wcout << L"Warning: " << warn << "\n";

    std::wcout << "\n==== Files Metadata ====\n";
    for (const auto& [fileName, file] : files) {

        tm timeInfoCreate;
        tm timeInfoModify;
        tm timeInfoAccess;
#if WIN32
        localtime_s(&timeInfoCreate, &file.creationDate);
        localtime_s(&timeInfoModify, &file.modificationDate);
        localtime_s(&timeInfoAccess, &file.accessDate);
#elif __linux__ || __APPLE__
        localtime_r(&file.creationDate, &timeInfoCreate);
        localtime_r(&file.modificationDate, &timeInfoModify);
        localtime_r(&file.accessDate, &timeInfoAccess);
#endif

        std::wcout << L"File: " << fileName << "\n";
        std::wcout << L"  - Encrypted: " << (file.encrypted ? "Yes" : "No") << "\n";
        std::wcout << L"  - Line Start: " << file.lineStart << "\n";
        std::wcout << L"  - Line End: " << file.lineEnd << "\n";
        std::wcout << L"  - Size: " << file.size << "\n";
        std::wcout << L"  - Created: " << std::put_time(&timeInfoCreate, L"%Y-%m-%d %H:%M:%S") << "\n";
        std::wcout << L"  - Modified: " << std::put_time(&timeInfoModify, L"%Y-%m-%d %H:%M:%S") << "\n";
        std::wcout << L"  - Accessed: " << std::put_time(&timeInfoAccess, L"%Y-%m-%d %H:%M:%S") << "\n";
        std::wcout << L"  - Compression Method: " << file.compressionMethod << "\n";
        std::wcout << L"  - Compressed Size: " << formatBytes(file.compressedSize) << "\n";
        std::wcout << L"  - Regular Size: " << formatBytes(file.regularsize) << "\n";

        // Log compression chunking details if enabled
        if (file.isCompressionChunked) {
            std::wcout << "  - Compression Chunked: Yes\n";
            std::wcout << "  - Compression Chunk Size: " << formatBytes(file.compressionChunkSize) << "\n";
            std::wcout << "  - Number of Compression Chunks: " << file.compressionChunks << "\n";
            std::wcout << "  - Compression Chunk Sizes: \n";
            for (size_t chunkSize : file.compressionChunksSizes) {
                std::wcout << "      " << formatBytes(chunkSize) << "\n";
            }
            std::wcout << "\n";
        }
        else {
            std::wcout << "  - Compression Chunked: No\n";
        }

        // Log encryption chunking details if enabled
        if (file.isEncryptionChunked) {
            std::wcout << "  - Encryption Chunked: Yes\n";
            std::wcout << "  - Encryption Chunk Size: " << formatBytes(file.encryptionChunkSize) << "\n";
            std::wcout << "  - Number of Encryption Chunks: " << file.encryptionChunks << "\n";
            std::wcout << "  - Encryption Chunk Sizes: \n";
            for (size_t chunkSize : file.encryptionChunkSizes) {
                std::wcout << "      " << formatBytes(chunkSize) << "\n";
            }
            std::wcout << "\n";
        }
        else {
            std::wcout << "  - Encryption Chunked: No\n";
        }

        std::wcout << "---------------------------\n";
    }
    std::wcout.rdbuf(originalCout);
    logFile.close();
    TAZA_LOG_NORMAL("V2 Header will be located at the executable directory!");
    }

    

}


#endif
