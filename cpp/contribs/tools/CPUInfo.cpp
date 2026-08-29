// Apart of the BORA Runtime Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
#include "CPUInfo.h"
#ifdef _WIN64
#include <sysinfoapi.h>
#endif

const char* CPUInfo::getArchitectureSuffix()
{
#ifdef _WIN64
    if (sysInfo.wProcessorArchitecture == 0)
    {
        GetNativeSystemInfo(&sysInfo);
    }

    switch (sysInfo.wProcessorArchitecture)
    {
    case PROCESSOR_ARCHITECTURE_AMD64:
        return "x64";
    case PROCESSOR_ARCHITECTURE_ARM64:
        return "arm64";
    default:
        return ""; // unknown
    }
#endif
}

const wchar_t* CPUInfo::getArchitectureSuffixW()
{
#ifdef _WIN64
    if (sysInfo.wProcessorArchitecture == 0)
    {
        GetNativeSystemInfo(&sysInfo);
    }

    switch (sysInfo.wProcessorArchitecture)
    {
    case PROCESSOR_ARCHITECTURE_AMD64:
        return L"x64";
    case PROCESSOR_ARCHITECTURE_ARM64:
        return L"arm64";
    default:
        return L""; // unknown
    }
#endif
}
