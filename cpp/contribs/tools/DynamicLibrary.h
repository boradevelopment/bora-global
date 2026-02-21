// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.

/* Common

 * FileName: DynamicLibraryLoader.h
 * Title: ?
 * Purpose: ?
 */
#pragma once

class DynamicLibrary {
public:
   explicit DynamicLibrary(const wchar_t* path);
   bool hasOpened() const;
   void* getProcAddress(const char* procName) const;
   void close();
private:
 DynLibraryHandle libraryHandle;
};

