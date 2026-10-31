// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: StringUtils.h
 * Purpose: ?
*/
#pragma once
#include <string>

class StringUtils {
public:
    // Delete constructors to prevent instantiation (pure static helper class)
    StringUtils() = delete;
    ~StringUtils() = delete;
    StringUtils(const StringUtils&) = delete;
    StringUtils& operator=(const StringUtils&) = delete;

    // Static helper methods
    static std::string wstringToUtf8(const std::wstring& wstr);
    static std::string wcharToUtf8(const wchar_t* src);
    static std::wstring utf8ToWstring(const std::string& str);
};