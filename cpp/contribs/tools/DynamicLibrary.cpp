// Apart of the BORA Runtime Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
#include "DynamicLibrary.h"

#include "StringUtils.h"
#ifdef __linux__
#include <dlfcn.h>
#endif

DynamicLibrary::DynamicLibrary(const wchar_t* path) : libraryHandle(nullptr) {
#ifdef _WIN64
    libraryHandle = LoadLibraryW(path);
#elif defined(__linux__)
    auto string = StringUtils::wstringToUtf8(path);
    libraryHandle = dlopen(string.c_str(), RTLD_LAZY);
    if (!libraryHandle) {
        const char* error = dlerror();
        // Log or print the error
        fprintf(stderr, "Failed to load library %s: %s\n", string.c_str(), error ? error : "unknown error");
    }
#endif
}

bool DynamicLibrary::hasOpened() const {
    return libraryHandle != nullptr;
}

void* DynamicLibrary::getProcAddress(const char* procName) const {
#ifdef _WIN64
    return reinterpret_cast<void*>(GetProcAddress(libraryHandle, procName));
#elif defined(__linux__)
    return dlsym(libraryHandle, procName);
#endif
}

void DynamicLibrary::close() {
if (!libraryHandle) return;
#ifdef _WIN64
    FreeLibrary(libraryHandle);
#elif defined(__linux__)
    dlclose(libraryHandle);
#endif
    libraryHandle = nullptr;
}