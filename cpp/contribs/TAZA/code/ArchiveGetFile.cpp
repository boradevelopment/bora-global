#include "ArchiveGetFile.h"
#if TAZAEXTRAS
fileResult archiveGetFile(V2Archive archive, std::string filename)
{
    fileResult result;
    if (archive.header.files.size() < 1) {
        result.result = 2;
        return result; // BAD Archive
    }

    if (archive.header.files.find(filename) == archive.header.files.end()) {
        result.result = 3; // BAD FILE
        return result;
    }

    std::ifstream file(archive.output, std::ios::binary);
     
    std::vector<uint8_t> data = archive.header.getV2File(file, archive.header.files.find(filename)->second, archive.iv, archive.key);
    if (data.empty()) { // We automatically assume failure if data is empty! DO NOT USE THIS AS AN METHOD OF CHECKING EXISTING DATA! ADD DATA TO YOUR FILES!
        result.result = 1; // FAILURE GETTING FILE
        return result;
    }

    result.fileData = data;
    return result;
}

#endif