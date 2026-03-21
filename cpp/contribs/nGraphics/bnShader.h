#pragma once
#ifdef BORA_USE_SPIRV
#include "ExplicitGraphicsAbstract.h"
#include "3rdparty/bspirv/spirv_cross.hpp"
#include "3rdparty/bspirv/spirv_hlsl.hpp"
#include "3rdparty/bspirv/spirv_glsl.hpp"
#include "3rdparty/bspirv/spirv_msl.hpp"

struct ShaderResourceBinding {
    std::string name;       // "u_MVP", "texDiffuse", etc.
    uint32_t set;           // descriptor set index
    uint32_t binding;       // binding number inside the set
    DescriptorType type;  // UNIFORM_BUFFER, COMBINED_IMAGE_SAMPLER, STORAGE_BUFFER, etc.
    uint32_t count;         // array size (1 if not an array)
    IShaderStage stage;      // which stage(s) use it
};

struct PushConstantRange {
    std::string name;
    uint32_t offset;
    uint32_t size;
    IShaderStage stage;
};


struct ShaderReflection {
    IShaderStage stage;
    std::vector<ShaderResourceBinding> resources;
    std::vector<PushConstantRange> pushConstants;

    struct InputAttribute {
        std::string name;
        uint32_t location;
        TextureFormat format;
    };
    std::vector<InputAttribute> inputs;

    struct OutputAttribute {
        std::string name;
        uint32_t location;
        TextureFormat format;
    };
    std::vector<OutputAttribute> outputs;
};

struct BinaryShaderResult {
    GraphicsChoice type;
    u8* data;
    size_t size;
};

class bnShader
{
public:
    bnShader(IShader* shader);
    bnShader(ShaderDesc::Type type, u8* spvData, size_t size);
    bnShader(ShaderDesc::Type, u32* spvData, size_t size);
    ~bnShader();
public:
    BinaryShaderResult* GetBinary(GraphicsChoice type);
    ShaderReflection getReflection();
private:
    sVec<u32> data;
    ShaderDesc::Type type;
};
#endif

