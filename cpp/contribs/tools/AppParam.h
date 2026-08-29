#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <regex>

#define CONCAT_HIDDEN(a, b) a ## b
#define CONCAT(a, b) CONCAT_HIDDEN(a, b)

#define REGISTER_PARAM_AD(key, aliases, description) static bool CONCAT(_reg_, __LINE__) = AppParam::registerParam(key, aliases, description);
#define REGISTER_PARAM_A(key, aliases) static bool CONCAT(_reg_, __LINE__) = AppParam::registerParam(key, aliases);
#define REGISTER_PARAM(key) static bool CONCAT(_reg_, __LINE__) = AppParam::registerParam(key);

struct ParamMeta {
    std::string key;                  // e.g. "define"
    std::vector<std::string> aliases;  // e.g. {"-D"}
    std::wstring value;               // Holds the latest value set
    std::vector<std::wstring> values; // Accumulates ALL values (e.g. for multiple -D flags)
    std::string description;
    bool isSet = false;
};

class AppParam {
public:
    static void initialize(int argc, char** argv);
    static bool registerParam(std::string key, std::vector<std::string> aliases = {}, std::string description = "");

    // Returns the latest value
    static std::wstring get(const std::string& key);

    // Returns ALL values registered for this parameter
    static std::vector<std::wstring> getValues(const std::string& key);

    // Helper specifically for -D flags: turns ["FOO", "BAR=10"] into {L"FOO": L"1", L"BAR": L"10"}
    static std::unordered_map<std::wstring, std::wstring> getDefinitions(const std::string& key);

    static const std::unordered_map<std::string, ParamMeta>& getAll();

    template<typename T>
    static T getValue(const std::string& key) {
        auto it = params.find(key);
        if (it == params.end() || it->second.values.empty()) return T();

        std::wstringstream ss(it->second.values.back());
        T result;
        ss >> std::boolalpha >> result;
        if (ss.fail()) return T();
        return result;
    }

    static bool has(const std::string& key);
    static std::vector<std::wstring> getArray(const std::string& key);

private:
    static inline std::unordered_map<std::string, ParamMeta> params;
    static inline std::unordered_map<std::string, std::string> aliasMap;

    static std::wstring toWString(const std::string& str) {
        return std::wstring(str.begin(), str.end());
    }
};