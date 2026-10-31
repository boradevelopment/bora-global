// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: SkiaSurface.h
 * Purpose: ?
*/
#pragma once
#include "SkiaCanvas.h"
#include "nGUI/skia/interfaces/GUISkiaSysInfo.h"
#include "nGUI/skia/interfaces/IGUICanvas.h"
#include "nGUI/skia/interfaces/IGUISurface.h"

class SkiaSurface : public IGUISurface
{
public:
    explicit SkiaSurface(const UXRect& initialRect, sk_sp<SkSurface> surface)
        : m_skSurface(std::move(surface)), m_canvasWrapper(), rect(initialRect)
    {
        if (m_skSurface)
        {
            m_canvasWrapper.SetSkCanvas(m_skSurface->getCanvas());
        }
    }

    void Reset() override
    {
        if (!m_skSurface) return;
        m_skSurface.reset();
    }

    bool Resize(int width, int height) override {
        if (!m_skSurface) return false;

        // Re-create surface with new dimensions using same image info
        SkImageInfo info = m_skSurface->imageInfo().makeWH(width, height);
        sk_sp<SkSurface> newSurface = m_skSurface->makeSurface(info);

        if (!newSurface) return false;

        m_skSurface = std::move(newSurface);
        m_canvasWrapper.SetSkCanvas(m_skSurface->getCanvas());
        return true;
    }

    bool PeekPixels(vec8& out) const override
    {
        if (!m_skSurface) {
            return false;
        }

        SkPixmap pixmap;
        if (m_skSurface->peekPixels(&pixmap)) {
            const auto* rawPixels = static_cast<const uint8_t*>(pixmap.addr());
            if (!rawPixels) {
                return false;
            }
            size_t byteSize = pixmap.computeByteSize();

            out.assign(rawPixels, rawPixels + byteSize);
            return true;
        }

        return false;
    }

    [[nodiscard]] u32 GetRowBytes() const override
    {
        if (!m_skSurface) return 0;

        SkPixmap pixmap;
        if (m_skSurface->peekPixels(&pixmap))
        {
            return pixmap.rowBytes();
        }

        return 0;
    }

    [[nodiscard]] const u8* GetRawPixels() const override
    {
        if (!m_skSurface) return nullptr;

        SkImageInfo info = m_skSurface->imageInfo();
        size_t rowBytes = info.minRowBytes();


        m_pixelCache.resize(info.height() * rowBytes);
        if (m_skSurface->readPixels(info, m_pixelCache.data(), rowBytes, 0, 0))
        {
            return m_pixelCache.data();
        }

        return nullptr;
    }

    [[nodiscard]] size_t GetTotalByteSize() const override
    {
        if (!m_skSurface) return 0;

        SkPixmap pixmap;
        if (m_skSurface->peekPixels(&pixmap))
        {
            return pixmap.computeByteSize();
        }

        return 0;
    }

    IGUICanvas* GetCanvas() override {
        return m_skSurface ? &m_canvasWrapper : nullptr;
    }

    [[nodiscard]] int GetWidth() const override {
        return m_skSurface ? m_skSurface->width() : 0;
    }

    [[nodiscard]] int GetHeight() const override {
        return m_skSurface ? m_skSurface->height() : 0;
    }

    [[nodiscard]] UXRect GetRect() const override
    {
        return rect;
    }

    void SetRect(const UXRect& newRect) override {
        rect = newRect;
        auto canvas = GetCanvas();
        canvas->Save();
        canvas->Translate(static_cast<float>(rect.left), static_cast<float>(rect.top));
        canvas->ClipRect(rect);
    }

    [[nodiscard]] void* GetNativeHandle() const override {
        return static_cast<void*>(m_skSurface.get());
    }

private:
    sk_sp<SkSurface> m_skSurface;
    SkiaCanvas m_canvasWrapper;
    UXRect rect;
    mutable std::vector<u8> m_pixelCache;
};
