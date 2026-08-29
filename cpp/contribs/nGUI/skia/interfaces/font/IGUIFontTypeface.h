// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: IGUIFontTypeface.h
 * Purpose: Font typeface
*/
#pragma once
#include "nGUI/skia/interfaces/GUIAbstractions.h"
#include "nCommon/Resources.h"
#include "nWindow/bnWindowAbstracts.h"

class IGUIFontTypeface {
public:
    virtual ~IGUIFontTypeface() = default;


    // Native Escape Hatch (for backend-specific extensions)
    [[nodiscard]] virtual void* GetNativeHandle() const = 0;
};
