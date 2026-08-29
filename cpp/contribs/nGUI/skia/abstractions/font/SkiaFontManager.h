// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: SkiaCanvas.h
 * Purpose: ?
*/
#pragma once
#include <include/core/SkCanvas.h>
#include "SkiaFont.h"
#include "SkiaFontManager.h"
#include "SkiaFontTextBlob.h"
#include "bskia/include/core/SkFontMgr.h"
#include "bskia/include/core/SkTypeface.h"
#include "bskia/include/core/SkFontScanner.h"
#include "nGUI/skia/interfaces/font/IGUIFontTypeface.h"
#include "nGUI/skia/interfaces/font/IGUIFontManager.h"
#ifdef WIN32
#include "bskia/include/ports/SkTypeface_win.h"
#elif defined(__APPLE__)
#include "bskia/include/ports/SkFontMgr_mac_ct.h"
#elif defined(__linux__)
#include "bskia/include/ports/SkFontMgr_fontconfig.h"
#include <include/ports/SkFontScanner_FreeType.h>
#include <fontconfig/fontconfig.h>
#endif

class SkiaFontManager : public IGUIFontManager
{
public:
    SkiaFontManager()
    {
       auto logger = LogManager::instance().getLogger("bora.gui.fontmanager");
#ifdef WIN32
        mgr = sk_sp<SkFontMgr>(SkFontMgr_New_DirectWrite());
#elif defined(__APPLE__)
        mgr = SkFontMgr_New_CoreText(nullptr); // todo?
#elif defined(__linux__)
        if (!FcInit()) {
            // Failed to initialize fontconfig
            LOG_ERROR(logger) << "Unable to initalize fontconfig!";
            return;
        }

        std::unique_ptr<SkFontScanner> scanner = SkFontScanner_Make_FreeType();
        mgr = SkFontMgr_New_FontConfig(nullptr, std::move(scanner));
#endif
    }

    ResourceHandle<IGUIFont> getFont(const char* family,
             int size,
             GUIFontStyle style) override
    {
        SkFontStyle skStyle(style.weight(), style.width(), (SkFontStyle::Slant)((u8)style.slant()));
        sk_sp<SkTypeface> tf = sk_sp<SkTypeface>(mgr->matchFamilyStyleCharacter(family, skStyle, nullptr, 0, U'日'));
        if (!tf) {
            // fallback to default system font
            tf = sk_sp<SkTypeface>(mgr->legacyMakeTypeface(nullptr, skStyle));
        }

        ResourceHandle<IGUIFont> font = new SkiaFont(tf, static_cast<SkScalar>(size));
        font->setEdging(IGUIFont::Edging::SubpixelAntiAlias);
        font->setHinting(FontHinting::kFull);

        return font;
    };

    virtual ResourceHandle<IGUIFont>  getFont(u8* data,
           u64 size,
           GUIFontStyle style) override
    {

    };

     ResourceHandle<IGUIFontTypeface>  getFontTypeface(const char* family,
            int size,
            GUIFontStyle style) override
    {

    };

    ResourceHandle<IGUIFontTypeface> getFontTypeface(u8* data,
           u64 size,
           GUIFontStyle style) override
    {

    };

    ResourceHandle<IGUIFontTextBlob> createTextBlob(const void* text, size_t byteLength, const IGUIFont* font, TextEncoding encoding = TextEncoding::kUTF8) override
    {
        return new SkiaFontTextBlob(text, byteLength, font, encoding);
    }

    ResourceHandle<IGUIFontTextBlob> createTextBlob(const char* string, const IGUIFont* font, TextEncoding encoding = TextEncoding::kUTF8) override
    {
        return new SkiaFontTextBlob(string, font, encoding);
    }

    [[nodiscard]] void* GetNativeHandle() const override {
        return static_cast<void*>(mgr.get());
    }

private:
    sk_sp<SkFontMgr> mgr;
};
