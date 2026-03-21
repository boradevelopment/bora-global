#include <Utilities.h>
#include "AppParam.h"


std::unordered_map<std::string, ParamMeta> AppParam::params;
std::unordered_map<std::string, std::string> AppParam::aliasMap;

void AppParam::registerParam(std::string key, std::vector<std::string> aliases, std::string description) {
    ParamMeta meta;
    meta.key = key;
    meta.aliases = aliases;
    meta.description = description;

    params[key] = meta;

    for (const auto& alias : aliases) {
        aliasMap[alias] = key;
    }

    // Also register the full key name with -- prefix
    if(key.length() > 4) aliasMap["--" + key] = key;
    else aliasMap["-" + key] = key;
}


void AppParam::initialize(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::wstring arg = utf8ToWstring(argv[i]);

        size_t equalPos = arg.find('=');
        std::string rawKey = (equalPos != std::string::npos) ? wstringToUtf8(arg.substr(0, equalPos)) : wstringToUtf8(arg);

        auto keyIt = aliasMap.find(rawKey);
        if (keyIt != aliasMap.end()) {
            std::string key = keyIt->second;
            std::wstring value;

            if (equalPos != std::string::npos) {
                value = arg.substr(equalPos + 1);
            } else if ((i + 1) < argc && argv[i + 1][0] != '-') {
                value = utf8ToWstring(argv[++i]); // take next argument as value
            } else {
                value = L"true"; // treat as flag
            }

            params[key].value = value;
            params[key].isSet = true;
        } else {
            // If it looks like a param (--something) but isn't registered
            if (!arg.empty() && arg[0] == '-') {
                if(arg.length() > 4) aliasMap["--" + wstringToUtf8(arg)] = wstringToUtf8(arg);
                aliasMap["-" + wstringToUtf8(arg)] = wstringToUtf8(arg);
                --i;
            } else {
                // Positional argument
                params[""].value = arg;
            }
        }
    }

}

std::wstring AppParam::get(const std::string& key) {
    auto it = params.find(key);
    if (it != params.end()) {
        return it->second.value;
    }
    return L"";
}

 const std::unordered_map<std::string, ParamMeta>& AppParam::getAll() {
    return params;
}

bool AppParam::has(const std::string& key) {
    auto it = params.find(key);
    return it != params.end() && it->second.isSet;
}


std::vector<std::wstring> AppParam::getArray(const std::string& key) {
    auto it = params.find(key);
    if (it == params.end()) return {};

    std::wstring raw = it->second.value;

    if (raw.empty()) return {};

    // Remove outer brackets
    if (raw.front() == '[' && raw.back() == ']')
        raw = raw.substr(1, raw.size() - 2);

    std::vector<std::wstring> result;

    std::wregex re(LR"((\".*?\"|\S+))"); // Match quoted strings or non-whitespace tokens
    auto begin = std::wsregex_iterator(raw.begin(), raw.end(), re);
    auto end = std::wsregex_iterator();

    for (auto i = begin; i != end; ++i) {
        std::wstring token = (*i)[1].str();

        // Remove quotes if present
        if (!token.empty() && token.front() == L'"' && token.back() == L'"') {
            token = token.substr(1, token.size() - 2);
        }

        result.push_back(token);
    }

    return result;
}

