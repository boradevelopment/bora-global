#if defined(__cplusplus)
#pragma once
// C and C++ shared types
typedef unsigned int u32;
typedef unsigned char u8;
typedef u8 byte;
#if __linux__ || __APPLE__
typedef signed long int __int64;
#endif

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

// C++ only aliases
template<template<typename...> class C, typename... Args>
using sAny = C<Args...>;

template<typename T>
using sVec = std::vector<T>;

template<typename T, typename V>
using sMap = std::map<T, V>;

template<typename T>
using sSharedPointer = std::shared_ptr<T>;

template<typename T>
using sUniquePointer = std::unique_ptr<T>;

typedef std::string sString;
typedef sVec<u8> vec8;
typedef vec8 byteVector;

#define tUnlimited std::numeric_limits<size_t>::max()
#endif

