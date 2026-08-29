// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: IGUISize.h
 * Purpose: ?
*/
#pragma once

struct IGUISize
{
    float fWidth;
    float fHeight;

    static constexpr IGUISize Make(float w, float h) { return {w, h}; }

    static constexpr IGUISize MakeEmpty() { return {0, 0}; }

    void set(float w, float h) { *this = IGUISize{w, h}; }

    /** Returns true iff fWidth == 0 && fHeight == 0
     */
    bool isZero() const { return 0 == fWidth && 0 == fHeight; }

    /** Returns true if either width or height are <= 0 */
    bool isEmpty() const { return fWidth <= 0 || fHeight <= 0; }

    /** Set the width and height to 0 */
    void setEmpty() { *this = IGUISize{0, 0}; }

    float width() const { return fWidth; }
    float height() const { return fHeight; }

    bool equals(float w, float h) const { return fWidth == w && fHeight == h; }
};