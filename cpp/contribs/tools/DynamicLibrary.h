// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.

/* ? [Common or PDS (Platform Dependent Source)]

 * FileName: DynamicLibraryLoader.h
 * Title: ?
 * Author: ?
 * Purpose: ?

 * Compatibility: ?

 * Updates - ?
 * Known issues - ?
 */
#pragma once

class DynamicLibrary {
public:
   DynamicLibrary(const char* path);
   bool hasOpened();
   void* getProcAddress(const char* procName);
   void close();
private:
 int status = -1;
 DynLibraryHandle libraryHandle;
};

