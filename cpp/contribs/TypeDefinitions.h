// This is used for the entirity of BORA C++ Source.
#pragma once
#if defined(__cplusplus)
#if __linux__ || __APPLE__
typedef signed long int __int64;
#endif

// C and C++ shared types
typedef unsigned int u32, *p32;
typedef unsigned char u8, *p8;
typedef unsigned long int u64, * pu64;
typedef unsigned short u16, * pu16;
typedef signed char i8, *pi8;
typedef signed short i16, *p16;
typedef signed int i32, *pi32;
typedef signed long int i64, *pi64;
typedef unsigned long long u64Pointer;
typedef long long i64Pointer;

// extras
typedef u8 byte;


#if WIN32
#define ftellout(out) _ftelli64(out);
#elif __linux__
#define ftellout(out) ftello64(out);
#elif __APPLE__
#define ftellout(out) ftello(out);
#endif

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <unordered_map>

// C++ only aliases
template<template<typename...> class C, typename... Args>
using sAny = C<Args...>;

template<typename T>
using sVec = std::vector<T>;

template<typename T, typename V>
using sMap = std::map<T, V>;

template<typename T, typename V>
using sUnorderedMap = std::unordered_map<T, V>;

template<typename T>
using sSharedPointer = std::shared_ptr<T>;

template<typename T>
using sUniquePointer = std::unique_ptr<T>;

typedef std::string sString;
typedef std::wstring sWstring;
typedef sVec<u8> vec8;
typedef vec8 byteVector;

#define tUnlimited std::numeric_limits<size_t>::max()

// Graphics Related

struct rgb;
struct rgba;

struct rgba {
    float r;
    float g;
    float b;
    float a;

    rgba& operator=(const rgb& color);
};

struct rgb {
    float r;
    float g;
    float b;

    rgb& operator=(const rgba& color); 

    bool isDefault() const {
        return r == 0 && g == 0 && b == 0;
    }
};


inline rgba& rgba::operator=(const rgb& color) {
    r = color.r;
    g = color.g;
    b = color.b;
    a = 1.0f;
    return *this;
}

inline rgb& rgb::operator=(const rgba& color) {
    r = color.r;
    g = color.g;
    b = color.b;
    return *this;
}



struct Vertex {
    float x, y, z;   // position
    float u, v;      // texture coordinates
};

// Win32 stuff
#ifdef WIN32
#include "Windows.h"
#include <string>
#include <vector>

// Fast conversion helper
inline std::wstring Utf8ToWide(const char* utf8) {
    if (!utf8 || !*utf8) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, &wstr[0], size_needed);
    // Remove the null terminator that MultiByteToWideChar adds to the length
    if (!wstr.empty() && wstr.back() == L'\0') wstr.pop_back();
    return wstr;
}
#define WIN32_FAKE_SHADOW_HEIGHT 1
#define WIN32_MAXIMIZED_RECTANGLE_OFFSET 2
#ifndef GET_X_PARAM
#define GET_X_PARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_PARAM
#define GET_Y_PARAM(lp) ((int)(short)HIWORD(lp))
#endif
typedef HWND SysHandle;
typedef HMODULE DynLibraryHandle;
#elif  defined(__APPLE__)
typedef void* SysHandle;
typedef void* DynLibraryHandle;
#elif defined(__linux__)
typedef int SysHandle;
typedef void* DynLibraryHandle;
#endif
inline sUnorderedMap<SysHandle, float> gDeltaTimes;
#endif


