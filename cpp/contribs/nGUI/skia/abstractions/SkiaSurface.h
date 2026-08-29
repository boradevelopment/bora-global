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
    explicit SkiaSurface(sk_sp<SkSurface> surface)
        : m_skSurface(std::move(surface)), m_canvasWrapper()
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

    IGUICanvas* GetCanvas() override {
        return m_skSurface ? &m_canvasWrapper : nullptr;
    }

    [[nodiscard]] int GetWidth() const override {
        return m_skSurface ? m_skSurface->width() : 0;
    }

    [[nodiscard]] int GetHeight() const override {
        return m_skSurface ? m_skSurface->height() : 0;
    }

    [[nodiscard]] void* GetNativeHandle() const override {
        return static_cast<void*>(m_skSurface.get());
    }

private:
    sk_sp<SkSurface> m_skSurface;
    SkiaCanvas m_canvasWrapper;
};
