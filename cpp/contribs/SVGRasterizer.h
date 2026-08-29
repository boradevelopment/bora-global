// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.

/* 
 * FileName: SVGRasterizer.h
 * Purpose: A static class that rasterizes an SVG based file and returns a PNG based on the arguments provided,
 * This class will only return and provide raw binary data for simplicity, with the returning type being a vector!
 *
 * Notes: This class uses headers and is designed to have every platform supported on one file instead of seperate
 * files.
 */
#pragma once
#ifdef BORA_UI_SUPPORT
#include "modules/svg/include/SkSVGDOM.h"
#include "include/core/SkStream.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "nGUI/skia/GPUContextManager.h"
// OS Rasterization is for OS Level rasterization SUPPORTED, for example, Windows Direct2D
// I'm not so sure if OSX has a native SVG rasterizer as well and for linux as well, but if they don't.
// I would just use skia rasterization anyway.
enum RasterizationBackend {
    OS_RASTERIZATION, // OS Supported rasterizers
    SKIA_RASTERIZATION_CPU, // CPU based Rasterization on skia's end for simple things
    SKIA_RASTERIZATION_GPU // GPU based Rasterization on skia's end for more demanding things
};

enum class RasterizationPolicy {
    Auto,          // Chooses what's the best backend for the SVG
    NativeFast,    // Prefer OS APIs if possible
    Portable,      // Force Skia
    Deterministic  // Force CPU Skia
};

struct SvgFeatures {
    bool usesFilters = false;
    bool usesMasks = false;
    bool usesText = false;
    bool isLarge = false;
};

#ifdef _WIN64
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <wrl/client.h>
#include <wincodec.h>

inline D3D_FEATURE_LEVEL featureLevels[] =
{
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1
};
#endif

class SVGRasterizer {
public:
    static bool rasterize(
     const u8* svgData,
     size_t svgSize,
     u32 width,
     u32 height,
     std::vector<u8>& outRGBA,
     RasterizationPolicy policy = RasterizationPolicy::Auto
    );
private:
    static RasterizationBackend getBestBackend(RasterizationPolicy policy, const u8* svgData, size_t svgSize);
#ifdef _WIN64
    inline  static D3D_FEATURE_LEVEL d3dFeatureLevel;
    inline static Microsoft::WRL::ComPtr<ID3D11Device> device;
    inline static Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    inline static Microsoft::WRL::ComPtr<ID3D11Device2>           d3dDevice;
    inline static Microsoft::WRL::ComPtr<ID3D11DeviceContext3>    d3dContext;
    inline static Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    inline static Microsoft::WRL::ComPtr<ID2D1Device5>            d2dDevice;
    inline static Microsoft::WRL::ComPtr<ID2D1DeviceContext5>     d2dContext;
    inline static Microsoft::WRL::ComPtr<ID2D1Factory6> factory;

    static bool D2DVectorSVGLoader(
        const uint8_t *data,
        size_t size,
        UINT width,
        UINT height, std::vector<u8> &pngData
    );
#endif

    static SvgFeatures inspectSVG(const u8* data, size_t size);

    static bool supportsOSRasterization()
    {
#ifdef _WIN64
        return true; // Direct2D SVG supported (Win10+)
#else
        return false;
#endif
    }

    // todo: add something better than this
    static bool supportsSkiaGPU()
    {
        return true;
    }


    static bool rasterizeSkiaCPU(
        const u8* svgData,
        size_t svgSize,
        u32 width,
        u32 height,
        std::vector<u8>& outRGBA
    );

    static bool rasterizeSkiaGPU(
        GrDirectContext* grContext,
        const u8* svgData,
        size_t svgSize,
        u32 width,
        u32 height,
        std::vector<u8>& outRGBA
    );



    static sk_sp<SkSVGDOM> loadSVGFromMemory(
        const u8* data,
        size_t size,
        u32 width,
        u32 height
    ) {;
        auto stream = std::make_unique<SkMemoryStream>(data, size, false);

        SkSVGDOM::Builder builder;
        builder.setFontManager(SkFontMgr::RefEmpty());

        auto svg = builder.make(*stream);
        if (!svg) return nullptr;

        svg->setContainerSize(SkSize::Make(width, height));
        return svg;
    }
};
#endif