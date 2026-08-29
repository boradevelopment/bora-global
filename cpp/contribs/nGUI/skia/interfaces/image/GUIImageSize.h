// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: IGUIImageSize.h
 * Purpose: ?
*/
#pragma once
#include <cstdint>

struct GUIImageSize {
    int32_t fWidth;
    int32_t fHeight;

    static constexpr GUIImageSize Make(int32_t w, int32_t h) { return {w, h}; }

    static constexpr GUIImageSize MakeEmpty() { return {0, 0}; }

    void set(int32_t w, int32_t h) { *this = GUIImageSize{w, h}; }

    /** Returns true iff fWidth == 0 && fHeight == 0
     */
    bool isZero() const { return 0 == fWidth && 0 == fHeight; }

    /** Returns true if either width or height are <= 0 */
    bool isEmpty() const { return fWidth <= 0 || fHeight <= 0; }

    /** Set the width and height to 0 */
    void setEmpty() { fWidth = fHeight = 0; }

    constexpr int32_t width() const { return fWidth; }
    constexpr int32_t height() const { return fHeight; }

    constexpr int64_t area() const { return SkToS64(fWidth) * SkToS64(fHeight); }

    bool equals(int32_t w, int32_t h) const { return fWidth == w && fHeight == h; }
};

static inline bool operator==(const GUIImageSize& a, const GUIImageSize& b) {
    return a.fWidth == b.fWidth && a.fHeight == b.fHeight;
}

static inline bool operator!=(const GUIImageSize& a, const GUIImageSize& b) { return !(a == b); }