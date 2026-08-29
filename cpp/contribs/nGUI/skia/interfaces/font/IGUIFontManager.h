// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: IGUIFontManager.h
 * Purpose: Manages fonts throughout the GUI
*/
#pragma once
#include "nGUI/skia/interfaces/GUIAbstractions.h"
#include "IGUIFont.h"
#include "IGUIFontTextBlob.h"
#include "TypeDefinitions.h"
#include "nWindow/bnWindowAbstracts.h"

class IGUIFont;

class IGUIFontManager {
public:
    virtual ~IGUIFontManager() = default;

    virtual ResourceHandle<IGUIFont> getFont(const char* family,
               int size,
               GUIFontStyle style) = 0;

    virtual ResourceHandle<IGUIFont> getFont(u8* data,
           u64 size,
           GUIFontStyle style) = 0;

    virtual ResourceHandle<IGUIFontTypeface> getFontTypeface(const char* family,
            int size,
            GUIFontStyle style) = 0;

    virtual ResourceHandle<IGUIFontTypeface> getFontTypeface(u8* data,
           u64 size,
           GUIFontStyle style) = 0;


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
    virtual ResourceHandle<IGUIFontTextBlob> createTextBlob(const void* text, size_t byteLength, const IGUIFont* font, TextEncoding encoding = TextEncoding::kUTF8) = 0;

    /** Creates a TextBlob with a single run. string meaning depends on SkTextEncoding;
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
    virtual ResourceHandle<IGUIFontTextBlob> createTextBlob(const char* string, const IGUIFont* font, TextEncoding encoding = TextEncoding::kUTF8) = 0;

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
    virtual ResourceHandle<IGUIFontTextBlob> createTextBlobPositionH(const void* text, size_t byteLength,
                              std::span<const float> xpos, float constY,
                              const IGUIFont& font,
                              TextEncoding encoding = TextEncoding::kUTF8) = 0;

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
    virtual ResourceHandle<IGUIFontTextBlob> createTextBlobPosition(const void* text, size_t byteLength,
                                         std::span<const WindowPoint> pos, const IGUIFont* font,
                                         TextEncoding encoding = TextEncoding::kUTF8) = 0;

    // Native Escape Hatch (for backend-specific extensions)
    [[nodiscard]] virtual void* GetNativeHandle() const = 0;
};
