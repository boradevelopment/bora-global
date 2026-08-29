#include "bnShader.h"
#ifdef BORA_USE_SPIRV
bnShader::bnShader(IShader* shader)
{
    type = shader->type;
    size_t intCount = shader->ogData.size() / 4;
    data = sVec<u32>(intCount);

    for (size_t i = 0; i < intCount; ++i) {
        data[i] = static_cast<uint32_t>(shader->ogData.data()[i * 4]) |
            (static_cast<uint32_t>(shader->ogData.data()[i * 4 + 1]) << 8) |
            (static_cast<uint32_t>(shader->ogData.data()[i * 4 + 2]) << 16) |
            (static_cast<uint32_t>(shader->ogData.data()[i * 4 + 3]) << 24);
    }
}

bnShader::bnShader(ShaderDesc::Type type, u8* spvData, size_t size) : type(type)
{
    size_t intCount = size / 4;
    data = sVec<u32>(intCount);

    for (size_t i = 0; i < intCount; ++i) {
        data[i] = static_cast<uint32_t>(spvData[i * 4]) |
            (static_cast<uint32_t>(spvData[i * 4 + 1]) << 8) |
            (static_cast<uint32_t>(spvData[i * 4 + 2]) << 16) |
            (static_cast<uint32_t>(spvData[i * 4 + 3]) << 24);
    }
}

bnShader::bnShader(ShaderDesc::Type type, u32* spvData, size_t size) : type(type)
{
	data = sVec<u32>(size);
	memcpy(data.data(), spvData, size);
}

bnShader::~bnShader()
{
}

IShaderStage DetectStage(const spirv_cross::Compiler& compiler) {
    const auto& entryPoints = compiler.get_entry_points_and_stages();
    if (entryPoints.empty()) return IShaderStage::None;

    auto stage = entryPoints[0].execution_model;
    switch (stage) {
    case spv::ExecutionModelVertex:   return IShaderStage::Vertex;
    case spv::ExecutionModelFragment: return IShaderStage::Fragment;
    case spv::ExecutionModelGLCompute:return IShaderStage::Compute;
    case spv::ExecutionModelGeometry: return IShaderStage::Geometry;
    case spv::ExecutionModelTessellationControl: return IShaderStage::TessControl;
    case spv::ExecutionModelTessellationEvaluation: return IShaderStage::TessEval;
    default: return IShaderStage::None;
    }
}

TextureFormat DeduceFormat(const spirv_cross::SPIRType& type) {
    using Base = spirv_cross::SPIRType::BaseType;

    // Scalars and vectors
    if (type.columns == 1) {
        switch (type.basetype) {
        case Base::Float:
            switch (type.vecsize) {
            case 1: return TextureFormat::R32_Float;
            case 2: return TextureFormat::RG32_Float;
            case 3: return TextureFormat::RGBA32_Float; // pack RGB into RGBA32
            case 4: return TextureFormat::RGBA32_Float;
            }
            break;

        case Base::Int:
        case Base::UInt:
            switch (type.vecsize) {
            case 1: return TextureFormat::R16_UNorm; // or R32_UINT if you want
            case 2: return TextureFormat::RG32_Float; // not perfect, but map to closest
            case 3:
            case 4: return TextureFormat::RGBA32_Float;
            }
            break;

        case Base::Boolean:
            return TextureFormat::R16_UNorm; // store as 16-bit UNORM internally
        }
    }

    // Matrices: treat as array of vectors
    if (type.columns > 1) {
        // Example: mat4 -> 4x vec4 -> RGBA32_Float
        return TextureFormat::RGBA32_Float;
    }

    return TextureFormat::Unknown;
}

BinaryShaderResult* bnShader::GetBinary(GraphicsChoice type)
{
    auto result = new BinaryShaderResult();
    result->type = type;

    switch (result->type)
    {
    case GraphicsChoice::VULKAN: {
        result->data = reinterpret_cast<u8*>(data.data());
        result->size = data.size();
        break;
    }
    case GraphicsChoice::D3D11: {
#ifdef WIN32
        spirv_cross::CompilerHLSL compiler(data);
        spirv_cross::CompilerHLSL::Options options;

        options.shader_model = 50;
        compiler.set_hlsl_options(options);
        std::string source = compiler.compile();
#endif
        break;
    }

    case GraphicsChoice::METAL: {
        spirv_cross::CompilerMSL compiler(data);
        std::string source = compiler.compile();
#ifdef __APPLE__
        // call .mm helper which writes to result.data and result.size
        // bnShaderMTLCompile(&result.data, &result.size);
#endif
        break;
    }
    case GraphicsChoice::OPENGL: {
        spirv_cross::CompilerGLSL compiler(data);
        std::string source = compiler.compile();
        // add opengl compile code here.
        break;
    }
    }

    return result;
}

ShaderReflection bnShader::getReflection()
{
    spirv_cross::Compiler comp(data);
    spirv_cross::ShaderResources res = comp.get_shader_resources();

    // Detect shader stage
    ShaderReflection refl;
    refl.stage = DetectStage(comp); 

    // Uniform buffers
    for (auto& ub : res.uniform_buffers) {
        auto set = comp.get_decoration(ub.id, spv::DecorationDescriptorSet);
        auto binding = comp.get_decoration(ub.id, spv::DecorationBinding);
        auto type = comp.get_type(ub.base_type_id);
        auto size = comp.get_declared_struct_size(type);

        ShaderResourceBinding b{};
        b.name = comp.get_name(ub.id);
        b.set = set;
        b.binding = binding;
        b.type = DescriptorType::UniformBuffer;
        if (!type.array.empty())
            b.count = type.array[0] == 0 ? 1 : type.array[0];
        else
            b.count = 1;

        b.stage = refl.stage;

        refl.resources.push_back(b);
    }

    // Sampled images
    for (auto& si : res.sampled_images) {
        auto set = comp.get_decoration(si.id, spv::DecorationDescriptorSet);
        auto binding = comp.get_decoration(si.id, spv::DecorationBinding);

        ShaderResourceBinding b{};
        b.name = comp.get_name(si.id);
        b.set = set;
        b.binding = binding;
        b.type = DescriptorType::CombinedImageSampler;
        b.count = 1;
        b.stage = refl.stage;

        refl.resources.push_back(b);
    }

    // Push constants
    for (auto& pc : res.push_constant_buffers) {
        auto type = comp.get_type(pc.base_type_id);
        uint32_t size = comp.get_declared_struct_size(type);

        PushConstantRange r{};
        r.name = comp.get_name(pc.id);
        r.offset = 0;
        r.size = size;
        r.stage = refl.stage;

        refl.pushConstants.push_back(r);
    }

    // Vertex inputs
    if (refl.stage == IShaderStage::Vertex) {
        for (auto& in : res.stage_inputs) {
            ShaderReflection::InputAttribute attr{};
            attr.name = comp.get_name(in.id);
            attr.location = comp.get_decoration(in.id, spv::DecorationLocation);

            auto type = comp.get_type(in.type_id);
            attr.format = DeduceFormat(type); // you�ll need a helper
            refl.inputs.push_back(attr);
        }
    }

    return refl;
}
#endif