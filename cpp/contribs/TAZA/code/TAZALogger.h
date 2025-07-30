#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <algorithm>

#define TAZA_LOG(msg, type) TAZALogger::log(msg, type, __FILE__, __LINE__)
#define TAZA_LOG_NORMAL(msg) TAZA_LOG(msg, TAZALogTypes::Normal)
#define TAZA_LOG_WARNING(msg) TAZA_LOG(msg, TAZALogTypes::Warning)
#define TAZA_LOG_ERROR(msg) TAZA_LOG(msg, TAZALogTypes::Error)
#define TAZA_BASEDIR(dir) TAZALogger::setBaseDirectory(dir)

namespace TAZALogTypes {
    constexpr int Normal = 0;
    constexpr int Warning = 1;
    constexpr int Error = 2;
}

struct LogEntry {
    int type;           // Log type (use TAZALogTypes constants)
    std::string message; // Log message
    std::string file;    // Source file where log originated
    int line;           // Line number in source file
};

class TAZALogger {
public:
    using Listener = std::function<void(const LogEntry&)>;

    // Add a listener and return its ID
    static int addListener(Listener callback);

    // Remove a listener by ID
    static void removeListener(int id);

    // Core logging method
    static void log(const std::string& message, int type,
        const std::string& file, int line);

    static void setBaseDirectory(const std::string& baseDir);

private:
    static std::unordered_map<int, Listener> listeners_;
    static std::atomic<int> nextId_;
    static std::mutex mutex_;
    static std::string baseDir_;  // Add this member variable
    static std::string makeRelativePath(const std::string& absPath);
};
