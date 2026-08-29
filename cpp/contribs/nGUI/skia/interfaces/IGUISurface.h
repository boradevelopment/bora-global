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
    virtual ~IGUISurface() = default;

    // Resizes or reallocates the backing surface buffer
    virtual bool Resize(int width, int height) = 0;
    virtual void Reset() = 0;

    // Returns the canvas handle bound to this surface
    [[nodiscard]] virtual IGUICanvas* GetCanvas() = 0;

    // Surface metadata
    [[nodiscard]] virtual int GetWidth() const = 0;
    [[nodiscard]] virtual int GetHeight() const = 0;

    // Native handle access (e.g., returns SkSurface*)
    [[nodiscard]] virtual void* GetNativeHandle() const = 0;
};
