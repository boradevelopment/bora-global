// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: SkiaImageCodec.h
 * Purpose: ?
*/
#pragma once
#include <include/codec/SkCodec.h>
#include "nGUI/skia/interfaces/image/IGUIImageCodec.h"

class SkiaImageCodec : public IGUIImageCodec {
public:
    bool LoadFromMemory(const u8* data, size_t size) override {
        if (!data || size == 0) return false;

        m_codec = SkCodec::MakeFromData(SkData::MakeWithoutCopy(data, size));
        return m_codec != nullptr;
    }

    ImageDesc GetDesc() const override {
        if (!m_codec) return {};
        auto dim = m_codec->dimensions();
        return ImageDesc{
            dim.width(),
            dim.height(),
            m_codec->getInfo().alphaType() != kOpaque_SkAlphaType
        };
    }

    bool DecodePixels(sVec<u8>& outPixels) override {
        if (!m_codec) return false;

        SkImageInfo info = SkImageInfo::MakeN32Premul(
            m_codec->dimensions().width(),
            m_codec->dimensions().height(),
            SkColorSpace::MakeSRGB()
        );

        outPixels.resize(info.minRowBytes() * info.height());

        if (m_codec->getPixels(info, outPixels.data(), info.minRowBytes()) != SkCodec::kSuccess) {
            return false;
        }

        // Optional custom alpha/color channel adjustments if needed
        ProcessPixelAlpha(outPixels, info);

        return true;
    }

    // sk_sp<SkImage> CreateSkImage() override {
    //     sVec<u8> pixels;
    //     if (!DecodePixels(pixels)) return nullptr;
    //
    //     auto dim = m_codec->dimensions();
    //     SkImageInfo info = SkImageInfo::MakeN32Premul(dim.width(), dim.height(), SkColorSpace::MakeSRGB());
    //
    //     SkPixmap pixmap(info, pixels.data(), info.minRowBytes());
    //     return SkImages::RasterFromPixmapCopy(pixmap);
    // }

private:
    // void ProcessPixelAlpha(sVec<u8>& pixels, const SkImageInfo& info) {
    //     // Your custom color/alpha adjustment loop
    //     for (int y = 0; y < info.height(); y++) {
    //         auto* row = reinterpret_cast<uint32_t*>(pixels.data() + y * info.minRowBytes());
    //         for (int x = 0; x < info.width(); x++) {
    //             auto* px = reinterpret_cast<uint8_t*>(&row[x]);
    //             uint8_t a = px[3];
    //             px[0] = (px[0] * a) / 255; // B
    //             px[1] = (px[1] * a) / 255; // G
    //             px[2] = (px[2] * a) / 255; // R
    //         }
    //     }
    // }

    std::unique_ptr<SkCodec> m_codec;
};