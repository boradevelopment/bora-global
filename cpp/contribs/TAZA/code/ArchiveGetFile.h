#pragma once
#if TAZAEXTRAS
#include "V2Archive.h"

/// <summary>
/// An struct designed to bring relevent information all in one,
/// </summary>
struct fileResult {
    int result = 0; // the final result
    std::vector<uint8_t> fileData;
};

/// <summary>
/// An archive command that fetches/gets the requested filename provided. 
/// </summary>
/// <param name="Archive:">Your V2Archieve file</param>
/// <param name="Filename:">Your string filename (This is an relative path! DO NOT put an drive path)</param>
/// <returns>fileResult</returns>
fileResult archiveGetFile(V2Archive archive, std::string filename);
#endif