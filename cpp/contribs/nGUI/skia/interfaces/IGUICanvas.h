// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: IGUICanvas.h
 * Purpose: ?
*/
#pragma once
#include "GUIAbstractions.h"
#include "font/IGUIFontTextBlob.h"
#include "nCommon/Resources.h"
#include "nWindow/bnWindowAbstracts.h"

class IGUICanvas {
public:
    virtual ~IGUICanvas() = default;

    // Transform & State Management
    virtual void Save() = 0;
    virtual void Restore() = 0;
    virtual void Translate(float dx, float dy) = 0;
    virtual void Rotate(float degrees) = 0;
    virtual void Scale(float sx, float sy) = 0;

    // Basic Primitives
    virtual void Clear(const Color4f& color) = 0;
    virtual void DrawRect(const UXRect& rect, const GUIPaint& paint) = 0;
    virtual void DrawRoundRect(const UXRect& rect, float rx, float ry, const GUIPaint& paint) = 0;
    virtual void DrawCircle(Vec2 center, float radius, const GUIPaint& paint) = 0;
    virtual void DrawLine(Vec2 p0, Vec2 p1, const GUIPaint& paint) = 0;

    // Text & Utilities
    virtual void DrawTextString(std::string_view text, Vec2 pos, float fontSize, const GUIPaint& paint) = 0;
    virtual void DrawTextBlob(ResourceHandle<IGUIFontTextBlob> blob, Vec2 point, const GUIPaint& paint) = 0;
    virtual void ClipRect(const UXRect& rect, bool antiAlias = false) = 0;

    // Native Escape Hatch (for backend-specific extensions)
    [[nodiscard]] virtual void* GetNativeHandle() const = 0;
};
