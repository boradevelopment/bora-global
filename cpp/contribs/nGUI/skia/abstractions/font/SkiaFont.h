// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: SkiaSurface.h
 * Purpose: ?
*/
#pragma once
#include <utility>
#include "nGUI/skia/abstractions/SkiaCanvas.h"
#include "nCommon/Resources.h"
#include "nGUI/skia/interfaces/GUISkiaSysInfo.h"
#include "nGUI/skia/interfaces/IGUICanvas.h"
#include "nGUI/skia/interfaces/font/IGUIFont.h"

class SkiaFont : public IGUIFont
{
public:
  explicit SkiaFont(sk_sp<SkTypeface> typeface, float size)
        : m_Font(std::make_unique<SkFont>(std::move(typeface), static_cast<SkScalar>(size)))
    {}

    explicit SkiaFont(const SkFont& font)
        : m_Font(std::make_unique<SkFont>(font))
    {}

    ~SkiaFont() override = default;

    // --- Comparison ---
    bool operator==(const IGUIFont& font) const override {
        const auto* other = dynamic_cast<const SkiaFont*>(&font);
        return other && (*m_Font == *other->m_Font);
    }

    // --- Flags / Queries ---
    [[nodiscard]] bool isForceAutoHinting() const override { return m_Font->isForceAutoHinting(); }
    [[nodiscard]] bool isEmbeddedBitmaps() const override { return m_Font->isEmbeddedBitmaps(); }
    [[nodiscard]] bool isSubpixel() const override { return m_Font->isSubpixel(); }
    [[nodiscard]] bool isLinearMetrics() const override { return m_Font->isLinearMetrics(); }
    [[nodiscard]] bool isEmbolden() const override { return m_Font->isEmbolden(); }
    [[nodiscard]] bool isBaselineSnap() const override { return m_Font->isBaselineSnap(); }

    // --- Mutators ---
    void setForceAutoHinting(bool forceAutoHinting) override { m_Font->setForceAutoHinting(forceAutoHinting); }
    void setEmbeddedBitmaps(bool embeddedBitmaps) override { m_Font->setEmbeddedBitmaps(embeddedBitmaps); }
    void setSubpixel(bool subpixel) override { m_Font->setSubpixel(subpixel); }
    void setLinearMetrics(bool linearMetrics) override { m_Font->setLinearMetrics(linearMetrics); }
    void setEmbolden(bool embolden) override { m_Font->setEmbolden(embolden); }
    void setBaselineSnap(bool baselineSnap) override { m_Font->setBaselineSnap(baselineSnap); }

    // --- Hints & Edging ---
    [[nodiscard]] Edging getEdging() const override {
        return static_cast<Edging>(m_Font->getEdging());
    }
    void setEdging(Edging edging) override {
        m_Font->setEdging(static_cast<SkFont::Edging>(edging));
    }

    void setHinting(FontHinting hintingLevel) override {
        m_Font->setHinting(static_cast<SkFontHinting>(hintingLevel));
    }
    [[nodiscard]] FontHinting getHinting() const override {
        return static_cast<FontHinting>(m_Font->getHinting());
    }

    // --- Metrics & Parameters ---
    [[nodiscard]] IGUIFontTypeface* getTypeface() const override {
        // Return raw backend typeface or bridge wrapper if applicable
        return reinterpret_cast<IGUIFontTypeface*>(m_Font->getTypeface());
    }

    [[nodiscard]] ResourceHandle<IGUIFontTypeface> refTypeface() const override {
        sk_sp<SkTypeface> tf = m_Font->refTypeface();
        return {reinterpret_cast<IGUIFontTypeface*>(tf.get())};
    }

    void setTypeface(ResourceHandle<IGUIFontTypeface> tf) override {
        auto* rawTf = reinterpret_cast<SkTypeface*>(tf.Get());
        m_Font->setTypeface(sk_ref_sp(rawTf));
    }

    [[nodiscard]] float getSize() const override { return static_cast<float>(m_Font->getSize()); }
    void setSize(float textSize) override { m_Font->setSize(static_cast<SkScalar>(textSize)); }

    [[nodiscard]] float getScaleX() const override { return static_cast<float>(m_Font->getScaleX()); }
    void setScaleX(float scaleX) override { m_Font->setScaleX(static_cast<SkScalar>(scaleX)); }

    [[nodiscard]] float getSkewX() const override { return static_cast<float>(m_Font->getSkewX()); }
    void setSkewX(float skewX) override { m_Font->setSkewX(static_cast<SkScalar>(skewX)); }

    // --- Text & Glyph Conversion ---
    size_t textToGlyphs(const void* text, size_t byteLength, TextEncoding encoding,
                        std::span<uint16_t> glyphs) const override {
        auto skEncoding = static_cast<SkTextEncoding>(encoding);
        return m_Font->textToGlyphs(text, byteLength, skEncoding,
                                     SkSpan<SkGlyphID>(glyphs.data(), glyphs.size()));
    }

    [[nodiscard]] uint16_t unicharToGlyph(int32_t uni) const override {
        return m_Font->unicharToGlyph(static_cast<SkUnichar>(uni));
    }

    void unicharsToGlyphs(std::span<const int32_t> src, std::span<uint16_t> dst) const override {
        m_Font->unicharsToGlyphs(SkSpan((int*)src.data(), src.size()),
                                 SkSpan(dst.data(), dst.size()));
    }

    // --- Measurement & Layout ---
    float measureText(const void* text, size_t byteLength, TextEncoding encoding,
                      WindowRect* bounds, const GUIPaint* paint) const override {
        auto skEncoding = static_cast<SkTextEncoding>(encoding);
        SkRect skBounds;
        const auto* skPaint = reinterpret_cast<const SkPaint*>(paint);

        SkScalar advance = m_Font->measureText(text, byteLength, skEncoding,
                                               bounds ? &skBounds : nullptr, skPaint);

        if (bounds) {
            *bounds = *reinterpret_cast<WindowRect*>(&skBounds);
        }
        return static_cast<float>(advance);
    }

    void getWidthsBounds(std::span<const uint16_t> glyphs, std::span<float> widths,
                         std::span<WindowRect> bounds, const GUIPaint* paint) const override {
        auto skGlyphs = SkSpan(glyphs.data(), glyphs.size());
        int count = static_cast<int>(glyphs.size());
        SkSpan<float> skWidths = widths.empty() ? SkSpan<float>() : SkSpan(widths.data(), widths.size());
        auto skBounds = bounds.empty()
        ? SkSpan<SkRect>()
        : SkSpan(reinterpret_cast<SkRect*>(bounds.data()), bounds.size());

        const auto* skPaint = reinterpret_cast<const SkPaint*>(paint);

        m_Font->getWidthsBounds(skGlyphs, skWidths, skBounds, skPaint);
    }

    float getMetrics(GUIFontMetrics* metrics) const override {
        auto* skMetrics = reinterpret_cast<SkFontMetrics*>(metrics);
        return static_cast<float>(m_Font->getMetrics(skMetrics));
    }

    // --- Native Escape Hatch ---
    [[nodiscard]] void* GetNativeHandle() const override {
        return m_Font.get();
    }

private:
    std::unique_ptr<SkFont> m_Font;
};
