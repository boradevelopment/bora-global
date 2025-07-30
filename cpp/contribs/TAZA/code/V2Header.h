#pragma once
#ifndef V2HEADER_H
#include <map>
#include "V2File.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
//#include <Windows.h>
#include <iostream>
#include <iomanip>  // for std::put_time
#include <ctime>    // for std::time_t
#include "Utilities.h"

// The V2Header structure
struct V2Header {
    std::string warn = ""; // Warning text
    std::string version; // Version number string
    std::string file = ""; // file name
    bool encrypted; // Is the whole archive encrypted?
    uint64_t size = 0; // Size of the whole archive
    uint64_t sizeRegular = 0; // Size of the whole archive
    std::map< std::string, V2File> files; // This do not contain the data itself but metadata behind it
    std::time_t creationDate;   // Archive creation date
    std::string author = "Unknown";         // Creator of the archive
    std::string comment = "NONE";        // Archive comment or description
    size_t totalFiles;          // Total number of files in the archive
    std::string compressionMethod; // Compression method for the archive

    // CUSTOM VARIABLES
    std::unordered_map<std::string, std::variant<std::string, int, float, double, bool, uint64_t>> customVariables;

    // Function helpers

    /// <summary>
    /// Gets all pathes in directory! (On the Archive)
    /// </summary>
    /// <param name="DirectoryPath:">An string path</param>
    /// <param name="IsRecursive:">An bool for recursiveness</param>
    /// <returns>An vector of strings</returns>
    std::vector<std::string> getPathesInDirectory(std::string directoryPath, bool isRecursive) const;
    /// <summary>
    /// Writes/Serializes the header to an real file.
    /// </summary>
    /// <param name="Header">The header</param>
    /// <param name="OutputFile:">The output file</param>
    /// <param name="Og">: Useless, I might take it away later on</param>
    /// <returns>A boolean for success or not!</returns>
    bool writeV2HeaderToFile(const V2Header& header, FILE* outputFile, std::string og = "");
    /// <summary>
    /// Reads/Deserializes a header and turn it back into an understandable V2Header
    /// </summary>
    /// <param name="File:">An ifstream file that has the header</param>
    /// <param name="StartPos:">That starting offset of the header</param>
    /// <param name="EndPos:">The ending offset of the header</param>
    /// <returns>A boolean for success or not!</returns>
    bool readV2HeaderFromFile(std::ifstream& file, __int64 startPos, __int64 endPos);
    /// <summary>
    /// Reads an V2File from the archive and turns it back into its original file inside of TZA_[file]
    /// </summary>
    /// <param name="File:">An ifstream file</param>
    /// <param name="V2file:">An v2file containing metadata</param>
    /// <param name="IvStr:">IV for decryption</param>
    /// <param name="KeyStr:">KEY for decryption</param>
    /// <returns>A boolean for success or not!</returns>
    bool readV2FileFromFile(std::ifstream& file, V2File v2file, const char* ivStr = "", const char* keyStr = "");
    /// <summary>
    /// Reads an V2File from the archive and returns its binary.
    /// </summary>
    /// <param name="File:">An ifstream file</param>
    /// <param name="V2file:">An v2file containing metadata</param>
    /// <param name="IvStr:">IV for decryption</param>
    /// <param name="KeyStr:">KEY for decryption</param>
    /// <returns>A vector containing bytes of data.</returns>
    std::vector<uint8_t> getV2File(std::ifstream& file, V2File v2file, const char* ivStr = "", const char* keyStr = "");
    /// <summary>
    /// Logs the V2Header
    /// </summary>
    void logV2Header();


};
#endif