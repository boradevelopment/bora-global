// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: IGUISVG.h
 * Purpose: ?
*/
#pragma once
#include "GUIAbstractions.h"
#include "IGUICanvas.h"
#include "TypeDefinitions.h"
#include "nCommon/Resources.h"

struct IGUISize;
class IGUICanvas;

class IGUISVGDOM {
public:
    virtual ~IGUISVGDOM() = default;
    static ResourceHandle<IGUISVGDOM> CreateFromBinary(u8* data, size_t size);

    // Native handle access
    [[nodiscard]] virtual void* GetNativeHandle() const = 0;

    // IGUISVGSVG* getRoot() const = 0;

    virtual void setContainerSize(const IGUISize&) = 0;
    [[nodiscard]] virtual const IGUISize& containerSize() const = 0;

    // Returns the node with the given id, or nullptr if not found.
    // ResourceHandle<IGUISVGNode>* findNodeById(const char* id);

    virtual void render(IGUICanvas*) const = 0;

    /** Render the node with the given id as if it were the only child of the root. */
    // void renderNode(IGUICanvas*, SkSVGPresentationContext&, const char* id) const;
};
