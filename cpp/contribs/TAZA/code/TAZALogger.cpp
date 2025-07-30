// Logger.cpp
#include "TAZALogger.h"
#include <mutex>



std::unordered_map<int, TAZALogger::Listener> TAZALogger::listeners_;
std::atomic<int> TAZALogger::nextId_{ 0 };
std::mutex TAZALogger::mutex_;

int TAZALogger::addListener(Listener callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    int id = nextId_++;
    listeners_[id] = callback;
    return id;
}

void TAZALogger::removeListener(int id) {
    std::lock_guard<std::mutex> lock(mutex_);
    listeners_.erase(id);
}

void TAZALogger::log(const std::string& message, int type,
    const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (listeners_.empty()) return;

    LogEntry entry{ type, message, makeRelativePath(file), line };
    for (const auto& [id, callback] : listeners_) {
        callback(entry);
    }
}

std::string TAZALogger::baseDir_ = "";

void TAZALogger::setBaseDirectory(const std::string& baseDir) {
    std::lock_guard<std::mutex> lock(mutex_);
    baseDir_ = baseDir;

    // Normalize path separators and ensure trailing slash
    std::replace(baseDir_.begin(), baseDir_.end(), '\\', '/');
    if (!baseDir_.empty() && baseDir_.back() != '/') {
        baseDir_ += '/';
    }
}

std::string TAZALogger::makeRelativePath(const std::string& absPath) {
    std::string path = absPath;
    std::replace(path.begin(), path.end(), '\\', '/');

    if (!baseDir_.empty()) {
        size_t basePos = path.find(baseDir_);
        if (basePos != std::string::npos) {
            return path.substr(basePos + baseDir_.length());
        }
    }
    return path;
}