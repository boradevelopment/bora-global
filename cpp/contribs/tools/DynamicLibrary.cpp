// Apart of the BORA Runtime Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
#include "DynamicLibrary.h"

DynamicLibrary::DynamicLibrary(const wchar_t* path) : libraryHandle(nullptr) {
#ifdef WIN32
    libraryHandle = LoadLibraryW(path);
#elif defined(__linux__)
    libraryHandle = dlopen(path, RTLD_LAZY);
#endif
}

bool DynamicLibrary::hasOpened() const {
    return libraryHandle != nullptr;
}

void* DynamicLibrary::getProcAddress(const char* procName) const {
#ifdef WIN32
    return reinterpret_cast<void*>(GetProcAddress(libraryHandle, procName));
#elif defined(__linux__)
    return dlsym(libraryHandle, procName);
#endif
}

void DynamicLibrary::close() {
#ifdef WIN32
    FreeLibrary(libraryHandle);
#elif defined(__linux__)
    dlclose(libraryHandle);
#endif
    libraryHandle = nullptr;
}