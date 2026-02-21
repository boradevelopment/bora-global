// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.

/* 
 * FileName: CommandListCategories.h
 * Purpose: A bunch of definitions for list categories, this is for specific features that use a commandlist
 * that have a special command list object to prevent naming clashing.
 */
#pragma once
/// <i>Command Categories</i>
/// Command categories are essential to preventing registry clashes between classes that share common names.
/// There are two forms of categories, one that is designated (Bora) Native and one that is designated for System Imports
/// If you want to learn more about the difference between this two, read more through the official documentation
namespace CommandCategories {
    inline constexpr const char* NativeTesting       = "bnTests";
    inline constexpr const char* NativeGraphics = "bnGraphics";
    inline constexpr const char* NativeAudio    = "bnAudio";
    inline constexpr const char* NativeCPU    = "bnCPU";
    inline constexpr const char* SysImportsGraphics    = "sysimpGraphics";
    inline constexpr const char* SysImportsAudio       = "sysimpAudio";
    inline constexpr const char* SysImportsCPU       = "sysimpCPU";
}


#pragma pack(push, 1)
struct TestCommandData
{
    int value;
};
#pragma pack(pop)
