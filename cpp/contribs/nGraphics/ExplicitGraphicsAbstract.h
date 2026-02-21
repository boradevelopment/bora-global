#pragma once
#include "GraphicsAbstractions.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
// For low level APIS like D3D12 and Vulkan, things aren't as immediate and obviously the designs have to be changed.

// todo: implement independence system
class IDescriptorSet {
public:
    virtual ~IDescriptorSet() = default;
   
    virtual void SetTexture(u32 slot, ITexture* tex) {};
    virtual void SetBuffer(u32 slot, IBuffer* buf) {};
    virtual void SetSampler(u32 slot, ISamplerState* sampler) {};

    virtual void Update() {};
    virtual void Release() {};
};

class IPipeline {
public:
    virtual ~IPipeline() = default;

    IRenderTarget* renderTarget;
    virtual void* GetNativeHandle() { return nullptr; };
    virtual void Release() {}
    virtual IDescriptorSet* CreateDescriptorSet(u32 slot = 0) { return nullptr; };
    virtual IDescriptorSet* GetDescriptorSet(u32 slot = 0) { return nullptr; };
};

class ICommandBuffer {
public:
    virtual ~ICommandBuffer() = default;

    virtual void* GetNativeHandle() { return nullptr; };
    virtual void PipelineBarrier(
        ITexture* image,
        ImageLayout oldLayout,
        ImageLayout newLayout,
        ImageAccessLayout srcAccessMask,
        ImageAccessLayout dstAccessMask) = 0;
    virtual void PipelineBarrierBatched( 
        ITexture* image,
        ImageLayout oldLayout,
        ImageLayout newLayout,
        ImageAccessLayout srcAccessMask,
        ImageAccessLayout dstAccessMask) = 0;
    virtual void FlushBatchedBarriers() = 0;
};

class ICommandPool {
public:
    virtual ~ICommandPool() = default;

    virtual void* GetNativeHandle() { return nullptr; };
    virtual void Release() {}
};

class IDescriptorPool {
public:
    virtual ~IDescriptorPool() = default;

    virtual void* GetNativeHandle() { return nullptr; };
    virtual void Release() {}
};

class IDescriptorSetLayout {
public:
    virtual ~IDescriptorSetLayout() = default;

    virtual void* GetNativeHandle() { return nullptr; };
    virtual void Release() {}
};

class IPipelineBuilder {
public:
    virtual ~IPipelineBuilder() = default;

    virtual IPipelineBuilder& AddShader(IShader* shader) = 0;

    virtual IPipelineBuilder& SetInputLayout(IInputLayout* layout) = 0;

    virtual IPipelineBuilder& SetRenderTarget(IRenderTarget* target) = 0;

    virtual IPipelineBuilder& SetRasterizer(IRasterizerState* raster) = 0;

    virtual IPipelineBuilder& SetDepthStencil(IDepthStencilState* depth) = 0;

    virtual IPipelineBuilder& SetBlendState(IBlendState* blend) = 0;

    virtual IPipelineBuilder& SetDescriptorPool(IDescriptorPool* pool) = 0;

    virtual IPipelineBuilder& SetDescriptorSetLayout(IDescriptorSetLayout* pool) = 0;

    virtual sVec<IShader*>* GetShaders() = 0;

    virtual IPipeline* Build() { return nullptr; };
};

struct PipelineDesc {
    std::vector<IShader*> shaders;
    IInputLayout* inputLayout;
    IRasterizerState* rasterizer;
    IDepthStencil* depthStencil;
    IBlendState* blendState;
};

struct CommandPoolDesc {
    enum class Type {
        DIRECT,
        COMPUTE,
        COPY
    };
    uint32_t queueFamilyIndex;  // Required: which queue family this pool is for
    bool transient = false;     // Short-lived command buffers?
    bool resettable = false;    // Allow resetting individual command buffers?   
    CommandPoolDesc::Type type;
};

enum class DescriptorType {
    UniformBuffer,
    CombinedImageSampler,
    StorageBuffer,
    StorageImage,
    Sampler,
    // Add more if needed
};

enum class DescriptorPoolFlags : uint32_t {
    None = 0,
    FreeDescriptor = 1 << 0,  
    UpdateAfterBind = 1 << 1,  
};

inline DescriptorPoolFlags operator|(DescriptorPoolFlags a, DescriptorPoolFlags b) {
    return static_cast<DescriptorPoolFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline bool operator&(DescriptorPoolFlags a, DescriptorPoolFlags b) {
    return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b)) != 0;
}

struct DescriptorPoolSizeDesc {
    DescriptorType type;
    uint32_t count;
};

struct DescriptorPoolDesc {
    std::vector<std::pair<DescriptorType, uint32_t>> poolSizes;
    uint32_t maxSets = 0;
    DescriptorPoolFlags flags = DescriptorPoolFlags::None;
};

enum class IShaderStage : uint32_t {
    None = 0,
    Vertex = 1 << 0,
    Fragment = 1 << 1,
    Compute = 1 << 2,
    Geometry = 1 << 3,
    TessControl = 1 << 4,
    TessEval = 1 << 5,
    AllGraphics = 0x1F,
    All = 0x7FFFFFFF
};

inline IShaderStage operator|(IShaderStage a, IShaderStage b) {
    return static_cast<IShaderStage>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
        );
}

inline IShaderStage& operator|=(IShaderStage& a, IShaderStage b) {
    a = a | b;
    return a;
}

inline bool operator&(IShaderStage a, IShaderStage b) {
    return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b)) != 0;
}

struct DescriptorSetLayoutBindingDesc {
    uint32_t binding = 0;
    DescriptorType type;
    uint32_t count = 1;
    IShaderStage stageFlags;
};

enum class DescriptorSetLayoutFlags : uint32_t {
    NONE = 0,
    PUSH_DESCRIPTOR = 1 << 0,  // VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR
    // add more as needed
};

// Enable bitwise operations
inline DescriptorSetLayoutFlags operator|(DescriptorSetLayoutFlags a, DescriptorSetLayoutFlags b) {
    return static_cast<DescriptorSetLayoutFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

struct DescriptorSetLayoutDesc {
    std::vector<DescriptorSetLayoutBindingDesc> bindings;
    DescriptorSetLayoutFlags flags = DescriptorSetLayoutFlags::NONE;
};



class ICommandList;
class IGraphicsDeviceExplicit : IGraphicsDevice {
public:;
    virtual const char* GetAPIName() const = 0;
    virtual uint32_t GetAPIVersion() const = 0;
    virtual bool IsFeatureSupported(const std::string& feature) const = 0;
    virtual u8 GetFlags() const {
        return GraphicsDeviceFlags::IS_EXPLICIT;
    }

    virtual bool Init() { return false; };

    // Command list lifecycle
    virtual ICommandList* GetCommandList() { return nullptr; }

    // Frame-level
    virtual void BeginFrame() {};
    virtual void EndFrame() {};
    virtual void Present() {};
    virtual void Resize(long width, long height) {};
    virtual ITexture* GetSwapchainImage() { return nullptr; };

    // Resource creation
    virtual IPipelineBuilder* CreatePipelineBuilder() { return nullptr; };
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
    virtual ICommandPool* CreateCommandPool(CommandPoolDesc desc) { return nullptr; };
    virtual IDescriptorPool* CreateDescriptorPool(DescriptorPoolDesc desc) { return nullptr; };
    virtual IDescriptorSetLayout* CreateDescriptorSetLayout(DescriptorSetLayoutDesc desc) { return nullptr; };
    
    // Single Time Commands
    virtual ICommandBuffer* BeginSingleTimeCommands(ICommandPool* pool) { return nullptr; };
    virtual void EndSingleTimeCommands(ICommandBuffer* buffer) {};

    // Buffer Memory Commands
    virtual void CopyToBuffer(IBuffer* buffer, ICommandBuffer* pool, void* data, size_t size) {};
    virtual void MapBufferMemory(IBuffer* buffer, void** dataPtr) {};
    virtual void UnmapBufferMemory(IBuffer* buffer) {};
    virtual void CopyBufferToImage(ICommandBuffer* cBuffer, IBuffer* srcBuffer, ITexture* dstTexture, BufferImageCopyDesc desc) {};
    virtual void CopyImageToImage(ICommandBuffer* cBuffer, ITexture* srcBuffer, ITexture* dstBuffer, ImageCopyDesc desc) {};

    // Resource release
    virtual void ReleaseShader(IShader**) {};
    virtual void ReleaseBuffer(IBuffer**) {};
    virtual void ReleaseTexture(ITexture**) {};
    virtual void ReleaseCommandPool(ICommandPool**) {};
    virtual void ReleaseDescriptorPool(IDescriptorPool**) {};
    virtual void ReleaseDescriptorSetLayout(IDescriptorSetLayout**) {};

    virtual void ReleaseOnPend(void*) {};
    virtual void WaitForNewFrame() {};

    virtual void ClearPendingReleases() {};
    virtual void WaitTillImFree() {};

    // Device access
    virtual IDeviceContext* getContext() { return nullptr; };
    virtual IDevice* getDevice() { return nullptr; };

    // Shutdown
    virtual void Shutdown() {};
};

// Command list interface
class ICommandList {
public:
    virtual ~ICommandList() = default;
    virtual void Release() {};

    // Pipeline
    virtual void BindPipeline(IPipeline* pipeline) {};

    // Descriptor sets (buffers, textures, samplers)
    virtual void BindDescriptorSet(IDescriptorSet* set, uint32_t index = 0) {};

    // Viewport / dynamic states
    virtual void BindViewPort(IViewPort*) {};
    virtual void BindScissor(IViewPort*) {};
    virtual void BindBuffer(IBuffer* buffer) {};

    // Drawing
    virtual void Draw(PrimitiveType type, size_t vertexCount, size_t vertexOffset = 0) {};
    virtual void DrawIndexed(PrimitiveType type, IBuffer* indexBuffer, size_t indexCount, size_t indexOffset = 0) {};

    // Resource copy / transitions
    virtual void CopyToBuffer(IBuffer*, void* data, size_t size) {};
};


struct IPendingDraw {
    IPipeline* pipeline = nullptr;
    IDescriptorSet* ds = nullptr;
    u32 dsIndex;
    IViewPort* viewport = nullptr;
    IViewPort* scissor = nullptr;
    IBuffer* buffer = nullptr;
    PrimitiveType type;
    size_t vertexCount;
    size_t vertexOffset;
    IBuffer* indexBuffer = nullptr  ;
    size_t indexCount;
    size_t indexOffset;
};