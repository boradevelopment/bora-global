
#include "V2Archive.h"
#include "TAZALogger.h"
#include <cstring>

#ifndef V2ARCHIVE_H




int V2Archive::makeArchive()
{
    TAZA_BASEDIR(TAZABASEDIR);

	if (input.empty() || output.empty()) {
        TAZA_LOG_ERROR("PARAMETER ERROR");
		return 1; // PARAMETER ERROR
	}

#ifdef NOWRITE
    TAZA_LOG_ERROR("WRITE FUNCTIONS ARE DISABLED IN BUILD!");
	return -1; // NOT SUPPORTED ON LIBRARY 
#else
    header.version = TAZAVERSION;

	std::string fulOut = output;
	const std::string directoryPath = input;


#if WIN32
    fopen_s(&outputFile, fulOut.c_str(), "wb");
#elif __linux__
    outputFile = fopen64(fulOut.c_str(), "wb");
#endif

    if (outputFile == nullptr) {
        TAZA_LOG_ERROR("INPUT ERROR");
        return 2; // INPUT ERROR
    }

    fwrite(this->warn.c_str(), sizeof(char), strlen(this->warn.c_str()), outputFile);
    const char padding[] = "#00000000000000000000$00000000000000000000\n";
    fwrite(padding, sizeof(char), strlen(padding), outputFile);


    int threads = getRecommendedThreadCount();
    std::vector<std::string> files = getFilesInDirectory(directoryPath, recursive, threads);
    if (files.empty()) {
        TAZA_LOG_ERROR("DIRECTORY EMPTY: "+directoryPath);
        return 3; // DIRECTORY EMPTY
    }
    else {
        header.warn = this->warn.c_str();

        if (!noencrypt) {
            header.encrypted = true;
        }


        std::string zipname = std::filesystem::path(output).filename().string();
        header.file = zipname.c_str();
        header.totalFiles = files.size();
        header.creationDate = std::time(nullptr);
        header.compressionMethod = "zlib";

        multiThreadedProcessFiles(files, directoryPath, zipname, outputFile, header, true, iv, key, nocompression, noencrypt, chunkmode, compressionMode);

        bool be = header.writeV2HeaderToFile(header, outputFile, fulOut);
        if (!be) {
            TAZA_LOG_ERROR("OUTPUT ERROR");
            return 4; // OUTPUT ERROR
        }

        TAZA_LOG_NORMAL("ARCHIVE CREATED!");
        return 0; // ARCHIVE INITALIZED AND CREATED
    }
#endif
}

int V2Archive::getArchive()
{
    TAZA_BASEDIR(TAZABASEDIR);

    long long headbeg = 0;
    long long headend = 0;


#ifdef NOREAD
    TAZA_LOG_ERROR("READ FUNCTIONS ARE DISABLED IN BUILD!");
    return -1; // NOT SUPPORTED ON LIBRARY 
#else

    
    std::ifstream file(output, std::ios::binary);

    if (!file.is_open()) {
        TAZA_LOG_ERROR("BAD OUTPUT");
        return 1; // BAD FILE
    }

    std::string line;
    std::string headerpos;

    int lineCount = 0;

    while (lineCount < 2 && std::getline(file, line)) {
        if (lineCount + 1 == 2) {
            headerpos = line;
        }
        lineCount++;
    }

    if (lineCount < 2) {
        TAZA_LOG_ERROR("NOT IN TAZA FORMAT");
        return 2; // NOT TAZA FORMAT
    }

    size_t hashPos = headerpos.find('#');
    size_t dollarPos = headerpos.find('$');

    if (hashPos != std::string::npos && dollarPos != std::string::npos) {
        //std::string headbegStr = headerpos.substr(hashPos + 1, dollarPos - hashPos - 1);  // Value between # and $
        //std::string headendStr = headerpos.substr(dollarPos + 1);  // Value after $

        int64_t start = 0, end = 0;
#if WIN32
        sscanf_s(headerpos.c_str(), "#%lld$%lld", &headbeg, &headend);
#elif __linux__
        sscanf(headerpos.c_str(), "#%lld$%lld", &headbeg, &headend);
#endif

        //// Convert to integers (use stoll for long long if needed)
        //headbeg = std::stoll(headbegStr);  // Convert to long long (or int if smaller range is needed)
        //headend = std::stoll(headendStr);

#ifdef DETAILEDLOGGING
        TAZA_LOG_NORMAL("Header Start Position:"+headbeg);
        TAZA_LOG_NORMAL("Header Start End:" + headend);
#endif
    }
    else {
        TAZA_LOG_ERROR("NOT IN TAZA FORMAT");
        return 2; // NOT TAZA FORMAT
    }

    if (header.readV2HeaderFromFile(file, headbeg, headend)) {
        TAZA_LOG_NORMAL("ARCHIVE INITALIZED");
        return 0;
    }
    else {
        TAZA_LOG_ERROR("CORRUPTED ARCHIVE");
        return 3; // CORRUPTED ARCHIVE
    }
#endif
}

int V2Archive::addFile(std::string name) {
    // NOTE: This will not initalize header, use initalize
    V2File fileMeta;
    fileMeta.file = name;
    fileMeta.filePath = "";

    // Prepare the data in a more memory-efficient way
    std::vector<uint8_t> buffer = readFileMain(fileMeta.file);
    if (buffer.empty()) {
        return 1; // empty buffer [file does not exist]
    }

    if (outputFile == nullptr) {
        TAZA_LOG_ERROR("OUTPUT ERROR | Make sure you're output file is initalized if you are manually adding files");
        return 2; // OUTPUT ERROR
    }

    processFile(fileMeta, buffer, outputFile, header, true, iv, key, nocompression, noencrypt, chunkmode, compressionMode);
    // NOTE: This will not add an header. Use finalize
    return 0;
}

int V2Archive::addFile(std::vector<uint8_t> data, std::string name) {
    // NOTE: This will not initalize header, use initalize
    V2File fileMeta;
    fileMeta.file = std::move(name);
    fileMeta.filePath = "";


    if (data.empty()) {
        return 1; // empty buffer, why did you provide nothing?
    }

    if (outputFile == nullptr) {
        TAZA_LOG_ERROR("OUTPUT ERROR | Make sure you're output file is initalized if you are manually adding files");
        return 2; // OUTPUT ERROR
    }

    processFile(fileMeta, data, outputFile, header, true, iv, key, nocompression, noencrypt, chunkmode, compressionMode);
    // NOTE: This will not add an header. Use finalize
    return 0;
}

void V2Archive::createHeaderPadding() {
    if (outputFile == nullptr) {
        TAZA_LOG_ERROR("OUTPUT ERROR | Make sure you're output file is initalized if you are manually adding padding");
        return; // OUTPUT ERROR
    }

    fwrite(this->warn.c_str(), sizeof(char), strlen(this->warn.c_str()), outputFile);
    const char padding[] = "#00000000000000000000$00000000000000000000\n";
    fwrite(padding, sizeof(char), strlen(padding), outputFile);

    header.warn = this->warn.c_str();

    if (!noencrypt) {
        header.encrypted = true;
    }


    std::string zipname = "test";
    header.file = zipname.c_str();
    header.totalFiles = files.size();
    header.creationDate = std::time(nullptr);
    header.compressionMethod = "zlib";
}

bool V2Archive::finalizeHeader() {
    return header.writeV2HeaderToFile(header, outputFile);
}

V2Archive::V2Archive(){
    TAZA_BASEDIR(TAZABASEDIR);
}

V2Archive::V2Archive(std::string name, const std::string& path, const std::string& warn){
    TAZA_BASEDIR(TAZABASEDIR);
#if WIN32
    if (fopen_s(&outputFile, path.c_str(), "wb") != 0 || !outputFile) {
        throw std::runtime_error("Failed to open archive file for writing.");
    }
#elif __linux__
    outputFile = fopen64(path.c_str(), "wb");
    if (!outputFile) {
        throw std::runtime_error("Failed to open archive file for writing.");
    }
#endif
    header.version = TAZAVERSION;
    autoLifeSpan = true;
    header.file = std::move(name);
    if(!warn.empty()) this->warn = warn + "\n";
    createHeaderPadding();
}

V2Archive::~V2Archive(){
    if(autoLifeSpan) finalizeHeader();
}

V2File* V2Archive::addFileAndGet(std::vector<uint8_t> data, std::string name) {
    // NOTE: This will not initalize header, use initalize
    V2File fileMeta;
    fileMeta.file = std::move(name);
    fileMeta.filePath = "";
    fileMeta.regularsize = data.size();


    if (data.empty()) {
        return nullptr; // empty buffer, why did you provide nothing?
    }

    if (outputFile == nullptr) {
        TAZA_LOG_ERROR("OUTPUT ERROR | Make sure you're output file is initalized if you are manually adding files");
        return nullptr; // OUTPUT ERROR
    }

    processFile(fileMeta, data, outputFile, header, true, iv, key, nocompression, noencrypt, chunkmode, compressionMode);
    // NOTE: This will not add an header. Use finalize
    return &header.files[fileMeta.file];
}

V2File* V2Archive::addFileAndGet(std::string name,  std::string filename) {
    // NOTE: This will not initalize header, use initalize
    V2File fileMeta;
    if(filename.empty())
        fileMeta.file = std::filesystem::path(name).filename().string();
    else
        fileMeta.file = filename;

    fileMeta.filePath = name;

    // Prepare the data in a more memory-efficient way
    std::vector<uint8_t> buffer = readFileMain(fileMeta.filePath);
    if (buffer.empty()) {
        return nullptr; // empty buffer [file does not exist]
    }

    if (outputFile == nullptr) {
        TAZA_LOG_ERROR("OUTPUT ERROR | Make sure you're output file is initalized if you are manually adding files");
        return nullptr; // OUTPUT ERROR
    }

    header.totalFiles++;

    processFile(fileMeta, buffer, outputFile, header, true, iv, key, nocompression, noencrypt, chunkmode, compressionMode);
    // NOTE: This will not add an header. Use finalize
    return &header.files[fileMeta.file];
}


#endif