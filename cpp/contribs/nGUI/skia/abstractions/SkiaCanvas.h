// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: SkiaCanvas.h
 * Purpose: ?
*/
#pragma once
#include <include/core/SkCanvas.h>
#include "nGUI/skia/interfaces/IGUICanvas.h"
#include <include/core/SkFont.h>
#include <include/core/SkRRect.h>
#include <include/core/SkTextBlob.h>

class SkiaCanvas : public IGUICanvas
{
public:
    explicit SkiaCanvas(SkCanvas* canvas = nullptr) : m_skCanvas(canvas) {}

    void SetSkCanvas(SkCanvas* canvas) { m_skCanvas = canvas; }

    // --- State Management ---
    void Save() override {
        if (m_skCanvas) m_skCanvas->save();
    }

    void Restore() override {
        if (m_skCanvas) m_skCanvas->restore();
    }

    void Translate(float dx, float dy) override {
        if (m_skCanvas) m_skCanvas->translate(dx, dy);
    }

    void Rotate(float degrees) override {
        if (m_skCanvas) m_skCanvas->rotate(degrees);
    }

    void Scale(float sx, float sy) override {
        if (m_skCanvas) m_skCanvas->scale(sx, sy);
    }

    // --- Primitives ---
    void Clear(const Color4f& color) override {
        if (m_skCanvas) {
            m_skCanvas->clear(SkColor4f{color.red, color.green,  color.blue, color.alpha});
        }
    }

    void DrawRect(const UXRect& rect, const GUIPaint& paint) override {
        if (!m_skCanvas) return;
        SkPaint skPaint = ToSkPaint(paint);
        m_skCanvas->drawRect(SkRect::MakeLTRB(rect.left, rect.top, rect.right, rect.bottom), skPaint);
    }

    void DrawRoundRect(const UXRect& rect, float rx, float ry, const GUIPaint& paint) override {
        if (!m_skCanvas) return;
        SkPaint skPaint = ToSkPaint(paint);
        m_skCanvas->drawRRect(
            SkRRect::MakeRectXY(SkRect::MakeLTRB(rect.left, rect.top, rect.right, rect.bottom), rx, ry),
            skPaint
        );
    }

    void DrawCircle(Vec2 center, float radius, const GUIPaint& paint) override {
        if (!m_skCanvas) return;
        m_skCanvas->drawCircle(center.x, center.y, radius, ToSkPaint(paint));
    }

    void DrawLine(Vec2 p0, Vec2 p1, const GUIPaint& paint) override {
        if (!m_skCanvas) return;
        m_skCanvas->drawLine(p0.x, p0.y, p1.x, p1.y, ToSkPaint(paint));
    }

    void DrawTextBlob(ResourceHandle<IGUIFontTextBlob> blob, Vec2 point, const GUIPaint& paint) override
    {
        if (!m_skCanvas) return;
        m_skCanvas->drawTextBlob((SkTextBlob*)blob->GetNativeHandle(), point.x, point.y, ToSkPaint(paint));
    }

    void DrawTextString(std::string_view text, Vec2 pos, float fontSize, const GUIPaint& paint) override {
        if (!m_skCanvas) return;
        SkFont font(nullptr, fontSize);
        m_skCanvas->drawSimpleText(text.data(), text.size(), SkTextEncoding::kUTF8, pos.x, pos.y, font, ToSkPaint(paint));
    }

    void ClipRect(const UXRect& rect, bool antiAlias) override {
        if (m_skCanvas) {
            m_skCanvas->clipRect(SkRect::MakeLTRB(rect.left, rect.top, rect.right, rect.bottom), antiAlias);
        }
    }

    [[nodiscard]] void* GetNativeHandle() const override {
        return static_cast<void*>(m_skCanvas);
    }

private:
    static SkPaint ToSkPaint(const GUIPaint& paint) {
        SkPaint skPaint;
        skPaint.setColor4f(SkColor4f{paint.color.red, paint.color.green, paint.color.blue, paint.color.alpha});
        skPaint.setStyle(paint.isStroke ? SkPaint::kStroke_Style : SkPaint::kFill_Style);
        skPaint.setStrokeWidth(paint.strokeWidth);
        skPaint.setAntiAlias(paint.isAntiAlias);
        return skPaint;
    }

    SkCanvas* m_skCanvas{nullptr};
};
