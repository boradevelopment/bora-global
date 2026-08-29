// BORA_INCLUDED_IN_SDK
// ^^^ Never remove this! This indicates that this should be included in production builds of libbora
// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.
/* 
 * FileName: GraphicsCommands.h
 * Purpose: All graphical command structures that need to be shared with the SDK
 */
#pragma once
#include "nGraphics/ImmediateGraphicsAbstract.h"
#include "nGraphics/ExplicitGraphicsAbstract.h"

#pragma pack(push, 1)
struct CreateTextureCommandData
{
    TextureDesc desc;
    const void* initialData;
    ResourceHandle<ITexture>* handle;
};
#pragma pack(pop)

