// Apart of the BORA Runtime Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
#ifdef BORA_UI_SUPPORT
#include "SVGRasterizer.h"
#ifdef _WIN64
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#endif

bool SVGRasterizer::rasterize(const u8 *svgData, size_t svgSize, u32 width, u32 height, std::vector<u8> &outRGBA, RasterizationPolicy policy) {
    RasterizationBackend backend = getBestBackend(policy, svgData, svgSize);
    switch (backend) {
        case OS_RASTERIZATION:
#ifdef _WIN64
            return D2DVectorSVGLoader(
                svgData, svgSize, width, height, outRGBA
            );
#else
            return false;
#endif

        case SKIA_RASTERIZATION_CPU:
            return rasterizeSkiaCPU(
                svgData, svgSize, width, height, outRGBA
            );

        case SKIA_RASTERIZATION_GPU:
            return rasterizeSkiaGPU(
                GPUContextManager::getGaneshHeadlessContext().get(),
                svgData, svgSize, width, height, outRGBA
            );
    }

    return false;
}

RasterizationBackend SVGRasterizer::getBestBackend(RasterizationPolicy policy, const u8 *svgData, size_t svgSize) {
    SvgFeatures features = inspectSVG(svgData, svgSize);

    switch(policy) {
        case RasterizationPolicy::Deterministic:
            return SKIA_RASTERIZATION_CPU;
        case RasterizationPolicy::Portable: {
            if (supportsSkiaGPU())
                return SKIA_RASTERIZATION_GPU;
            return SKIA_RASTERIZATION_CPU;
        }
        case RasterizationPolicy::NativeFast: {
            if (supportsOSRasterization()) {
                if (!features.usesFilters &&
                    !features.usesMasks &&
                    !features.usesText &&
                    !features.isLarge) {
                    return OS_RASTERIZATION;
                    }
            }

            // fallback
            if (supportsSkiaGPU())
                return SKIA_RASTERIZATION_GPU;

            return SKIA_RASTERIZATION_CPU;
        }
        case RasterizationPolicy::Auto: {
            if (supportsOSRasterization()) {
                if (!features.usesFilters &&
                    !features.usesMasks &&
                    !features.isLarge) {
                    return OS_RASTERIZATION;
                    }
            }

            if (supportsSkiaGPU())
                return SKIA_RASTERIZATION_GPU;

            return SKIA_RASTERIZATION_CPU;
        }
        default:
            return SKIA_RASTERIZATION_CPU;
    }
}

bool SVGRasterizer::D2DVectorSVGLoader(const uint8_t *data, size_t size, UINT width, UINT height, std::vector<u8> &pngData){
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    HRESULT hr;
    D2D1_MAPPED_RECT mapped{};
    Microsoft::WRL::ComPtr<ID2D1SvgDocument> svg = nullptr;
    static Microsoft::WRL::ComPtr<IWICImagingFactory> wic;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> gpuBMP = nullptr;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> bmp = nullptr;
    Microsoft::WRL::ComPtr<IWICStream> stream;
    float scaleX = 0.0f;
    float scaleY = 0.0f;
    float scale  = 0.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    Microsoft::WRL::ComPtr<ID2D1SvgElement> root;
    float svgWidth = 0, svgHeight = 0;
    Microsoft::WRL::ComPtr<IWICBitmap> wicBitmap;
    Microsoft::WRL::ComPtr<IWICBitmapEncoder> encoder;
    Microsoft::WRL::ComPtr<IWICStream> pngStream;
    Microsoft::WRL::ComPtr<IWICBitmapFrameEncode> frame;
    WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;
    ULONG bytesRead = 0;
    STATSTG stat{};

    if (!factory) {
        D2D1_FACTORY_OPTIONS options;
        ZeroMemory(&options, sizeof(options));

        hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
                 __uuidof(ID2D1Factory6),
                 &options,
                 &factory
        );
        if (FAILED(hr)) goto cleanup;

        hr = CoCreateInstance(
    CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
    IID_PPV_ARGS(&wic)
    );
        if (FAILED(hr)) goto cleanup;
    }

    // Open SVG file
    hr = wic->CreateStream(stream.GetAddressOf());
    if (FAILED(hr)) goto cleanup;

    hr = stream->InitializeFromMemory(const_cast<BYTE*>(data), static_cast<DWORD>(size));
    if (FAILED(hr)) goto cleanup;

    if (!device) {
        HRESULT createResult = D3D11CreateDevice(
            nullptr,                    // Specify nullptr to use the default adapter.
            D3D_DRIVER_TYPE_HARDWARE,   // Create a device using the hardware graphics driver.
            0,                          // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
            creationFlags,              // Set Direct2D compatibility flags.
            featureLevels,              // List of feature levels this app can support.
            ARRAYSIZE(featureLevels),   // Size of the list above.
            D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Runtime apps.
            &device,                    // Returns the Direct3D device created.
            &d3dFeatureLevel,         // Returns feature level of device created.
            &context                    // Returns the device immediate context.
        );

        if (FAILED(createResult))
        {
            // If the initialization fails, fall back to the WARP device.
            // For more information on WARP, see:
            // http://go.microsoft.com/fwlink/?LinkId=286690
            createResult = D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
                0,
                creationFlags,
                featureLevels,
                ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,
                &device,
                &d3dFeatureLevel,
                &context
            );

            if (FAILED(createResult)) goto cleanup;
        }

        device.As(&d3dDevice);
        context.As(&d3dContext);
        d3dDevice.As(&dxgiDevice);
    }

    if(!d2dDevice){
        // Create device context
        hr = factory->CreateDevice(dxgiDevice.Get(), &d2dDevice);
        if (FAILED(hr)) goto cleanup;
        hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);
        if (FAILED(hr)) goto cleanup;
    }

    hr = d2dContext->CreateSvgDocument(
        stream.Get(),
        D2D1::SizeF((FLOAT)width, (FLOAT)height),
        &svg
    );
    if (FAILED(hr)) goto cleanup;
    ;
    // Get the root <svg> element to see its internal width/height
    svg->GetRoot(&root);

    // Retrieve the "width" and "height" attributes from the XML
    if (root->IsAttributeSpecified(L"width"))
        root->GetAttributeValue(L"width", &svgWidth);
    if (root->IsAttributeSpecified(L"height"))
        root->GetAttributeValue(L"height", &svgHeight);

    // Fallback: If no width/height, use the viewBox
    if (svgWidth <= 0 || svgHeight <= 0) {
        D2D1_SVG_VIEWBOX viewBox;

        if (SUCCEEDED(root->GetAttributeValue(L"viewBox", D2D1_SVG_ATTRIBUTE_POD_TYPE_VIEWBOX, &viewBox, sizeof(viewBox)))) {
            svgWidth = viewBox.width;
            svgHeight = viewBox.height;
        }
    }

    svg->SetViewportSize(D2D1::SizeF(svgWidth, svgHeight));

    // Create target bitmap
    D2D1_BITMAP_PROPERTIES1 props =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(
                DXGI_FORMAT_B8G8R8A8_UNORM,
                D2D1_ALPHA_MODE_PREMULTIPLIED
            )
        );

    hr = d2dContext->CreateBitmap(
        D2D1::SizeU(width, height),
        nullptr, 0,
        props,
        &gpuBMP
    );
    if (FAILED(hr)) goto cleanup;

    // Render SVG
    d2dContext->SetTarget(gpuBMP.Get());
    d2dContext->BeginDraw();
    D2D1_SIZE_F svgSize = svg->GetViewportSize();
    scaleX = width / svgSize.width;
    scaleY = height / svgSize.height;
    scale = std::min(scaleX, scaleY);

    offsetX = (width - svgSize.width * scale) / 2.0f;
    offsetY = (height - svgSize.height * scale) / 2.0f;

    // Translate first, then scale
    d2dContext->SetTransform(
        D2D1::Matrix3x2F::Translation(offsetX, offsetY) *
        D2D1::Matrix3x2F::Scale(scale, scale, D2D1::Point2F(0,0))
    );

    d2dContext->DrawSvgDocument(svg.Get());
    hr = d2dContext->EndDraw();
    if (FAILED(hr)) goto cleanup;

    D2D1_BITMAP_PROPERTIES1 cpuProps =
          D2D1::BitmapProperties1(
              D2D1_BITMAP_OPTIONS_CANNOT_DRAW | D2D1_BITMAP_OPTIONS_CPU_READ,
              D2D1::PixelFormat(
                  DXGI_FORMAT_B8G8R8A8_UNORM,
                  D2D1_ALPHA_MODE_PREMULTIPLIED
              )
          );

    hr = d2dContext->CreateBitmap(
    D2D1::SizeU(width, height),
    nullptr, // No initial data
    0,       // Pitch
    &cpuProps,
    &bmp
    );
    if (FAILED(hr)) goto cleanup;

    hr = bmp->CopyFromBitmap(
        nullptr,          // destPoint (entire bitmap)
        gpuBMP.Get(),
        nullptr           // srcRect (entire bitmap)
    );
    if (FAILED(hr)) goto cleanup;

    // Copy bitmap to HBITMAP
    hr = bmp->Map(D2D1_MAP_OPTIONS_READ, &mapped);
    if (FAILED(hr)) goto cleanup;


    hr = wic->CreateBitmapFromMemory(
        width,
        height,
        GUID_WICPixelFormat32bppPBGRA, // matches D2D bitmap
        mapped.pitch,
        mapped.pitch * height,
        mapped.bits,
        &wicBitmap
    );
    if (FAILED(hr)) goto cleanup;

    hr = wic->CreateStream(&pngStream);
    if (FAILED(hr)) goto cleanup;

    hr = pngStream->InitializeFromMemory(nullptr, 0);
    if (FAILED(hr)) goto cleanup;

    hr = wic->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder);
    if (FAILED(hr)) goto cleanup;

    hr = encoder->Initialize(pngStream.Get(), WICBitmapEncoderNoCache);
    if (FAILED(hr)) goto cleanup;


    hr = encoder->CreateNewFrame(&frame, nullptr);
    if (FAILED(hr)) goto cleanup;

    hr = frame->Initialize(nullptr);
    if (FAILED(hr)) goto cleanup;

    hr = frame->SetSize(width, height);
    if (FAILED(hr)) goto cleanup;


    hr = frame->SetPixelFormat(&format);
    if (FAILED(hr)) goto cleanup;

    hr = frame->WriteSource(wicBitmap.Get(), nullptr);
    if (FAILED(hr)) goto cleanup;

    hr = frame->Commit();
    if (FAILED(hr)) goto cleanup;

    hr = encoder->Commit();
    if (FAILED(hr)) goto cleanup;

    hr = pngStream->Stat(&stat, STATFLAG_NONAME);
    if (FAILED(hr)) goto cleanup;

    pngData.clear();
    pngData.resize(stat.cbSize.LowPart);
    hr = pngStream->Read(pngData.data(), stat.cbSize.LowPart, &bytesRead);
    if (FAILED(hr)) goto cleanup;
    pngData.resize(bytesRead);
    bmp->Unmap();

    return true;

cleanup: {
    return false;
}
}

SvgFeatures SVGRasterizer::inspectSVG(const u8 *data, size_t size) {
    SvgFeatures f{};

    std::string_view svg(reinterpret_cast<const char*>(data), size);

    f.usesFilters = svg.find("<filter") != std::string_view::npos;
    f.usesMasks  = svg.find("<mask")   != std::string_view::npos;
    f.usesText   = svg.find("<text")   != std::string_view::npos;

    // crude but useful heuristic
    f.isLarge = size > 64 * 1024; // 64 KB SVGs tend to be complex

    return f;
}

bool SVGRasterizer::rasterizeSkiaCPU(const u8 *svgData, size_t svgSize, u32 width, u32 height,
    std::vector<u8> &outRGBA) {
    auto svg = loadSVGFromMemory(svgData, svgSize, width, height);
    if (!svg) return false;

    SkImageInfo info = SkImageInfo::Make(
     width,
     height,
     kRGBA_8888_SkColorType,
     kPremul_SkAlphaType
 );

    auto surface = SkSurfaces::Raster(info);
    if (!surface) return false;

    SkCanvas* canvas = surface->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);


    svg->render(canvas);

    outRGBA.resize(width * height * 4);

    SkPixmap pixmap;
    if (!surface->peekPixels(&pixmap)) return false;

    // Copy row-by-row to respect rowBytes
    for (u32 y = 0; y < height; ++y) {
        memcpy(
            outRGBA.data() + y * width * 4,
            (const u8*)pixmap.addr(0, y),
            width * 4
        );
    }

    return true;
}

bool SVGRasterizer::rasterizeSkiaGPU(GrDirectContext *grContext, const u8 *svgData, size_t svgSize, u32 width,
    u32 height, std::vector<u8> &outRGBA) {
    if (!grContext) return false;

    auto svg = loadSVGFromMemory(svgData, svgSize, width, height);
    if (!svg) return false;

    SkImageInfo info = SkImageInfo::Make(
        width,
        height,
        kRGBA_8888_SkColorType,
        kPremul_SkAlphaType
    );

    auto surface = SkSurfaces::RenderTarget(
    grContext,
    skgpu::Budgeted::kYes,
    info
);
    if (!surface) return false;

    SkCanvas* canvas = surface->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    svg->render(canvas);

    grContext->flushAndSubmit();

    outRGBA.resize(width * height * 4);

    SkPixmap pixmap(info, outRGBA.data(), width * 4);

    // GPU → CPU readback
    if (!surface->readPixels(pixmap, 0, 0))
        return false;

    return true;
}
#endif