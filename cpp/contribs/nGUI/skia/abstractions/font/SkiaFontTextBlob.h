// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: SkiaFontTextBlob.h
 * Purpose: A Text Blob for a font
*/
#pragma once
#include <include/core/SkTextBlob.h>

#include "nGUI/skia/interfaces/font/IGUIFont.h"
#include "nGUI/skia/interfaces/font/IGUIFontTextBlob.h"

class SkiaFontTextBlob : public IGUIFontTextBlob
{
public:
    const UXRect& bounds() const override
    {
        if (blob_)
        {
            auto bound = blob_->bounds();
            return { bound.left, bound.top, bound.right, bound.bottom };
        }

        return {};
    }

    int getIntercepts(const float bounds[2], float intervals[],
                    const GUIPaint* paint = nullptr) const override
    {
        if (blob_)
        {
            const auto* skPaint = reinterpret_cast<const SkPaint*>(paint);;
            return blob_->getIntercepts(bounds, intervals, skPaint);
        }

        return 0;
    }

    /** Creates SkTextBlob with a single run.

        font contains attributes used to define the run text.

        When encoding is SkTextEncoding::kUTF8, SkTextEncoding::kUTF16, or
        SkTextEncoding::kUTF32, this function uses the default
        character-to-glyph mapping from the SkTypeface in font.  It does not
        perform typeface fallback for characters not found in the SkTypeface.
        It does not perform kerning or other complex shaping; glyphs are
        positioned based on their default advances.

        @param text        character code points or glyphs drawn
        @param byteLength  byte length of text array
        @param font        text size, typeface, text scale, and so on, used to draw
        @param encoding    text encoding used in the text array
        @return            SkTextBlob constructed from one run
    */
    explicit SkiaFontTextBlob(const void* text, size_t byteLength, const IGUIFont* font,
    TextEncoding encoding = TextEncoding::kUTF8)
    {
        auto skEncoding = static_cast<SkTextEncoding>(encoding);
        blob_ = SkTextBlob::MakeFromText(text, byteLength, *((SkFont*)font->GetNativeHandle()), skEncoding);
    }

    /** Creates SkTextBlob with a single run. string meaning depends on SkTextEncoding;
        by default, string is encoded as UTF-8.

        font contains attributes used to define the run text.

        When encoding is SkTextEncoding::kUTF8, SkTextEncoding::kUTF16, or
        SkTextEncoding::kUTF32, this function uses the default
        character-to-glyph mapping from the SkTypeface in font.  It does not
        perform typeface fallback for characters not found in the SkTypeface.
        It does not perform kerning or other complex shaping; glyphs are
        positioned based on their default advances.

        @param string   character code points or glyphs drawn
        @param font     text size, typeface, text scale, and so on, used to draw
        @param encoding text encoding used in the text array
        @return         SkTextBlob constructed from one run
    */
    explicit SkiaFontTextBlob(const char* string, const IGUIFont* font,
                             TextEncoding encoding = TextEncoding::kUTF8) : SkiaFontTextBlob(string, string ? strlen(string) : 0, font, encoding)
    {
        auto skEncoding = static_cast<SkTextEncoding>(encoding);
        blob_ = SkTextBlob::MakeFromText(text, byteLength, *((SkFont*)font->GetNativeHandle()), skEncoding);
    }

    /** Returns a textblob built from a single run of text with x-positions and a single y value.
        This is equivalent to using SkTextBlobBuilder and calling allocRunPosH().
        Returns nullptr if byteLength is zero.

        @param text        character code points or glyphs drawn (based on encoding)
        @param byteLength  byte length of text array
        @param xpos    array of x-positions, must contain values for all of the character points.
        @param constY  shared y-position for each character point, to be paired with each xpos.
        @param font    SkFont used for this run
        @param encoding specifies the encoding of the text array.
        @return        new textblob or nullptr
     */
    explicit SkiaFontTextBlob(const void* text, size_t byteLength,
                              std::span<const float> xpos, float constY,
                              const IGUIFont* font,
                              TextEncoding encoding = TextEncoding::kUTF8)
    {
        auto skEncoding = static_cast<SkTextEncoding>(encoding);
        blob_ = SkTextBlob::MakeFromPosTextH(text, byteLength, SkSpan(xpos.data(), xpos.size()), constY, *((SkFont*)font->GetNativeHandle()), skEncoding);
    }

    /** Returns a textblob built from a single run of text with positions.
        This is equivalent to using SkTextBlobBuilder and calling allocRunPos().
        Returns nullptr if byteLength is zero.

        @param text        character code points or glyphs drawn (based on encoding)
        @param byteLength  byte length of text array
        @param pos     array of positions, must contain values for all of the character points.
        @param font    SkFont used for this run
        @param encoding specifies the encoding of the text array.
        @return        new textblob or nullptr
     */
    explicit SkiaFontTextBlob(const void* text, size_t byteLength,
                             std::span<const Vec2> pos, const IGUIFont* font,
                             TextEncoding encoding = TextEncoding::kUTF8)
    {
        auto skEncoding = static_cast<SkTextEncoding>(encoding);
        auto currentFont = *((SkFont*)font->GetNativeHandle());
        blob_ = SkTextBlob::MakeFromPosText(text, byteLength, SkSpan((SkPoint*)pos.data(), pos.size()), currentFont, skEncoding);
    }

    // static sk_sp<SkTextBlob> MakeFromRSXform(const void* text, size_t byteLength,
    //                                          SkSpan<const SkRSXform> xform, const SkFont& font,
    //                                          SkTextEncoding encoding = SkTextEncoding::kUTF8);
    //
    // // Helpers for glyphs
    //
    // static sk_sp<SkTextBlob> MakeFromPosHGlyphs(SkSpan<const SkGlyphID> glyphs,
    //                                             SkSpan<const SkScalar> xpos, SkScalar constY,
    //                                             const SkFont& font) {
    //     return MakeFromPosTextH(glyphs.data(), glyphs.size() * sizeof(SkGlyphID), xpos, constY,
    //                             font, SkTextEncoding::kGlyphID);
    // }
    // static sk_sp<SkTextBlob> MakeFromPosGlyphs(SkSpan<const SkGlyphID> glyphs,
    //                                            SkSpan<const SkPoint> pos, const SkFont& font) {
    //     return MakeFromPosText(glyphs.data(), glyphs.size() * sizeof(SkGlyphID), pos, font,
    //                            SkTextEncoding::kGlyphID);
    // }
    // static sk_sp<SkTextBlob> MakeFromRSXformGlyphs(SkSpan<const SkGlyphID> glyphs,
    //                                                SkSpan<const SkRSXform> xform,
    //                                                const SkFont& font) {
    //     return MakeFromRSXform(glyphs.data(), glyphs.size() * sizeof(SkGlyphID), xform, font,
    //                            SkTextEncoding::kGlyphID);
    // }

    [[nodiscard]] void* GetNativeHandle() const override {
        return blob_.get();
    }

private:
    sk_sp<SkTextBlob> blob_;

};