/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Markup size type for TGUI - Texus' Graphical User Interface
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

#ifndef TGUI_SIZE_TYPE_HPP
#define TGUI_SIZE_TYPE_HPP

namespace tgui  { namespace xhtml
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The enumeration of size types (size of font, border, margin, padding, ...)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    enum class SizeType
    {
        Pixel,         //!< The size is given in pixels, this is the default if no measuring unit is set
        Point,         //!< The size is given in points
        EquivalentOfM, //!< The size is given equivalents to the height of the letter m (16px)
        Relative,      //!< The size is given relative to the default size
        ViewportWidth  //!< The size is given relative to the view-port size
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The one dimensional size (can be used for font, ...)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct OneDimSize
    {
        SizeType   sizeType; //!< The size type (size measuring unit)
        float      value;    //!< The size value

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        OneDimSize(SizeType t, float s)
            : sizeType(t), value(s)
        { ; }
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The four dimensional size (can be used for border, margin, padding, ...)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct FourDimSize
    {
        SizeType    sizeType;   //!< The size type (size measuring unit)
        float       top;        //!< The top coordinate value
        float       right;      //!< The width coordinate value
        float       bottom;     //!< The height coordinate value
        float       left;       //!< The left coordinate value

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// @param type   The size type (size measuring unit)
        /// @param m      The margin size value (all four dimensions)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FourDimSize(SizeType type, float m)
            : sizeType(type), top(m), right(m), bottom(m), left(m)
        { ; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// @param type   The size type (size measuring unit)
        /// @param v      The vertical margin size value (top and bottom)
        /// @param h      The horizontal margin size value (left and right)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FourDimSize(SizeType type, float v, float h)
            : sizeType(type), top(v), right(h), bottom(v), left(h)
        { ; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// @param type   The size type (size measuring unit)
        /// @param t      The top margin size value
        /// @param h      The horizontal margin size value (left and right)
        /// @param b      The bottom margin size value
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FourDimSize(SizeType type, float t, float h, float b)
            : sizeType(type), top(t), right(h), bottom(b), left(h)
        { ; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// @param type   The size type (size measuring unit)
        /// @param t      The top margin size value
        /// @param r      The right margin size value
        /// @param b      The bottom margin size value
        /// @param l      The left margin size value
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FourDimSize(SizeType type, float t, float r, float b, float l)
            : sizeType(type), top(t), right(r), bottom(b), left(l)
        { ; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the flag whether this four dimension four dimansional size in pixel is empty
        ///
        /// @param parentSize  The parent size for the calculation of a relative defined size
        ///
        /// @return            The flag whether this four dimension size in pixel is empty
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline bool isEmpty(Vector2f parentSize) const
        {
            auto fourDimSize = toPixel(parentSize);
            if (fourDimSize.top > 0.0f || fourDimSize.right > 0.0f || fourDimSize.bottom > 0.0f || fourDimSize.left > 0.0f)
                return false;
            else
                return true;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Calculates this four dimension size equivalent in pixel out of the current size
        ///
        /// @param parentSize  The parent size for the calculation of a relative defined size
        ///
        /// @return            The four dimension size equivalent in pixel calculated out of the current size
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline FourDimSize toPixel(Vector2f parentSize) const
        {
            if (sizeType == SizeType::Pixel)
                return FourDimSize(SizeType::Pixel, top, right, bottom, left);
            else if (sizeType == SizeType::Point)
                return FourDimSize(SizeType::Pixel, top * 1.3333333333f, right * 1.3333333333f, bottom * 1.3333333333f, left * 1.3333333333f);
            else if (sizeType == SizeType::EquivalentOfM)
                return FourDimSize(SizeType::Pixel, top * 16.0f, right * 16.0f, bottom * 16.0f, left * 16.0f);
            else
                return FourDimSize(SizeType::Pixel, top * parentSize.y, right * parentSize.x, bottom * parentSize.y, left * parentSize.x);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines the size type
        ///
        /// @param sizePart  The string that should contain information about the size type
        ///
        /// @return          The size type
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static inline SizeType determineSizeType(tgui::String sizePart)
        {
            if (sizePart.find(U"px") != SIZE_MAX)        return SizeType::Pixel;
            else if (sizePart.find(U"pt") != SIZE_MAX)   return SizeType::Point;
            else if (sizePart.find(U"em") != SIZE_MAX)   return SizeType::EquivalentOfM;
            else if (sizePart.find(U"%") != SIZE_MAX)    return SizeType::Relative;
            else                                         return SizeType::Pixel;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Parsed the four size values
        ///
        /// @param sizeParts  The string array that should contain information about the size values
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void parse(std::vector<tgui::String> sizeParts)
        {
            if (sizeParts.size() == 1)
            {
                top      = left = bottom = right = sizeParts[0].toFloat();
                sizeType = FourDimSize::determineSizeType(sizeParts[0]);
            }
            if (sizeParts.size() == 2)
            {
                top      = bottom = sizeParts[0].toFloat();
                left     = right  = sizeParts[1].toFloat();
                sizeType = FourDimSize::determineSizeType(sizeParts[0]);
            }
            if (sizeParts.size() == 3)
            {
                top      = sizeParts[0].toFloat();
                left     = right = sizeParts[1].toFloat();
                bottom   = sizeParts[2].toFloat();
                sizeType = FourDimSize::determineSizeType(sizeParts[0]);
            }
            if (sizeParts.size() == 4)
            {
                top      = sizeParts[0].toFloat();
                right    = sizeParts[1].toFloat();
                bottom   = sizeParts[2].toFloat();
                left     = sizeParts[3].toFloat();
                sizeType = FourDimSize::determineSizeType(sizeParts[0]);
            }
        }
    };

} }

#endif // TGUI_SIZE_TYPE_HPP
