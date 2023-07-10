/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Markup border style provided for TGUI - Texus' Graphical User Interface
// Copyright (C) 2023 Steffen Ploetz (Steffen.Ploetz@cityweb.de)
// The size type is used to represent the measuring unit along with one ... four dimension value(s).
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_BORDER_STYLE_HPP
#define TGUI_BORDER_STYLE_HPP

namespace tgui  { namespace xhtml
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The enumeration of border styles (applied to a border of a block element like h1...h6, div, p, table, ...)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    enum class BorderStyle
    {
        None,          //!< No border
        Hidden,        //!< No border
        Dotted,        //!< A dotted border
        Dashed,        //!< A dashed border
        Solid,         //!< A solid border
        Double,        //!< A double-line solid border
        Groove,        //!< A two-color solid border with 3D-look of a surrounding cavitation and the impression of a light source at top left
        Ridge,         //!< A two-color solid border with 3D-look of a surrounding cumulation and the impression of a light source at top left
        Inset,         //!< A two-color solid border with 3D-look of a sunken center and the impression of a light source at top left
        Outset         //!< A two-color solid border with 3D-look of a raised center and the impression of a light source at top left
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The one dimensional size (can be used for font, ...)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct FourDimBorderStyle
    {
        BorderStyle left;       //!< The left border style
        BorderStyle top;        //!< The top border style
        BorderStyle right;      //!< The width border style
        BorderStyle bottom;     //!< The height border style

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FourDimBorderStyle()
            : left(BorderStyle::None), top(BorderStyle::None), right(BorderStyle::None), bottom(BorderStyle::None)
        {   ;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default and initializing constructor
        ///
        /// @param ltrb   The left/top/right/bottom border style
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FourDimBorderStyle(BorderStyle ltrb)
            : left(ltrb), top(ltrb), right(ltrb), bottom(ltrb)
        {   ;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the flag whether this four dimension border style is none
        ///
        /// @return The flag whether this four dimension border style is none
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline bool isNoneOrHidden() const
        {
            if ((left   != BorderStyle::None && left   != BorderStyle::Hidden) ||
                (top    != BorderStyle::None && top    != BorderStyle::Hidden) ||
                (right  != BorderStyle::None && right  != BorderStyle::Hidden) ||
                (bottom != BorderStyle::None && bottom != BorderStyle::Hidden))
                return false;
            else
                return true;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Tries to recognize the border style
        ///
        /// @param borderStyleString The string that should contain information about the border style
        /// @param borderStyleValue  The border style on success
        ///
        /// @return                  The flag indicating whether recognition succeded (true) or not (false)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static inline bool tryRecognizeBorderStyle(tgui::String borderStyleString, BorderStyle& borderStyleValue)
        {
            if (borderStyleString.length() == 0)
                return false;

            if(borderStyleString.equalIgnoreCase(U"none"))
                borderStyleValue = BorderStyle::None;
            else if (borderStyleString.equalIgnoreCase(U"hidden"))
                borderStyleValue = BorderStyle::Hidden;
            else if (borderStyleString.equalIgnoreCase(U"dotted"))
                borderStyleValue = BorderStyle::Dotted;
            else if (borderStyleString.equalIgnoreCase(U"dashed"))
                borderStyleValue = BorderStyle::Dashed;
            else if (borderStyleString.equalIgnoreCase(U"solid"))
                borderStyleValue = BorderStyle::Solid;
            else if (borderStyleString.equalIgnoreCase(U"double"))
                borderStyleValue = BorderStyle::Double;
            else if (borderStyleString.equalIgnoreCase(U"groove"))
                borderStyleValue = BorderStyle::Groove;
            else if (borderStyleString.equalIgnoreCase(U"ridge"))
                borderStyleValue = BorderStyle::Ridge;
            else if (borderStyleString.equalIgnoreCase(U"inset"))
                borderStyleValue = BorderStyle::Inset;
            else if (borderStyleString.equalIgnoreCase(U"outset"))
                borderStyleValue = BorderStyle::Outset;
            else
                return false;

            return true;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Tries to parse the four size values
        ///
        /// @param borderStyleParts  The string array that should contain information about the border style values
        ///
        /// @return                  The flag indicating whether parse succeded (true) or not (false)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline bool tryParse(std::vector<tgui::String> borderStyleParts)
        {
            if (borderStyleParts.size() == 1)
            {
                BorderStyle borderStyle = BorderStyle::None;
                if (!tryRecognizeBorderStyle(borderStyleParts[0], borderStyle))
                    return false;
                top = left = bottom = right = borderStyle;
                return true;
            }
            if (borderStyleParts.size() == 2)
            {
                BorderStyle borderStyle = BorderStyle::None;
                if (!tryRecognizeBorderStyle(borderStyleParts[0], borderStyle))
                    return false;
                top = bottom = borderStyle;
                if (!tryRecognizeBorderStyle(borderStyleParts[1], borderStyle))
                    return false;
                left = right = borderStyle;
                return true;
            }
            if (borderStyleParts.size() == 3)
            {
                BorderStyle borderStyle = BorderStyle::None;
                if (!tryRecognizeBorderStyle(borderStyleParts[0], borderStyle))
                    return false;
                top = borderStyle;
                if (!tryRecognizeBorderStyle(borderStyleParts[1], borderStyle))
                    return false;
                left = right = borderStyle;
                if (!tryRecognizeBorderStyle(borderStyleParts[2], borderStyle))
                    return false;
                bottom = borderStyle;
                return true;
            }
            if (borderStyleParts.size() == 4)
            {
                BorderStyle borderStyle = BorderStyle::None;
                if (!tryRecognizeBorderStyle(borderStyleParts[0], borderStyle))
                    return false;
                top = borderStyle;
                if (!tryRecognizeBorderStyle(borderStyleParts[1], borderStyle))
                    return false;
                right = borderStyle;
                if (!tryRecognizeBorderStyle(borderStyleParts[2], borderStyle))
                    return false;
                bottom = borderStyle;
                if (!tryRecognizeBorderStyle(borderStyleParts[3], borderStyle))
                    return false;
                left = borderStyle;
                return true;
            }
            return false;
        }
    };

} }

#endif // TGUI_BORDER_STYLE_HPP
