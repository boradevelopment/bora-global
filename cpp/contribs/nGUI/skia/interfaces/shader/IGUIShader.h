// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: GUIShader.h
 * Purpose: ?
*/
#pragma once

class IGUIShader
{
public:
    /**
     *  Returns true if the shader is guaranteed to produce only opaque
     *  colors, subject to the SkPaint using the shader to apply an opaque
     *  alpha value. Subclasses should override this to allow some
     *  optimizations.
     */
    virtual bool isOpaque() const { return false; }

    /**
     *  Iff this shader is backed by a single SkImage, return its ptr (the caller must ref this
     *  if they want to keep it longer than the lifetime of the shader). If not, return nullptr.
     */
    IGUIShader* isAImage(GUIMatrix* localMatrix, SkTileMode xy[2]) const;

    bool isAImage() const {
        return this->isAImage(nullptr, (SkTileMode*)nullptr) != nullptr;
    }
}