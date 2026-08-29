// Apart of the BORA Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
/*
 * FileName: GUIAbstractions.h
 * Purpose: ?
*/
#pragma once

struct Vec2 { float x, y; };
struct Color4f
{
    float red = 0, green = 0, blue = 0;
    float alpha = 1;
};
struct GUIPaint {
    Color4f color{1.0f, 1.0f, 1.0f, 1.0f};
    float strokeWidth{1.0f};
    bool isStroke{false};
    bool isAntiAlias{true};
};

typedef struct UXRect {
    float left = 0;
    float top = 0;
    float right = 0;
    float bottom = 0;

    [[nodiscard]] static constexpr UXRect FromXYWH(float x, float y, float width, float height) {
        return UXRect{x, y, x + width, y + height};
    }
    [[nodiscard]] constexpr float GetWidth() const { return right - left; }
    [[nodiscard]] constexpr float GetHeight() const { return bottom - top; }
    [[nodiscard]] constexpr bool IsEmpty() const {
        return GetWidth() <= 0 || GetHeight() <= 0;
    }

    [[nodiscard]] constexpr std::tuple<float, float, float, float> AsXYWH() const {
        return {left, top, GetWidth(), GetHeight()};
    }

    constexpr void FromXYWH(float& x, float& y, float& w, float& h) const {
        x = left;
        y = top;
        w = GetWidth();
        h = GetHeight();
    }
} UXRect;

class GUIFontStyle
{
public:
    enum Weight {
        Invisible_Weight   =    0,
        Thin_Weight        =  100,
        ExtraLight_Weight  =  200,
        Light_Weight       =  300,
        Normal_Weight      =  400,
        Medium_Weight      =  500,
        SemiBold_Weight    =  600,
        Bold_Weight        =  700,
        ExtraBold_Weight   =  800,
        Black_Weight       =  900,
        ExtraBlack_Weight  = 1000,
    };

    enum Width {
        UltraCondensed_Width   = 1,
        ExtraCondensed_Width   = 2,
        Condensed_Width        = 3,
        SemiCondensed_Width    = 4,
        Normal_Width           = 5,
        SemiExpanded_Width     = 6,
        Expanded_Width         = 7,
        ExtraExpanded_Width    = 8,
        UltraExpanded_Width    = 9,
    };

    enum Slant : u8 {
        Upright_Slant,
        Italic_Slant,
        Oblique_Slant,
    };

    constexpr GUIFontStyle(int weight, int width, Slant slant) : fValue(
       std::max((int)Invisible_Weight, std::min((int)weight, (int)ExtraBlack_Weight)) +
       (std::max((int)UltraCondensed_Width, std::min((int)width, (int)UltraExpanded_Width)) << 16) +
       (std::max((int)Upright_Slant, std::min((int)slant, (int)Oblique_Slant)) << 24)
    ) { }

    constexpr GUIFontStyle() : GUIFontStyle{Normal_Weight, Normal_Width, Upright_Slant} { }

    [[nodiscard]] int32_t getValue() const
    {
        return fValue;
    }

    [[nodiscard]] int weight() const { return fValue & 0xFFFF; }
    [[nodiscard]] int width() const { return (fValue >> 16) & 0xFF; }
    [[nodiscard]] Slant slant() const { return (Slant)((fValue >> 24) & 0xFF); }

    static constexpr GUIFontStyle Normal() {
        return {Normal_Weight, Normal_Width, Upright_Slant};
    }
    static constexpr GUIFontStyle Bold() {
        return {Bold_Weight,   Normal_Width, Upright_Slant};
    }
    static constexpr GUIFontStyle Italic() {
        return {Normal_Weight, Normal_Width, Italic_Slant };
    }
    static constexpr GUIFontStyle BoldItalic() {
        return {Bold_Weight,   Normal_Width, Italic_Slant };
    }

private:
    int32_t fValue;
};

enum class GUITileMode {
    /**
     *  Replicate the edge color if the shader draws outside of its
     *  original bounds.
     */
    kClamp,

    /**
     *  Repeat the shader's image horizontally and vertically.
     */
    kRepeat,

    /**
     *  Repeat the shader's image horizontally and vertically, alternating
     *  mirror images so that adjacent images always seam.
     */
    kMirror,

    /**
     *  Only draw within the original domain, return transparent-black everywhere else.
     */
    kDecal,

    kLastTileMode = kDecal,
};

