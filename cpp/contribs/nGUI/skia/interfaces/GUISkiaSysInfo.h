// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.
/* 
 * FileName: GUISysInfo.h
 * Purpose: Includes important information for UI classes to use globally
 */
#pragma once
#ifdef WIN32
#include <d3d11.h>
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/d3d/GrD3DBackendContext.h"
typedef GrDirectContext GaneshGPUContextType;
#endif
#include <include/gpu/ganesh/vk/GrVkDirectContext.h>
#include "include/gpu/ganesh/SkSurfaceGanesh.h"