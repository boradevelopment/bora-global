// BORA_INCLUDED_IN_SDK
// ^^^ Never remove this! This indicates that this should be included in production builds of libbora
#pragma once
#include "nCommon/Resources.h"

#if defined(Always)
  #undef Always
#endif

#if defined(None)
  #undef None
#endif

typedef struct Extent2D {
    uint32_t    width;
    uint32_t    height;
} Extent2D;

typedef struct Extent3D {
    uint32_t    width;
    uint32_t    height;
    uint32_t    depth;
} Extent3D;

typedef struct Offset2D {
    int32_t    x;
    int32_t    y;
} Offset2D;

typedef struct Offset3D {
    int32_t    x;
    int32_t    y;
    int32_t    z;
} Offset3D;

typedef struct Rect2D {
    Offset2D    offset;
    Extent2D    extent;
} Rect2D;


enum class TextureFormat {
    Unknown,

    // 8-bit per channel formats
    RGBA8_UNorm,
    RGBA8_UNorm_SRGB,
    BGRA8_UNorm,
    BGRA8_UNorm_SRGB,

    // 16-bit float formats
    RGBA16_Float,
    R16_Float,
    R16_UNorm,

    // 32-bit float formats
    R32_Float,
    RG32_Float,
    RGBA32_Float,

    // Depth/Stencil formats
    D24_UNorm_S8_UInt,
    D32_Float,
    D32_Float_S8X24_UInt,

};

enum class TextureDimensions {
    Dim1,
    Dim2,
    Dim3,
};

struct TextureDesc {
    uint32_t width;
    uint32_t height;
    uint32_t widthBytes;
    uint32_t mipLevels = 1;
    int samples = 1;
    bool isRenderTarget = false;
    bool isDepthStencil = false;
    bool CpuAccessWrite = false;
    bool Dynamic = false;
    TextureFormat format;
    TextureDimensions dimension = TextureDimensions::Dim2;
    u32 depth = 1;
    std::string debugName;
    u32 slot = 0;
};

// Layout abstraction
enum class ImageLayout : uint8_t {
    Undefined,
    GenericRead,
    RenderTarget,
    DepthStencil,
    ShaderRead,
    Present,
    CopySrc,
    CopyDst
};

class ITexture {
public:
    virtual ~ITexture() = default;

    virtual void* GetNativeHandle() = 0;
    virtual void Release() {}
    u32 slot = 0;
    bool owner = true;
    TextureDesc desc;
    ImageLayout explicitLayout;
};

// todo: sort this shit out
struct ShaderDesc {
    enum class Type {
        Vertex,       // VS
        Pixel,        // FS / PS
        Compute,      // CS
        Geometry,     // GS
        TessControl,  // TCS / Hull Shader
        TessEval,     // TES / Domain Shader
        RayGen,       // Ray tracing stages (DXR / Vulkan RT)
        ClosestHit,
        AnyHit,
        Miss,
        Intersection,
        Callable
    };
    ShaderDesc::Type type;
    const char* entryName = "main";
    u8* bytecode;
    long bytecodeSize;
    bool releaseByteCodeOnceInitalized = false;
    sVec<u8> ogData;
    std::string debugName = "";
};

class IShader {
public:
    virtual ~IShader() = default;
    virtual void* GetNativeHandle() = 0;
    virtual void Release() {}
    sVec<u8> ogData; //SPV
    sVec<u8> binary; //ACTUAL DATA
    ShaderDesc::Type type;
};

enum class VertexAttribType {
    Float,
    Float2,
    Float3,
    Float4,
    UInt,
    UInt2,
    UInt3,
    UInt4,
    // Add more as needed
};

enum class VertexAttribUsage {
    Position,
    Normal,
    TexCoord,
    Color,
    Tangent,
    Bitangent,
    // Add more if needed
};


struct VertexAttribDesc {
    uint32_t semanticIndex = 0;     // for TEXCOORD0, TEXCOORD1 etc
    VertexAttribType type;          // Float3, Float2, etc
    uint32_t inputSlot = 0;         // which vertex buffer slot
    size_t offset = 0;              // offsetof in struct
    bool perInstance = false;       // true = per-instance data
    std::string semanticName = "TEXCOORD"; // Spir-V uses texcoords, change if needed
};

struct InputLayoutDesc {
    std::vector<VertexAttribDesc> elements;
    uint32_t stride = 0;
    ResourceHandle<IShader>* vs;
};


class IInputLayout {
public:
    virtual void* GetNativeHandle() = 0;
    virtual void Release() {}
    InputLayoutDesc desc;
    virtual ~IInputLayout() = default;
};


class ISamplerState {
public:
    virtual void* GetNativeHandle() = 0;
    virtual void Release() {}
    virtual ~ISamplerState() = default;
};

class IViewPort {
public:
    virtual void* GetNativeHandle() = 0;
    virtual void Release() {}
    virtual ~IViewPort() = default;
};

enum class TextureFilter
{
    Nearest,
    Linear,
    Anisotropic
};

enum class TextureAddressMode
{
    Wrap,
    Mirror,
    Clamp,
    Border
};

enum class ComparisonFunc
{
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

struct SamplerStateDesc
{
    uint slot = 0;
    TextureFilter       minFilter = TextureFilter::Linear;   // Minification
    TextureFilter       magFilter = TextureFilter::Linear;   // Magnification
    TextureFilter       mipFilter = TextureFilter::Linear;   // Mipmap filtering

    TextureAddressMode  addressU = TextureAddressMode::Wrap; // U axis wrap mode
    TextureAddressMode  addressV = TextureAddressMode::Wrap; // V axis wrap mode
    TextureAddressMode  addressW = TextureAddressMode::Wrap; // W axis wrap mode

    float               mipLODBias = 0.0f;  // Mipmap LOD bias
    uint32_t            maxAnisotropy = 1;  // For anisotropic filtering

    ComparisonFunc      comparisonFunc = ComparisonFunc::Never; // For depth comparison samplers

    float               borderColor[4] = { 0, 0, 0, 0 }; // RGBA for border mode

    float               minLOD = 0.0f;  // Minimum LOD clamp
    float               maxLOD = FLT_MAX; // Maximum LOD clamp
};


enum class BufferType {
    Vertex,
    Index,
    Constant,
    Staging,
    Storage
};

class IBuffer {
public:
    virtual void* GetNativeHandle() = 0;
    virtual void Release() {}
    BufferType type;
    u32 slot = 0;
    virtual ~IBuffer() = default;
};

struct BufferDesc {
    size_t size = 0;
    u8 byteWidth = 0;
    u8 stride = 0;
    bool dynamic;
    BufferType type;
    TextureFormat format;
    u32 slot = 0;
};

enum class PrimitiveType {
    Triangles,
    TriangleStrip,
    Lines,
    LineStrip,
    Points,
    Quads
};


struct ViewPort {
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
};

struct ViewPortDesc {
    ViewPort* viewport;
};


class IDevice {
public:
    virtual void* GetNativeHandle() = 0;
    virtual void Release() {}
    virtual ~IDevice() = default;
};

class IDeviceContext {
public:
    virtual void* GetNativeHandle() = 0;
    virtual void Release() {}
    virtual ~IDeviceContext() = default;
};

class IRasterizerState {
public:
    virtual void* GetNativeHandle() = 0;
    virtual void Release() {}
    virtual ~IRasterizerState() = default;
};

class IDepthStencilState {
public:
    virtual ~IDepthStencilState() = default;

    virtual void* GetNativeHandle() = 0;
    virtual void Release() {}
};

class IBlendState {
public:
    virtual ~IBlendState() = default;
    virtual void* GetNativeHandle() = 0;
    virtual void Release() {}
};

enum class IFillMode {
    Solid,
    Wireframe
};

enum class CullMode {
    None,
    Front,
    Back
};

struct RasterizerDesc {
    IFillMode fillMode = IFillMode::Solid;
    CullMode cullMode = CullMode::Back;
    bool frontCounterClockwise = false;

    int depthBias = 0;
    float depthBiasClamp = 0.0f;
    float slopeScaledDepthBias = 0.0f;

    bool depthClipEnable = true;
    bool scissorEnable = false;
    bool multisampleEnable = false;
    bool antialiasedLineEnable = false;
};

enum class StencilOp {
    Keep,
    Zero,
    Replace,
    IncrSat,
    DecrSat,
    Invert,
    Incr,
    Decr
};

struct StencilOpDesc {
    StencilOp failOp = StencilOp::Keep;
    StencilOp depthFailOp = StencilOp::Keep;
    StencilOp passOp = StencilOp::Keep;
    ComparisonFunc func = ComparisonFunc::Always;
};

struct DepthStencilDesc {
    bool depthEnable = true;
    bool depthWriteMask = true;  // true = ALL, false = ZERO
    ComparisonFunc depthFunc = ComparisonFunc::Less;

    bool stencilEnable = false;
    uint8_t stencilReadMask = 0xFF;
    uint8_t stencilWriteMask = 0xFF;

    StencilOpDesc frontFace; // operations if front-facing
    StencilOpDesc backFace;  // operations if back-facing
};

enum class Blend {
    Zero,
    One,
    SrcColor,
    InvSrcColor,
    SrcAlpha,
    InvSrcAlpha,
    DestAlpha,
    InvDestAlpha,
    DestColor,
    InvDestColor,
    SrcAlphaSat,
    BlendFactor,
    InvBlendFactor,
    Src1Color,
    InvSrc1Color,
    Src1Alpha,
    InvSrc1Alpha
};

enum class BlendOp {
    Add,
    Subtract,
    RevSubtract,
    Min,
    Max
};

struct RenderTargetBlendDesc {
    bool blendEnable = false;

    Blend srcBlend = Blend::One;
    Blend destBlend = Blend::Zero;
    BlendOp blendOp = BlendOp::Add;

    Blend srcBlendAlpha = Blend::One;
    Blend destBlendAlpha = Blend::Zero;
    BlendOp blendOpAlpha = BlendOp::Add;

    uint8_t renderTargetWriteMask = 0x0F; // RGBA
};

struct BlendStateDesc {
    bool alphaToCoverageEnable = false;
    bool independentBlendEnable = false;

    RenderTargetBlendDesc renderTarget[8]; // up to 8 RTs
};

class IDepthStencil {
public:
    virtual ~IDepthStencil() = default;
    virtual void* GetNativeHandle() = 0;
    virtual void Release() {}
};

enum class ImageUsage : uint32_t {
    None = 0,
    ColorAttachment = 1 << 0,
    DepthStencil = 1 << 1,
    Sampled = 1 << 2,
    Storage = 1 << 3,
    TransferSrc = 1 << 4,
    TransferDst = 1 << 5,
    InputAttachment = 1 << 6,
    Presentable = 1 << 7
};

inline ImageUsage operator|(ImageUsage a, ImageUsage b) {
    return static_cast<ImageUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline ImageUsage& operator|=(ImageUsage& a, ImageUsage b) {
    a = a | b;
    return a;
}

inline ImageUsage operator&(ImageUsage a, ImageUsage b) {
    return static_cast<ImageUsage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline bool operator!=(ImageUsage a, ImageUsage b) {
    return static_cast<uint32_t>(a) != static_cast<uint32_t>(b);
}


enum class ImageAccessLayout : uint32_t {
    None = 0,
    Read = 1 << 0,           // shader read, texture sampling
    Write = 1 << 1,          // render target or UAV write
    DepthRead = 1 << 2,      // depth buffer read
    DepthWrite = 1 << 3,     // depth buffer write
    CopySrc = 1 << 4,        // copy source
    CopyDst = 1 << 5,        // copy destination
    Present = 1 << 6         // ready to present (swapchain)
};

inline ImageAccessLayout operator|(ImageAccessLayout a, ImageAccessLayout b) {
    return static_cast<ImageAccessLayout>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline ImageAccessLayout& operator|=(ImageAccessLayout& a, ImageAccessLayout b) {
    a = a | b;
    return a;
}

inline ImageAccessLayout operator&(ImageAccessLayout a, ImageAccessLayout b) {
    return static_cast<ImageAccessLayout>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline bool operator!=(ImageAccessLayout a, ImageAccessLayout b) {
    return static_cast<uint32_t>(a) != static_cast<uint32_t>(b);
}


struct RenderTargetDesc {
    std::vector<ITexture*> colorTargets;
    IDepthStencil* depth = nullptr;

    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t mipLevels = 1;

    bool makeFramebuffer = true;

    // Abstracted usage & layout
    std::vector<ImageUsage> colorUsage;    // one per color target
    std::vector<ImageLayout> colorLayout;  // one per color target
    ImageUsage depthUsage = ImageUsage::DepthStencil;
    ImageLayout depthLayout = ImageLayout::DepthStencil;

    std::vector<rgba> clearColors; // one per color
    float depthClear = 1.0f;
    uint32_t stencilClear = 0;
};



class IRenderTarget {
public:
    virtual ~IRenderTarget() = default;
    virtual void* GetNativeHandle() = 0;
    virtual void Release() {}
    RenderTargetDesc desc;
};


enum GraphicsDeviceFlags : u8 {
    IS_EXPLICIT = 1 << 0, // 0000 0001
    IS_IMMEDIATE = 1 << 1, // 0000 0010
};

enum GraphicsMode {
    IMMEDIATE = 0,
    EXPLICIT = 1,
};

enum GraphicsChoice  {
    NONE = -1,
    D3D11 = 0,
    VULKAN = 1,
    OPENGL = 2,
    D3D12 = 3,
    METAL = 4,
  
};

inline int GetFallbackChoice(GraphicsChoice choice) {
#if defined(_WIN32)
    // Order of preference for Windows
    switch (choice) {
    case GraphicsChoice::D3D12: return 0;
    case GraphicsChoice::VULKAN: return 1;
    case GraphicsChoice::D3D11: return 2;
    case GraphicsChoice::OPENGL: return 3;
    default: return -1;
    }
#elif defined(__APPLE__)
    // macOS / iOS
    switch (choice) {
    case 0: return GraphicsChoice::METAL;
    case 1: return GraphicsChoice::VULKAN; // MoltenVK fallback
    case 2: return GraphicsChoice::OPENGL;
    default: return GraphicsChoice::NONE;
    }
#elif defined(__ANDROID__)
    // Android (default GLES)
    switch (fallback) {
    case 0: return GraphicsChoice::OPENGL; // maps to GLES
    case 1: return GraphicsChoice::VULKAN; // if supported
    default: return GraphicsChoice::NONE;
    }
#elif defined(__linux__)
    // Linux
    switch (choice) {
    case 0: return GraphicsChoice::VULKAN;
    case 1: return GraphicsChoice::OPENGL;
    default: return GraphicsChoice::NONE;
    }
#else
    return GraphicsChoice::NONE;
#endif
}

inline GraphicsChoice GetBestGraphicsChoice(int fallback = 0, bool isExplicit = true)
{
#if defined(_WIN32)
    // Order of preference for Windows
    switch (fallback) {
    case 0: return isExplicit ? GraphicsChoice::D3D12 : GraphicsChoice::D3D11;
    case 1: return isExplicit ? GraphicsChoice::VULKAN : GraphicsChoice::OPENGL;
    case 2: return isExplicit ? GraphicsChoice::D3D11 : GraphicsChoice::D3D12;
    case 3: return isExplicit ? GraphicsChoice::OPENGL : GraphicsChoice::VULKAN;
    default: return GraphicsChoice::NONE;
    }
#elif defined(__APPLE__)
    // macOS / iOS
    switch (fallback) {
    case 0: return GraphicsChoice::METAL;
    case 1: return GraphicsChoice::VULKAN; // MoltenVK fallback
    default: return GraphicsChoice::NONE;
    }
#elif defined(__ANDROID__)
    // Android (default GLES)
    switch (fallback) {
    case 0: return GraphicsChoice::OPENGL; // maps to GLES
    case 1: return GraphicsChoice::VULKAN; // if supported
    default: return GraphicsChoice::NONE;
    }
#elif defined(__linux__)
    // Linux
    switch (fallback) {
    case 0: return GraphicsChoice::VULKAN;
    case 1: return GraphicsChoice::OPENGL;
    default: return GraphicsChoice::NONE;
    }
#else
    return fallback == 0 ? GraphicsChoice::OPENGL : GraphicsChoice::NONE;
#endif
}

// bool function that checks if choice is explicit or immediate
inline bool IsGraphicsChoiceExplicit(GraphicsChoice choice)
{
    switch (choice)
    {
    case GraphicsChoice::VULKAN:
        return true;
    case GraphicsChoice::D3D12:
        return true;
    case GraphicsChoice::METAL:
        return true;    // Explicit APIs
    case GraphicsChoice::D3D11:
        return false;
    case GraphicsChoice::OPENGL:
        return false;   // Immediate / legacy APIs
    default:
        return false;
    }
}

inline bool RequiresNewHandle(GraphicsChoice choice) {
    switch (choice)
    {
    case GraphicsChoice::VULKAN:
        return false;
    case GraphicsChoice::D3D12:
        return true;
    case GraphicsChoice::METAL:
        return true;
    case GraphicsChoice::D3D11:
        return true;
    case GraphicsChoice::OPENGL:
        return false;   
    default:
        return false;
    }   
}

inline bool IsMicrosoftShaderSupport(GraphicsChoice choice) {
    switch (choice)
    {
    case GraphicsChoice::VULKAN:
        return false;
    case GraphicsChoice::D3D12:
        return true;
    case GraphicsChoice::METAL:
        return false;
    case GraphicsChoice::D3D11:
        return true;
    case GraphicsChoice::OPENGL:
        return false;
    default:
        return false;
    }
}


enum class VerticesClipSpace : u8 {
    VULKAN,
    D3D,
};
struct IGraphicsDeviceConfig {
    int aliasLevel = 1;
    bool enableMSAA = false;
    int msaaSamples = 1;
    rgba clearColor = { 0, 0, 0, 1 };
    u32 framesInFlight = 4;
    bool enableValidation = true;
    bool vsync = false;
    bool enableDepth = false;
    VerticesClipSpace clipSpace = VerticesClipSpace::VULKAN; // Must pick either one
};

typedef enum ImageAspectFlagBits {
    IMAGE_ASPECT_COLOR_BIT = 0x00000001,
    IMAGE_ASPECT_DEPTH_BIT = 0x00000002,
    IMAGE_ASPECT_STENCIL_BIT = 0x00000004,
    IMAGE_ASPECT_METADATA_BIT = 0x00000008,
    IMAGE_ASPECT_PLANE_0_BIT = 0x00000010,
    IMAGE_ASPECT_PLANE_1_BIT = 0x00000020,
    IMAGE_ASPECT_PLANE_2_BIT = 0x00000040,
    IMAGE_ASPECT_NONE = 0,
    IMAGE_ASPECT_MEMORY_PLANE_0_BIT_EXT = 0x00000080,
    IMAGE_ASPECT_MEMORY_PLANE_1_BIT_EXT = 0x00000100,
    IMAGE_ASPECT_MEMORY_PLANE_2_BIT_EXT = 0x00000200,
    IMAGE_ASPECT_MEMORY_PLANE_3_BIT_EXT = 0x00000400,
    IMAGE_ASPECT_PLANE_0_BIT_KHR = IMAGE_ASPECT_PLANE_0_BIT,
    IMAGE_ASPECT_PLANE_1_BIT_KHR = IMAGE_ASPECT_PLANE_1_BIT,
    IMAGE_ASPECT_PLANE_2_BIT_KHR = IMAGE_ASPECT_PLANE_2_BIT,
    IMAGE_ASPECT_NONE_KHR = IMAGE_ASPECT_NONE,
    IMAGE_ASPECT_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
} ImageAspectFlagBits;

typedef u32 ImageAspectFlags;

typedef struct ImageSubresourceLayers {
    ImageAspectFlags    aspectMask;
    u32              mipLevel;
    u32              baseArrayLayer;
    u32              layerCount;
} ImageSubresourceLayers;



typedef struct ImageBlitDesc {
    ImageSubresourceLayers    srcSubresource;
    Offset3D                  srcOffsets[2];
    ImageSubresourceLayers    dstSubresource;
    Offset3D                  dstOffsets[2];
} ImageBlitDesc;

typedef struct ImageCopyDesc {
    ImageSubresourceLayers    srcSubresource;
    Offset3D                  srcOffset;
    ImageSubresourceLayers    dstSubresource;
    Offset3D                  dstOffset;
    Extent3D                  extent;
} ImageCopyDesc;

typedef struct ImageResolveDesc {
    ImageSubresourceLayers    srcSubresource;
    Offset3D                  srcOffset;
    ImageSubresourceLayers    dstSubresource;
    Offset3D                  dstOffset;
    Extent3D                  extent;
} ImageResolveDesc;

struct BufferImageCopyDesc {
    uint64_t                    bufferOffset;
    uint32_t                    bufferRowLength;
    uint32_t                    bufferImageHeight;
    ImageSubresourceLayers    imageSubresource;
    Offset3D                  imageOffset;
    Extent3D                  imageExtent;
};

class IGraphicsDevice {
public:
    virtual ~IGraphicsDevice() = default;

    virtual const char* GetAPIName() const = 0;
    virtual uint32_t GetAPIVersion() const = 0;
    virtual bool IsFeatureSupported(const std::string& feature) const = 0;
    virtual u8 GetFlags() const = 0;
    virtual bool Init() = 0;
    virtual void Shutdown() = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Present() = 0;
    virtual void Resize(long width, long height) = 0;
    virtual ITexture* GetSwapchainImage() { return nullptr; };

    virtual ITexture* CreateTexture(const TextureDesc& desc, const void* initialData = nullptr) = 0;
    virtual IShader* CreateShader(const ShaderDesc& desc) = 0;
    virtual IBuffer* CreateBuffer(const BufferDesc& desc, const void* data = nullptr) = 0;
    virtual IInputLayout* CreateInputLayout(const InputLayoutDesc& desc) = 0;
    virtual ISamplerState* CreateSamplerState(const SamplerStateDesc& desc) = 0;
    virtual IViewPort* CreateViewPort(const ViewPortDesc& desc) = 0;
    virtual IRasterizerState* CreateRasterizerState(const RasterizerDesc& desc) = 0;
    virtual IDepthStencilState* CreateDepthStencilState(const DepthStencilDesc& desc) = 0;
    virtual IBlendState* CreateBlendState(const BlendStateDesc& desc) = 0;
    virtual IRenderTarget* CreateRenderTarget(const RenderTargetDesc& desc) = 0;
    virtual IDepthStencil* CreateDepthStencil(ITexture* texture) = 0;

    virtual void MapBufferMemory(IBuffer* buffer, void** dataPtr) {};
    virtual void UnmapBufferMemory(IBuffer* buffer) {};

    virtual void ReleaseShader(IShader**) = 0;
    virtual void ReleaseBuffer(IBuffer**) = 0;
    virtual void ReleaseTexture(ITexture**) = 0;

    virtual IDeviceContext* getContext() = 0;
    virtual IDevice* getDevice() = 0;

    // DEBUGGING
    virtual void PushGroup(const char* name, uint32_t color = 0xFFFFFFFF) = 0;

    virtual void PopGroup() = 0;

    virtual void SetMarker(const char* name, uint32_t color = 0xFFFFFFFF) = 0;
};
