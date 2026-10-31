// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: DynamicLibraryLoader.h
 * Purpose: ?
 */
#pragma once

class DynamicLibrary {
public:
   explicit DynamicLibrary(const wchar_t* path);
   bool hasOpened() const;
   void* getProcAddress(const char* procName) const;
   void close();

    ~DynamicLibrary() {
        close();
    }

    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    DynamicLibrary(DynamicLibrary&& other) noexcept : libraryHandle(other.libraryHandle) {
        other.libraryHandle = nullptr;
    }
    DynamicLibrary& operator=(DynamicLibrary&& other) noexcept {
        if (this != &other) {
            close();
            libraryHandle = other.libraryHandle;
            other.libraryHandle = nullptr;
        }
        return *this;
    }
private:
 DynLibraryHandle libraryHandle;
};

