#include "ArchiveExport.h"

exportResult exportArchive(V2Archive archive) {
    exportResult result;
    if (archive.header.files.size() < 1) {
        result.result = 2;
        return result; // BAD Archive
    }

    std::ifstream file(archive.output, std::ios::binary);

    bool isAllSuccess = true;
    for (const auto& [fileName, v2File] : archive.header.files) {
        if (!archive.header.readV2FileFromFile(file, v2File, archive.iv, archive.key)) {
            TAZA_LOG_ERROR(fileName + ": Failed to export file!");
            isAllSuccess = false;
        }
        else {
            result.resultVector.push_back(fileName);
        }
    }

    if (!isAllSuccess) {
        result.result = 1;
    }
    return result;
}
