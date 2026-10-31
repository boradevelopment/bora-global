// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: IGUISurface.h
 * Purpose: Abstracts the surface for UX
*/
#pragma once
#include "IGUICanvas.h"

class IGUISurface {
public:
    IGUISurface() = default;
    virtual ~IGUISurface() = default;

    // Resizes or reallocates the backing surface buffer
    virtual bool Resize(int width, int height) = 0;
    virtual void Reset() = 0;

    // Returns the canvas handle bound to this surface
    [[nodiscard]] virtual IGUICanvas* GetCanvas() = 0;
    virtual bool PeekPixels(vec8& out) const = 0;
    virtual const u8* GetRawPixels() const = 0;
    virtual u32 GetRowBytes() const = 0;
    virtual size_t GetTotalByteSize() const = 0;
    // Surface metadata
    [[nodiscard]] virtual int GetWidth() const = 0;
    [[nodiscard]] virtual int GetHeight() const = 0;
    [[nodiscard]] virtual UXRect GetRect() const = 0;
    virtual void SetRect(const UXRect& newRect) = 0;

    // Native handle access (e.g., returns SkSurface*)
    [[nodiscard]] virtual void* GetNativeHandle() const = 0;
};
