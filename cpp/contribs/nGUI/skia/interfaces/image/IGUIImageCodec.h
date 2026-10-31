// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: IGUIImageCodec.h
 * Purpose: ?
*/
#pragma once
struct ImageDesc {
    int width = 0;
    int height = 0;
    bool hasAlpha = true;
};

class IGUIImageCodec {
public:
    virtual ~IGUIImageCodec() = default;

    // Load from memory buffer
    virtual bool LoadFromMemory(const u8* data, size_t size) = 0;

    // Metadata getters
    virtual ImageDesc GetDesc() const = 0;

    // Decode pixels into a destination buffer
    virtual bool DecodePixels(sVec<u8>& outPixels) = 0;
};