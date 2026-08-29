#include <Utilities.h>
#include "AppParam.h"


// inline std::unordered_map<std::string, ParamMeta> AppParam::params;
// inline std::unordered_map<std::string, std::string> AppParam::aliasMap;
bool AppParam::registerParam(std::string key, std::vector<std::string> aliases, std::string description) {
    ParamMeta meta;
    meta.key = key;
    meta.aliases = aliases;
    meta.description = description;

    params[key] = meta;
    if (key.length() >= 4) aliasMap["--" + key] = key;
    else aliasMap["-" + key] = key;

    for (const auto& alias : aliases) {
        aliasMap[alias] = key;
    }
    return true;
}

void AppParam::initialize(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        std::string matchedKey = "";
        std::string attachedVal = "";
        bool isAttached = false;
        bool hasEqualSign = false;

        if (arg.find("=") != std::string::npos)
        {
            arg = arg.substr(0, arg.find("=")-1);
            printf(arg.c_str());
            hasEqualSign = true;
        }

        // 1. Check exact match in alias map (e.g. "-D", "--out")
        auto itAlias = aliasMap.find(arg);
        if (itAlias != aliasMap.end()) {
            matchedKey = itAlias->second;
        } else {
            // 2. Check prefix match for attached parameters (e.g. "-DVALUE")
            for (const auto& [alias, key] : aliasMap) {
                if (alias.length() > 1 && arg.rfind(alias, 0) == 0) {
                    matchedKey = key;
                    attachedVal = arg.substr(alias.length());
                    isAttached = true;
                    break;
                }
            }
        }

        if (!matchedKey.empty()) {
            // Process named parameter (-D, --out, etc.)
            ParamMeta& meta = params[matchedKey];
            meta.isSet = true;

            std::wstring valToStore;
            if (isAttached) {
                valToStore = toWString(attachedVal);
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                valToStore = toWString(argv[++i]);
            } else if(hasEqualSign) {
                valToStore = toWString(arg.substr(arg.find("=") + 1));
                printf(wstringToUtf8(valToStore).c_str());
            }
            else {
                valToStore = L"1";
            }

            meta.values.push_back(valToStore);
            meta.value = valToStore;
        } else {
            ParamMeta& posMeta = params[""];
            posMeta.key = "";
            posMeta.isSet = true;

            std::wstring posVal = toWString(arg);
            posMeta.values.push_back(posVal);

            if (posMeta.value.empty()) {
                posMeta.value = posVal;
            }
        }
    }
}

std::wstring AppParam::get(const std::string& key) {
    auto it = params.find(key);
    if (it != params.end()) return it->second.value;
    return L"";
}

std::vector<std::wstring> AppParam::getValues(const std::string& key) {
    auto it = params.find(key);
    if (it != params.end()) return it->second.values;
    return {};
}

std::unordered_map<std::wstring, std::wstring> AppParam::getDefinitions(const std::string& key) {
    std::unordered_map<std::wstring, std::wstring> defs;
    auto rawValues = getValues(key);

    for (const auto& v : rawValues) {
        auto eqPos = v.find(L'=');
        if (eqPos != std::wstring::npos) {
            std::wstring macroName = v.substr(0, eqPos);
            std::wstring macroVal = v.substr(eqPos + 1);
            defs[macroName] = macroVal;
        } else {
            // E.g. -DFOO defaults to FOO = 1
            defs[v] = L"1";
        }
    }
    return defs;
}

bool AppParam::has(const std::string& key) {
    auto it = params.find(key);
    return it != params.end() && it->second.isSet;
}

const std::unordered_map<std::string, ParamMeta>& AppParam::getAll() {
    return params;
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

