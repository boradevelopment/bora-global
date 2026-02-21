// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.

/* ? [Common or PDS (Platform Dependent Source)]

 * FileName: Env.h
 * Title: Environment
 * Author: ?
 * Purpose: ?

 * Compatibility: ?

 * Updates - ?
 * Known issues - ?
 */
#ifndef BORAC_ENV_H
#define BORAC_ENV_H
#if WIN32

#include <windows.h>
#include <string>
#include <iostream>

class Environment {
public:
    // Get user environment variable by name
    static std::wstring getUserEnvVar(const std::wstring& name) {
        HKEY hKey;
        std::wstring value;

        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Environment", 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            std::wcerr << L"Failed to open Environment key\n";
            return L"";
        }

        DWORD size = 0;
        DWORD type = 0;
        LONG ret = RegQueryValueExW(hKey, name.c_str(), nullptr, &type, nullptr, &size);
        if (ret != ERROR_SUCCESS || (type != REG_SZ && type != REG_EXPAND_SZ)) {
            RegCloseKey(hKey);
            return L"";
        }

        value.resize(size / sizeof(wchar_t));
        ret = RegQueryValueExW(hKey, name.c_str(), nullptr, nullptr, reinterpret_cast<BYTE*>(&value[0]), &size);
        RegCloseKey(hKey);
        if (ret != ERROR_SUCCESS) {
            return L"";
        }

        // Remove trailing nulls
        value.resize(wcsnlen(value.c_str(), value.size()));
        return value;
    }


     static std::string getEnvVar(const std::string& name) {
        DWORD bufferSize = GetEnvironmentVariableA(name.c_str(), NULL, 0);
        if (bufferSize == 0) {
            // Variable not found or empty
            return "";
        }

        std::string value(bufferSize, '\0');
        GetEnvironmentVariableA(name.c_str(), &value[0], bufferSize);

        // Remove null terminator that was included
        value.pop_back();
        return value;
    }

    // Set user environment variable by name (REG_EXPAND_SZ for PATH compatibility)
    static bool setUserEnvVar(const std::wstring& name, const std::wstring& value) {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Environment", 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) {
            std::wcerr << L"Failed to open Environment key for writing\n";
            return false;
        }

        LONG ret = RegSetValueExW(hKey, name.c_str(), 0, REG_EXPAND_SZ,
                                  reinterpret_cast<const BYTE*>(value.c_str()),
                                  static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t)));

        RegCloseKey(hKey);

        if (ret != ERROR_SUCCESS) {
            std::wcerr << L"Failed to set environment variable\n";
            return false;
        }

        // Notify the system so new processes get the updated env
        SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                            (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 5000, nullptr);
        return true;
    }

    // Append a value to the user environment variable (like PATH), avoiding duplicates
    static bool appendToUserEnvVar(const std::wstring& name, const std::wstring& appendValue, wchar_t delimiter = L';') {
        std::wstring currentValue = getUserEnvVar(name);
        if (!currentValue.empty()) {
            // Check if appendValue already present (naive substring search)
            if (currentValue.find(appendValue) != std::wstring::npos) {
                std::wcout << L"Value already present in " << name << L"\n";
                return true;
            }

            if (currentValue.back() != delimiter) {
                currentValue += delimiter;
            }
            currentValue += appendValue;
        } else {
            currentValue = appendValue;
        }

        return setUserEnvVar(name, currentValue);
    }
};
#elif __linux__ || __APPLE__
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Environment {
public:
    // Get environment variable (current process environment)
    static std::string getEnvVar(const std::string& name) {
        const char* val = std::getenv(name.c_str());
        return val ? std::string(val) : "";
    }

    // Set environment variable (only for current process)
    static bool setEnvVar(const std::string& name, const std::string& value) {
        return setenv(name.c_str(), value.c_str(), 1) == 0;
    }

    // Append to environment variable (example, PATH) for current process
    static bool appendToEnvVar(const std::string& name, const std::string& appendValue, char delimiter = ':') {
        std::string current = getEnvVar(name);
        if (!current.empty()) {
            // Avoid duplicates
            if (current.find(appendValue) != std::string::npos) {
                std::cout << "Value already present in " << name << "\n";
                return true;
            }
            if (current.back() != delimiter) {
                current += delimiter;
            }
            current += appendValue;
        } else {
            current = appendValue;
        }

        return setEnvVar(name, current);
    }

    // Persistent setting (e.g., ~/.bashrc) - This modifies user shell config
    static bool persistEnvVar(const std::string& name, const std::string& value, const std::string& shellFile = "~/.bashrc") {
        std::string expandedShellFile = expandTilde(shellFile);
        std::ofstream out(expandedShellFile, std::ios::app);
        if (!out) {
            std::cerr << "Failed to open shell config for writing: " << expandedShellFile << "\n";
            return false;
        }

        out << "\nexport " << name << "=\"" << value << "\"\n";
        out.close();
        return true;
    }

private:
    // Utility to expand ~ to home directory
    static std::string expandTilde(const std::string& path) {
        if (path[0] != '~') return path;

        const char* home = std::getenv("HOME");
        if (!home) return path;

        return std::string(home) + path.substr(1);
    }
};
#endif
#endif //BORAC_ENV_H