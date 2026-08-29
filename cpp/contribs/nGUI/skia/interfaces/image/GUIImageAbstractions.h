// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: GUIImageAbstractions.h
 * Purpose: ?
*/
#pragma once

enum GUIImageAlphaType : int {
    kUnknown_SkAlphaType,                          //!< uninitialized
    kOpaque_SkAlphaType,                           //!< pixel is opaque
    kPremul_SkAlphaType,                           //!< pixel components are premultiplied by alpha
    kUnpremul_SkAlphaType,                         //!< pixel components are independent of alpha
    kLastEnum_SkAlphaType = kUnpremul_SkAlphaType, //!< last valid value
};

enum GUIImageColorType : int {
    // Unknown or unrepresentable as an SkColorType.
    kUnknown_SkColorType,
    // Single channel data (8-bit) interpreted as an alpha value. RGB are 0.
    //   Bits: [A:7..0]
    kAlpha_8_SkColorType,
    // Three channel BGR data (5 bits red, 6 bits green, 5 bits blue) packed into a LE 16-bit word.
    // NOTE: The name of this enum value does not match the standard convention for SkColorType.
    //   Bits: [R:15..11 G:10..5 B:4..0]
    kRGB_565_SkColorType,
    // Four channel ABGR data (4 bits per channel) packed into a LE 16-bit word.
    // NOTE: The name of this enum value does not match the standard convention for SkColorType.
    //   Bits: [R:15..12 G:11..8 B:7..4 A:3..0]
    kARGB_4444_SkColorType,
    // Four channel RGBA data (8 bits per channel) packed into a LE 32-bit word.
    //   Bits: [A:31..24 B:23..16 G:15..8 R:7..0]
    kRGBA_8888_SkColorType,
    // Three channel RGB data (8 bits per channel) packed into a LE 32-bit word. The remaining bits
    // are ignored and alpha is forced to opaque.
    //   Bits: [x:31..24 B:23..16 G:15..8 R:7..0]
    kRGB_888x_SkColorType,
    // Four channel BGRA data (8 bits per channel) packed into a LE 32-bit word. R and B are swapped
    // relative to kRGBA_8888.
    //   Bits: [A:31..24 R:23..16 G:15..8 B:7..0]
    kBGRA_8888_SkColorType,
    // Four channel RGBA data (10 bits per color, 2 bits for alpha) packed into a LE 32-bit word.
    //   Bits: [A:31..30 B:29..20 G:19..10 R:9..0]
    kRGBA_1010102_SkColorType,
    // Four channel BGRA data (10 bits per color, 2 bits for alpha) packed into a LE 32-bit word.
    // R and B are swapped relative to kRGBA_1010102.
    //   Bits: [A:31..30 R:29..20 G:19..10 B:9..0]
    kBGRA_1010102_SkColorType,
    // Three channel RGB data (10 bits per channel) packed into a LE 32-bit word. The remaining bits
    // are ignored and alpha is forced to opaque.
    //   Bits: [x:31..30 B:29..20 G:19..10 R:9..0]
    kRGB_101010x_SkColorType,
    // Three channel BGR data (10 bits per channel) packed into a LE 32-bit word. The remaining bits
    // are ignored and alpha is forced to opaque. R and B are swapped relative to kRGB_101010x.
    //   Bits: [x:31..30 R:29..20 G:19..10 B:9..0]
    kBGR_101010x_SkColorType,
    // Three channel BGR data (10 bits per channel) packed into a LE 32-bit word. The remaining bits
    // are ignored and alpha is forced to opaque. Instead of normalizing [0, 1023] to [0.0, 1.0] the
    // color channels map to an extended range of [-0.752941, 1.25098], compatible with
    // MTLPixelFormatBGR10_XR.
    //   Bits: [x:31..30 R:29..20 G:19..10 B:9..0]
    kBGR_101010x_XR_SkColorType,
    // Four channel BGRA data (10 bits per channel) packed into a LE 64-bit word. Each channel is
    // preceded by 6 bits of padding.  Instead of normalizing [0, 1023] to [0.0, 1.0] the color and
    // alpha channels map to an extended range of [-0.752941, 1.25098], compatible with
    // MTLPixelFormatBGRA10_XR.
    //   Bits: [A:63..54 x:53..48 R:47..38 x:37..32 G:31..22 x:21..16 B:15..6 x:5..0]
    kBGRA_10101010_XR_SkColorType,
    // Four channel RGBA data (10 bits per channel) packed into a LE 64-bit word. Each channel is
    // preceded by 6 bits of padding.
    //   Bits: [A:63..54 x:53..48 B:47..38 x:37..32 G:31..22 x:21..16 R:15..6 x:5..0]
    kRGBA_10x6_SkColorType,
    // Single channel data (8-bit) interpreted as a grayscale value (e.g. replicated to RGB).
    //   Bits: [G:7..0]
    kGray_8_SkColorType,
    // Four channel RGBA data (16-bit half-float per channel) packed into a LE 64-bit word. Values
    // are assumed to be in [0.0,1.0] range, unlike kRGBA_F16.
    //   Bits: [A:63..48 B:47..32 G:31..16 R:15..0]
    kRGBA_F16Norm_SkColorType,
    // Four channel RGBA data (16-bit half-float per channel) packed into a LE 64-bit word.
    // This has extended range compared to kRGBA_F16Norm.
    //   Bits: [A:63..48 B:47..32 G:31..16 R:15..0]
    kRGBA_F16_SkColorType,
    // Three channel RGB data (16-bit half-float per channel) packed into a LE 64-bit word. The last
    // 16 bits are ignored and alpha is forced to opaque.
    //   Bits: [x:63..48 B:47..32 G:31..16 R:15..0]
    kRGB_F16F16F16x_SkColorType,
    // Four channel RGBA data (32-bit float per channel) packed into a LE 128-bit word.
    //   Bits: [A:127..96 B:95..64 G:63..32 R:31..0]
    kRGBA_F32_SkColorType,

    // The following 8 colortypes are just for reading from - not for rendering to

    // Two channel RG data (8 bits per channel). Blue is forced to 0, alpha is forced to opaque.
    //   Bits: [G:15..8 R:7..0]
    kR8G8_unorm_SkColorType,
    // Single channel data (16-bit half-float) interpreted as alpha. RGB are 0.
    //   Bits: [A:15..0]
    kA16_float_SkColorType,
    // Two channel RG data (16-bit half-float per channel) packed into a LE 32-bit word.
    // Blue is forced to 0, alpha is forced to opaque.
    //   Bits: [G:31..16 R:15..0]
    kR16G16_float_SkColorType,
    // Single channel data (16 bits) interpreted as alpha. RGB are 0.
    //   Bits: [A:15..0]
    kA16_unorm_SkColorType,
    // Two channel RG data (16 bits per channel) packed into a LE 32-bit word. B is forced to 0,
    // alpha is forced to opaque.
    //   Bits: [G:31..16 R:15..0]
    kR16G16_unorm_SkColorType,
    // Four channel RGBA data (16 bits per channel) packed into a LE 64-bit word.
    //   Bits: [A:63..48 B:47..32 G:31..16 R:15..0]
    kR16G16B16A16_unorm_SkColorType,
    // Four channel RGBA data (8 bits per channel) packed into a LE 32-bit word. The RGB values are
    // assumed to be encoded with the sRGB transfer function, which can be decoded automatically
    // by GPU hardware with certain texture formats.
    //   Bits: [A:31..24 B:23..16 G:15..8 R:7..0]
    kSRGBA_8888_SkColorType,
    // Single channel data (8 bits) interpreted as red. G and B are forced to 0, alpha is forced to
    // opaque.
    //    Bits: [R:7..0]
    kR8_unorm_SkColorType,

    kLastEnum_SkColorType     = kR8_unorm_SkColorType,
};