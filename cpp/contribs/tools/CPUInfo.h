// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.
/* 
 * FileName: CPUInfo.h
 * Purpose: A static class for getting runtime CPU information for all OSes
 */

#pragma once
class CPUInfo
{
public:
    static const char* getArchitectureSuffix();
    static const wchar_t* getArchitectureSuffixW();
private:
#ifdef _WIN64
    static inline SYSTEM_INFO sysInfo = {0};
#endif
};
