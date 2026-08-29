// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.
/* 
 * FileName: GUISysInfo.h
 * Purpose: Includes important information for UI classes to use globally
 */
#pragma once
#ifdef BORA_UI_SUPPORT
#include "include/gpu/ganesh/GrDirectContext.h"
#ifdef WIN32
#include <d3d11.h>
#include "include/gpu/ganesh/d3d/GrD3DBackendContext.h"
#include <include/gpu/ganesh/gl/win/GrGLMakeWinInterface.h>
typedef GrDirectContext GaneshGPUContextType;
#elif defined(__APPLE__)
#import <Metal/Metal.h>
#include "include/gpu/ganesh/mtl/GrMtlDirectContext.h"
#include "include/gpu/ganesh/mtl/GrMtlBackendContext.h"
#include "include/gpu/ganesh/mtl/GrMtlBackendSurface.h"
#include <include/gpu/ganesh/gl/mac/GrGLMakeMacInterface.h>
typedef GrDirectContext GaneshGPUContextType;
#endif
#include <include/gpu/ganesh/vk/GrVkDirectContext.h>
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#endif