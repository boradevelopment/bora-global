// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
#include "StringUtils.h"

std::string StringUtils::wstringToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";

    // Determine required buffer size
    std::mbstate_t state = {};
    const wchar_t* src = wstr.c_str();
    size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
    if (len == static_cast<size_t>(-1)) return "";

    std::string result(len, '\0');
    src = wstr.c_str();
    std::wcsrtombs(&result[0], &src, result.size(), &state);
    return result;
}

std::string StringUtils::wcharToUtf8(const wchar_t* src) {
    if (!src) return "";
    return wstringToUtf8(src);
}

std::wstring StringUtils::utf8ToWstring(const std::string& str) {
    if (str.empty()) return L"";

    std::mbstate_t state = {};
    const char* src = str.c_str();
    size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
    if (len == static_cast<size_t>(-1)) return L"";

    std::wstring result(len, L'\0');
    src = str.c_str();
    std::mbsrtowcs(&result[0], &src, result.size(), &state);
    return result;
}