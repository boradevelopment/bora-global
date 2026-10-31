// Apart of the BORA Runtime Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
#ifdef BORA_UI_SUPPORT
#include "SkiaCPURenderer.h"

#include <include/core/SkCanvas.h>
#include "nGraphics/ImmediateGraphicsAbstract.h"
#include "nWindow/bnWindow.h"

SkiaCPURenderer::SkiaCPURenderer()
{

}

SkiaCPURenderer::SkiaCPURenderer(bnWindow* window) : window(window)
{
    engine = window->rootDevice;
    auto graphics = window->graphics;

    BlendStateDesc blendDesc;
    blendDesc.renderTarget[0].blendEnable = true;
    blendDesc.renderTarget[0].srcBlend = Blend::SrcAlpha;
    blendDesc.renderTarget[0].destBlend = Blend::InvSrcAlpha;
    blendDesc.renderTarget[0].blendOp = BlendOp::Add;
    blendDesc.renderTarget[0].renderTargetWriteMask = 0x0F;

    graphics->CreateBlendState(blendDesc, &blending);

    // Rasterizer
    RasterizerDesc rastDesc{};
    rastDesc.fillMode = IFillMode::Solid;        // Solid fill (Wireframe if debugging)
    rastDesc.cullMode = CullMode::Back;         // Cull back faces
    rastDesc.frontCounterClockwise = false;                  // CCW = front by default
    rastDesc.depthBias = 0;
    rastDesc.depthBiasClamp = 0.0f;
    rastDesc.slopeScaledDepthBias = 0.0f;
    rastDesc.depthClipEnable = true;                   // Enable depth clipping
    rastDesc.scissorEnable = false;                  // Disable unless using scissors
    rastDesc.multisampleEnable = false;                  // Enable if MSAA on
    rastDesc.antialiasedLineEnable = false;                  // Only relevant if wireframe lines

    graphics->CreateRasterizerState(rastDesc, &rast);

    // Depth-stencil
    DepthStencilDesc depDesc{};
    depDesc.depthEnable = true;                         // Enable depth testing
    depDesc.depthWriteMask = true;          // Write all depths
    depDesc.depthFunc = ComparisonFunc::LessEqual;    // Pass if <= existing depth

    depDesc.stencilEnable = false;                        // Disable stencil by default
    depDesc.stencilReadMask = 0xFF;
    depDesc.stencilWriteMask = 0xFF;

    // If you want stencil ops (rare for defaults), init like this:
    depDesc.frontFace.failOp = StencilOp::Keep;
    depDesc.frontFace.depthFailOp = StencilOp::Keep;
    depDesc.frontFace.passOp = StencilOp::Keep;
    depDesc.frontFace.func = ComparisonFunc::Always;

    depDesc.backFace = depDesc.frontFace;

    graphics->CreateDepthStencilState(depDesc, &depth);

    // todo: move it somewhere more unified.
    auto vertexData = bnShader(ShaderDesc::Type::Vertex, "./titlebarvert.spv");
    auto fragmentData = bnShader(ShaderDesc::Type::Pixel, "./titlebarfrag.spv");

    vertexShader = vertexData.getShader(window->rootDevice, window->choice);
    fragmentShader = fragmentData.getShader(window->rootDevice, window->choice);

    auto graphicsCommands = window->graphics->makeAdvanced();

    graphics->CreateInputLayout({ {

        {  0, VertexAttribType::Float3, 0, offsetof(Vertex, x), false},
            { 1, VertexAttribType::Float2, 0, offsetof(Vertex, u), false },
        }, sizeof(Vertex), &vertexShader }, &inputLayout);


    graphics->CreateSamplerState({
    0, TextureFilter::Linear,TextureFilter::Linear,TextureFilter::Linear
      }, &samplerState);

    graphicsCommands->CreateDescriptorSetLayout({
                {{0, DescriptorType::CombinedImageSampler, 1, IShaderStage::Fragment }}
        }, &dSetLayout);
}

sk_sp<SkSurface> SkiaCPURenderer::CreateSurfaceFromTexture(ITexture* texture)
{
    // if graphics device available, get texture data (copy it) and create a rasterized surface from it.
    // Texture will need to be CPU readable i believe.
    if (!engine) return nullptr;

    uint width = texture->desc.width;
    uint height = texture->desc.height;
    uint widthBytes = texture->desc.widthBytes;

    // todo check texture color type.
    SkImageInfo info = SkImageInfo::Make(
        width, height,
        kRGBA_8888_SkColorType,
        kPremul_SkAlphaType
    );

    sk_sp<SkSurface> surface = SkSurfaces::Raster(info);
    if (!surface) return nullptr;

    sVec<u8> pixelData = engine->ReadPixelsOfTexture(texture);
    if (pixelData.empty()) {
        return nullptr; // Readback failed or texture wasn't CPU-readable
    }

    surface->getCanvas()->writePixels(info, pixelData.data(), widthBytes, 0, 0);
    return surface;
}

IGUISurface* SkiaCPURenderer::getLastFrameSurface()
{
    auto surface = surfaces.back();
    return surface.uxSurface;
}

IGUISurface* SkiaCPURenderer::createFrameSurface(ResourceHandle<ITexture> texture)
{
    return createFrameSurface({}, texture);
}

IGUISurface* SkiaCPURenderer::createFrameSurface(const UXRect& rect, ResourceHandle<ITexture> texture)
{
    auto localRect = rect;
    if (localRect.IsEmpty() && texture)
    {
        u32 height = texture->desc.height;
        u32 width = texture->desc.width;

        localRect.left   = 0;
        localRect.top    = 0;
        localRect.right  = static_cast<long>(width);
        localRect.bottom = static_cast<long>(height);
    }

    for (auto it = lastFrameSurfaces.begin(); it != lastFrameSurfaces.end(); ++it)
    {
        auto existingRect = it->uxSurface->GetRect();
        if (!texture && existingRect.GetWidth() >= localRect.GetWidth() &&
            existingRect.GetHeight() >= localRect.GetHeight())
        {
            it->uxSurface->SetRect(localRect);
            it->isGeometryDirty = true;
            it->isDirty = true;
            surfaces.push_back(std::move(*it));
            lastFrameSurfaces.erase(it);

            return surfaces.back().uxSurface;
        }
    }

    sk_sp<SkSurface> skSurface;
    if (texture)
    {
        skSurface = CreateSurfaceFromTexture(texture);
    } else
    {
        SkImageInfo imageInfo = SkImageInfo::Make(
        localRect.GetWidth(),
        localRect.GetHeight(),
        kBGRA_8888_SkColorType,
        kPremul_SkAlphaType,
        SkColorSpace::MakeSRGB()
    );

        SkSurfaceProps props(0, kUnknown_SkPixelGeometry);
        skSurface = SkSurfaces::Raster(imageInfo, &props);
    }

    if (!skSurface)
    {
        return nullptr; // Creation failed
    }

    CPUSurfaceData data;
    data.uxSurface = new SkiaSurface(localRect, skSurface);
    surfaces.push_back(std::move(data));

    return surfaces.back().uxSurface;
}

void SkiaCPURenderer::finalizeResources()
{

}

void SkiaCPURenderer::endframe()
{
    for (CPUSurfaceData& cpuSurface : lastFrameSurfaces)
    {
        destroySurface(cpuSurface);
    }
    lastFrameSurfaces.clear();

    for (CPUSurfaceData& cpuSurface : surfaces) {
        auto surface = cpuSurface.uxSurface;
        if (!surface) continue;

        if (auto* canvas = surface->GetCanvas()) {
            canvas->Restore();
        }

        if (engine)
        {
            auto commandList = window->graphics->makeAdvanced();
            const u8* pixels = surface->GetRawPixels();
            size_t pixelSize = surface->GetTotalByteSize();

            if (cpuSurface.isDirty)
            {
                if (cpuSurface.texture) window->graphics->ReleaseTexture(cpuSurface.texture);
                TextureDesc desc;
                desc.height = surface->GetHeight();
                desc.width = surface->GetWidth();
                desc.widthBytes = surface->GetRowBytes();
                desc.CpuAccessWrite = true;
                desc.format = TextureFormat::BGRA8_UNorm_SRGB; // def for now
                cpuSurface.texture = window->graphics->CreateTexture(desc, pixels);
                cpuSurface.isDirty = false;
            }

            if (cpuSurface.isGeometryDirty)
            {
                if (cpuSurface.vertexBuffer) window->graphics->ReleaseBuffer(cpuSurface.vertexBuffer);
                if (cpuSurface.viewportHandle) window->graphics->ReleaseViewPort(&cpuSurface.viewportHandle);


                UXRect rect = cpuSurface.uxSurface->GetRect();
                cpuSurface.viewport.minDepth = 0;
                cpuSurface.viewport.maxDepth = 1;
                cpuSurface.viewport.height = rect.GetHeight();
                cpuSurface.viewport.width = rect.GetWidth();
                cpuSurface.viewport.x = rect.left;
                cpuSurface.viewport.y = rect.top;

                window->graphics->CreateViewPort({&cpuSurface.viewport}, &cpuSurface.viewportHandle);

                Vertex vertices[] = {
                    { -1.0f, -1.0f, 0.0f, 0.0f, 0.0f }, // top-left
                    {  1.0f, -1.0f, 0.0f, 1.0f, 0.0f }, // top-right
                    { -1.0f,  1.0f, 0.0f, 0.0f, 1.0f }, // bottom-left
                    {  1.0f,  1.0f, 0.0f, 1.0f, 1.0f }, // bottom-right
                };

                BufferDesc tbBufferDesc;
                tbBufferDesc.type = BufferType::Vertex;
                tbBufferDesc.size = sizeof(vertices);
                tbBufferDesc.stride = sizeof(Vertex);
                tbBufferDesc.dynamic = false;
                cpuSurface.vertexBuffer = window->graphics->CreateBuffer(tbBufferDesc, &vertices);

                cpuSurface.isGeometryDirty = false;
            }

            if (!cpuSurface.set)
            {
                cpuSurface.dPool = commandList->CreateDescriptorPool({
        {{DescriptorType::CombinedImageSampler, 1}},
        1, DescriptorPoolFlags::FreeDescriptor
              });

                auto builderObj = commandList->CreatePipelineBuilder();
                commandList->BuilderAddShader(builderObj, &fragmentShader);
                commandList->BuilderAddShader(builderObj, &vertexShader);
                commandList->BuilderSetBlendState(builderObj, &blending);
                commandList->BuilderSetInputLayout(builderObj, &inputLayout);
                commandList->BuilderSetRasterizer(builderObj, &rast);
                commandList->BuilderSetDepthStencil(builderObj, &depth);
                commandList->BuilderSetDescriptorPool(builderObj, cpuSurface.dPool);
                commandList->BuilderSetDescriptorSetLayout(builderObj, &dSetLayout);
                cpuSurface.pipeline = commandList->CreatePipeline(builderObj);
                commandList->ReleaseBuilder(builderObj);
                commandList->CreateDescriptorSet(cpuSurface.pipeline, 0, &cpuSurface.set);
            }
            commandList->SetDescriptorSetTexture(&cpuSurface.set, cpuSurface.texture);
            commandList->SetDescriptorSetSamplerState(&cpuSurface.set, &samplerState);
            commandList->UpdateDescriptorSet(&cpuSurface.set);

            auto list = commandList->GetCommandList();
            list->BindViewPort(&cpuSurface.viewportHandle);
            list->BindScissor(&cpuSurface.viewportHandle);
            list->BindPipeline(cpuSurface.pipeline);
            list->BindDescriptorSet(&cpuSurface.set);
            list->BindBuffer(cpuSurface.vertexBuffer);
            list->Draw(PrimitiveType::TriangleStrip, 4);
        }
    }

    // auto copyCmd = window->graphics->BeginSingleTimeCommands(engine->copyPool);
    // copyCmd->PipelineBarrier(backBuffer, backBuffer->explicitLayout, ImageLayout::RenderTarget, ImageAccessLayout::Write, ImageAccessLayout::Write);
    // engine->EndSingleTimeCommands(copyCmd);

    // Move surfaces to previousSurfaces to be reused
    lastFrameSurfaces = std::move(surfaces);
    surfaces.clear();
}

void SkiaCPURenderer::destroySurface(CPUSurfaceData& cpuSurface)
{
    auto graphics = window->graphics;
    auto commandList = graphics->makeAdvanced();

    delete cpuSurface.uxSurface;
    graphics->ReleaseBuffer(cpuSurface.vertexBuffer);
    graphics->ReleaseTexture(cpuSurface.texture);
    commandList->ReleasePipeline(cpuSurface.pipeline);
    commandList->ReleaseDescriptorPool(cpuSurface.dPool);

}
SkiaCPURenderer::~SkiaCPURenderer()
{
    if (engine)
    {
        auto graphics = window->graphics;
        auto commandList = graphics->makeAdvanced();

        graphics->ReleaseInputLayout(&inputLayout);
        graphics->ReleaseShader(&vertexShader);
        graphics->ReleaseShader(&fragmentShader);
        graphics->ReleaseSamplerState(&samplerState);
        graphics->ReleaseBlenderState(&blending);
        graphics->ReleaseRasterizerState(&rast);

        commandList->ReleaseDescriptorSetLayout(&dSetLayout);
        for (CPUSurfaceData& cpuSurface : lastFrameSurfaces)
        {
            destroySurface(cpuSurface);
        }
        for (CPUSurfaceData& cpuSurface : surfaces)
        {
            destroySurface(cpuSurface);
        }
    }
}

#endif