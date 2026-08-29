// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: IGUIFont.h
 * Purpose: A single font instance
*/
#pragma once
#include <span>
#include <cstdint>
#include <cstddef>
#include "nWindow/bnWindowAbstracts.h"

class IGUIFontTypeface;
struct GUIPaint;
struct GUIFontMetrics;

template <typename T>
class ResourceHandle;

enum class TextEncoding {
    kUTF8,
    kUTF16,
    kUTF32,
    kGlyphID
};

enum class FontHinting {
    kNone,
    kSlight,
    kNormal,
    kFull
};

class IGUIFont {
public:
    virtual ~IGUIFont() = default;

    enum class Edging {
        Alias,              //!< no transparent pixels on glyph edges
        AntiAlias,          //!< may have transparent pixels on glyph edges
        SubpixelAntiAlias,  //!< glyph positioned in pixel using transparency
    };

    virtual bool operator==(const IGUIFont& font) const = 0;
    virtual bool operator!=(const IGUIFont& font) const { return !(*this == font); }

    // Flags / Queries
    [[nodiscard]] virtual bool isForceAutoHinting() const = 0;
    [[nodiscard]] virtual bool isEmbeddedBitmaps() const = 0;
    [[nodiscard]] virtual bool isSubpixel() const = 0;
    [[nodiscard]] virtual bool isLinearMetrics() const = 0;
    [[nodiscard]] virtual bool isEmbolden() const = 0;
    [[nodiscard]] virtual bool isBaselineSnap() const = 0;

    // Mutators
    virtual void setForceAutoHinting(bool forceAutoHinting) = 0;
    virtual void setEmbeddedBitmaps(bool embeddedBitmaps) = 0;
    virtual void setSubpixel(bool subpixel) = 0;
    virtual void setLinearMetrics(bool linearMetrics) = 0;
    virtual void setEmbolden(bool embolden) = 0;
    virtual void setBaselineSnap(bool baselineSnap) = 0;

    // Hints & Edging
    [[nodiscard]] virtual Edging getEdging() const = 0;
    virtual void setEdging(Edging edging) = 0;
    virtual void setHinting(FontHinting hintingLevel) = 0;
    [[nodiscard]] virtual FontHinting getHinting() const = 0;

    // Metrics & Parameters
    [[nodiscard]] virtual IGUIFontTypeface* getTypeface() const = 0;
    [[nodiscard]] virtual ResourceHandle<IGUIFontTypeface> refTypeface() const = 0;
    virtual void setTypeface(ResourceHandle<IGUIFontTypeface> tf) = 0;

    [[nodiscard]] virtual float getSize() const = 0;
    virtual void setSize(float textSize) = 0;

    [[nodiscard]] virtual float getScaleX() const = 0;
    virtual void setScaleX(float scaleX) = 0;

    [[nodiscard]] virtual float getSkewX() const = 0;
    virtual void setSkewX(float skewX) = 0;

    // Text & Glyph Conversion
    virtual size_t textToGlyphs(const void* text, size_t byteLength, TextEncoding encoding,
                                std::span<uint16_t> glyphs) const = 0;

    virtual uint16_t unicharToGlyph(int32_t uni) const = 0;
    virtual void unicharsToGlyphs(std::span<const int32_t> src, std::span<uint16_t> dst) const = 0;

    virtual size_t countText(const void* text, size_t byteLength, TextEncoding encoding) const {
        return this->textToGlyphs(text, byteLength, encoding, {});
    }

    // Measurement & Layout
    virtual float measureText(const void* text, size_t byteLength, TextEncoding encoding,
                              WindowRect* bounds = nullptr) const {
        return this->measureText(text, byteLength, encoding, bounds, nullptr);
    }

    virtual float measureText(const void* text, size_t byteLength, TextEncoding encoding,
                              WindowRect* bounds, const GUIPaint* paint) const = 0;

    virtual void getWidthsBounds(std::span<const uint16_t> glyphs, std::span<float> widths, 
                                 std::span<WindowRect> bounds, const GUIPaint* paint) const = 0;

    virtual void getWidths(std::span<const uint16_t> glyphs, std::span<float> widths) const {
        this->getWidthsBounds(glyphs, widths, {}, nullptr);
    }
    
    [[nodiscard]] virtual float getWidth(uint16_t glyph) const {
        float width = 0.0f;
        this->getWidthsBounds({&glyph, 1}, {&width, 1}, {}, nullptr);
        return width;
    }

    virtual void getBounds(std::span<const uint16_t> glyphs, std::span<WindowRect> bounds,
                           const GUIPaint* paint) const {
        this->getWidthsBounds(glyphs, {}, bounds, paint);
    }

    virtual WindowRect getBounds(uint16_t glyph, const GUIPaint* paint) const {
        WindowRect bounds{};
        this->getBounds({&glyph, 1}, {&bounds, 1}, paint);
        return bounds;
    }

    virtual float getMetrics(GUIFontMetrics* metrics) const = 0;
    [[nodiscard]] virtual float getSpacing() const { return this->getMetrics(nullptr); }

    // Native Escape Hatch
    [[nodiscard]] virtual void* GetNativeHandle() const = 0;
};