// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: SkiaSvgDom.h
 * Purpose: ?
*/
#pragma once
#include <include/core/SkStream.h>
#include <modules/svg/include/SkSVGDOM.h>

#include "nGUI/skia/interfaces/svg/IGUISVGDOM.h"

class GUISVGDOM : public IGUISVGDOM {
public:
    GUISVGDOM(u8* data, size_t size) {
        // Initialization logic
        SkStream stream;
        stream.
        SkSVGDOM::MakeFromStream(SkStream(&data[0]), size);
    }

};

// Factory implementation
ResourceHandle<IGUISVGDOM> IGUISVGDOM::CreateFromBinary(u8* data, size_t size) {
    return new GUISVGDOM(data, size);
}