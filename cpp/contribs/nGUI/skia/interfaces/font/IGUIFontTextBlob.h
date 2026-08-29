// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: IGUICanvas.h
 * Purpose: ?
*/
#pragma once
#include "nGUI/skia/interfaces/GUIAbstractions.h"
#include "nWindow/bnWindowAbstracts.h"

class IGUIFontTextBlob {
public:
    virtual ~IGUIFontTextBlob() = default;

    /** Returns conservative bounding box. Uses SkPaint associated with each glyph to
       determine glyph bounds, and unions all bounds. Returned bounds may be
       larger than the bounds of all glyphs in runs.

       @return  conservative bounding box
   */
    virtual const UXRect& bounds() const = 0;

    /** Returns the number of intervals that intersect bounds.
        bounds describes a pair of lines parallel to the text advance.
        The return count is zero or a multiple of two, and is at most twice the number of glyphs in
        the the blob.

        Pass nullptr for intervals to determine the size of the interval array.

        Runs within the blob that contain SkRSXform are ignored when computing intercepts.

        @param bounds     lower and upper line parallel to the advance
        @param intervals  returned intersections; may be nullptr
        @param paint      specifies stroking, SkPathEffect that affects the result; may be nullptr
        @return           number of intersections; may be zero
     */
    virtual int getIntercepts(const float bounds[2], float intervals[],
                      const GUIPaint* paint = nullptr) const = 0;

    // Native Escape Hatch (for backend-specific extensions)
    [[nodiscard]] virtual void* GetNativeHandle() const = 0;
};
