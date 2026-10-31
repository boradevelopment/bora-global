// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: GUIImageInfo.h
 * Purpose: ?
*/
#pragma once
#include "GUIImageAbstractions.h"

enum class GUIColorFormat {
    RGBA8,
    BGRA8,
    N32 // Platform-native 32-bit
};

struct GUIImageInfo {
    int width = 0;
    int height = 0;
    GUIColorFormat format = GUIColorFormat::N32;
    GUIImageAlphaType alphaType = GUIImageAlphaType::kPremul_SkAlphaType;

    [[nodiscard]] size_t GetMinRowBytes() const {
        return static_cast<size_t>(width) * 4; // Assuming 4 bytes per pixel for 32-bit formats
    }
};