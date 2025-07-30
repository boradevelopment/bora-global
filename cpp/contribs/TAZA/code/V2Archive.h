#pragma once
#ifndef V2ARCHIVE_H
#include "V2Header.h"
#include <iostream>
#include <filesystem>
#include <utility>
#include "MTFP.h"
#include "TAZAInfo.h"

/// <summary>
/// The main archive structure
/// </summary>
struct V2Archive {
	V2Header header; // The header file
    // The rest in this struct are for creation only.
	bool recursive; // Search through the whole directory instead of the files only in the directory
	bool noencrypt = false; // Disable encryption methods
	bool nocompression = false; // Disable compression methods
	std::string input;  // Input directory
	std::string output; // Output file
	const char* iv  = TAZADEFAULTIV; // AES Encryption keys and iv for encrypted methods
	const char* key = TAZADEFAULTKEY; // AES Encryption keys and iv for encrypted methods
	std::string warn = TAZADEFAULTWARN; // Warning text for users & modders who could corrupt their files.
	int chunkmode = 0; // Chunk mode @ Effiency by default
	int compressionMode = 7; // Compression Mode @ Custom
    bool autoLifeSpan = false;
    FILE* outputFile;

	/// <summary>
	/// Creates an archive based on your V2Archive's metadata
	/// <list type="number">
	/// <item><param name="input">An proper input path (Directory)</param></item>
	/// <item><param name="username">An proper output path (File)</param></item>
	/// </list>
	/// </summary>
	/// <returns>integer</returns>
	int makeArchive();

	/// <summary>
	/// Loads an archieve based on your V2Archives's metadata
	/// </summary>
	/// <returns>integer</returns>
	int getArchive();

    void createHeaderPadding();
    bool finalizeHeader();

    // Creates and processes file
    int addFile(std::string name);
    int addFile(std::vector<uint8_t> data, std::string name = "");
    V2File* addFileAndGet(std::string name, std::string filename);
    V2File* addFileAndGet(std::vector<uint8_t> data, std::string name = "");


    V2Archive();

    V2Archive(std::string name, const std::string& path, const std::string& warn = "");
    ~V2Archive();
};

#endif