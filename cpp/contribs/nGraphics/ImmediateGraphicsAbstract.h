// BORA_INCLUDED_IN_SDK
// ^^^ Never remove this! This indicates that this should be included in production builds of libbora
#pragma once
#include "GraphicsAbstractions.h"

// For high level apis like Microsoft D3D11 and OPENGL
class IGraphicsDeviceImmediate : IGraphicsDevice {
public:
    virtual const char* GetAPIName() const = 0;
    virtual uint32_t GetAPIVersion() const = 0; 
    virtual bool IsFeatureSupported(const std::string& feature) const = 0;
    virtual u8 GetFlags() const {
        return GraphicsDeviceFlags::IS_IMMEDIATE;
    }

    virtual bool Init() { return false; };
    virtual void BeginFrame() {};
    virtual void EndFrame() {};
    virtual void Present() {};
    virtual void Resize(long width, long height) {};

    virtual ITexture* GetSwapchainImage() { return nullptr; };

    virtual ITexture* CreateTexture(const TextureDesc& desc, const void* initialData = nullptr) { return nullptr; };
    virtual IShader* CreateShader(const ShaderDesc& desc) { return nullptr; };
    virtual IBuffer* CreateBuffer(const BufferDesc& desc, const void* data = nullptr) { return nullptr; };
    virtual IInputLayout* CreateInputLayout(const InputLayoutDesc& desc) { return nullptr; };
    virtual ISamplerState* CreateSamplerState(const SamplerStateDesc& desc) { return nullptr; };
    virtual IViewPort* CreateViewPort(const ViewPortDesc& desc) { return nullptr; };
    virtual IRasterizerState* CreateRasterizerState(const RasterizerDesc& desc) { return nullptr; };
    virtual IDepthStencilState* CreateDepthStencilState(const DepthStencilDesc& desc) { return nullptr; };
    virtual IBlendState* CreateBlendState(const BlendStateDesc& desc) { return nullptr; };
    virtual IRenderTarget* CreateRenderTarget(const RenderTargetDesc& desc) { return nullptr; };
    virtual IDepthStencil* CreateDepthStencil(ITexture* texture) { return nullptr; };

    virtual void ReleaseShader(IShader**) {};
    virtual void ReleaseBuffer(IBuffer**) {};
    virtual void ReleaseTexture(ITexture**) {};

    virtual void BindShader(IShader*) {};
    virtual void BindBuffer(IBuffer*) {};
    virtual void BindTexture(ITexture*, uint slot = 0) {};
    virtual void BindInputLayout(IInputLayout*) {};
    virtual void BindSamplerState(ISamplerState*, uint slot = 0) {};
    virtual void BindViewPort(IViewPort*) {};
    virtual void BindRasterizerState(IRasterizerState*) {};
    virtual void BindDepthStencilState(IDepthStencilState*, uint stencilRef = 0) {};
    virtual void BindBlendState(IBlendState*, const float blendFactor[4], uint sampleMask = 0xFFFFFFFF) {};
    virtual void BindRenderTarget(IRenderTarget*, IDepthStencil* = nullptr) {};
    virtual void ClearRenderTarget(IRenderTarget* target, const float color[4]) {};
    virtual void ClearDepthStencil(IDepthStencil* target, float depth, uint stencil) {};
    virtual void DispatchCompute(uint x, uint y, uint z) {};
    virtual void CopyToBuffer(IBuffer* buffer, void* data, size_t size) {};
    virtual void Draw(PrimitiveType type, size_t vertexCount, size_t vertexOffset = 0) {};
    virtual void DrawIndexed(PrimitiveType type, IBuffer* indexBuffer, size_t indexCount, size_t indexOffset = 0) {};


    virtual void MapBufferMemory(IBuffer* buffer, void** dataPtr) {};
    virtual void UnmapBufferMemory(IBuffer* buffer) {};
    virtual void CopyBufferToImage(IBuffer* srcBuffer, ITexture* dstTexture, BufferImageCopyDesc desc) {};
    virtual void CopyImageToImage(ITexture* srcBuffer, ITexture* dstBuffer, ImageCopyDesc desc) {};

    virtual IDeviceContext* getContext() { return nullptr; };
    virtual IDevice* getDevice() { return nullptr; };

    virtual void Shutdown() = 0;
};

