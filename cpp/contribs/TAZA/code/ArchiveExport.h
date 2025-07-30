#pragma once

#include "V2Archive.h"
#include "TAZALogger.h"

/// <summary>
/// An struct designed to bring relevent information all in one,
/// </summary>
struct exportResult {
    int result = 0; // the final result
    std::vector<std::string> resultVector; // The amount of files exported properly
};



/// <summary>
///  An Archive command allows you to export all the files located inside of an file.
/// </summary>
/// <param name="Archive:">Your V2Archieve file</param>
/// <returns>an exportResult</returns>
exportResult exportArchive(V2Archive archive);

