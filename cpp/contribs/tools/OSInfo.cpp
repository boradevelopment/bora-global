// Apart of the BORA Runtime Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.

#include "OSInfo.h"

const char* OSInfo::GetOSName()
{
#ifdef _WIN32
    return "windows";
#elif defined(__APPLE__)
    return "macos";
#elif defined(__LINUX__)
    return "linux"
#endif
}
