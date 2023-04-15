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
        /// @brief Determines the border style
        ///
        /// @param borderStylePart  The string that should contain information about the border style
        ///
        /// @return                 The border style
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static inline BorderStyle determineBorderStyle(tgui::String borderStylePart)
        {
            if (borderStylePart.length() == 0)
                return BorderStyle::None;

            if(borderStylePart.equalIgnoreCase(U"none"))
                return BorderStyle::None;
            else if (borderStylePart.equalIgnoreCase(U"hidden"))
                return BorderStyle::Hidden;
            else if (borderStylePart.equalIgnoreCase(U"dotted"))
                return BorderStyle::Dotted;
            else if (borderStylePart.equalIgnoreCase(U"dashed"))
                return BorderStyle::Dashed;
            else if (borderStylePart.equalIgnoreCase(U"solid"))
                return BorderStyle::Solid;
            else if (borderStylePart.equalIgnoreCase(U"double"))
                return BorderStyle::Double;
            else if (borderStylePart.equalIgnoreCase(U"groove"))
                return BorderStyle::Groove;
            else if (borderStylePart.equalIgnoreCase(U"ridge"))
                return BorderStyle::Ridge;
            else if (borderStylePart.equalIgnoreCase(U"inset"))
                return BorderStyle::Inset;
            else if (borderStylePart.equalIgnoreCase(U"outset"))
                return BorderStyle::Outset;
            else
                return BorderStyle::None;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Parsed the four size values
        ///
        /// @param borderStyleParts  The string array that should contain information about the border style values
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void parse(std::vector<tgui::String> borderStyleParts)
        {
            if (borderStyleParts.size() == 1)
            {
                top = left = bottom = right = determineBorderStyle(borderStyleParts[0]);
            }
            if (borderStyleParts.size() == 2)
            {
                top = bottom = determineBorderStyle(borderStyleParts[0]);
                left = right = determineBorderStyle(borderStyleParts[1]);
            }
            if (borderStyleParts.size() == 3)
            {
                top = determineBorderStyle(borderStyleParts[0]);
                left = right = determineBorderStyle(borderStyleParts[1]);
                bottom = determineBorderStyle(borderStyleParts[2]);
            }
            if (borderStyleParts.size() == 4)
            {
                top = determineBorderStyle(borderStyleParts[0]);
                right = determineBorderStyle(borderStyleParts[1]);
                bottom = determineBorderStyle(borderStyleParts[2]);
                left = determineBorderStyle(borderStyleParts[3]);
            }
        }
    };

} }

#endif // TGUI_BORDER_STYLE_HPP
