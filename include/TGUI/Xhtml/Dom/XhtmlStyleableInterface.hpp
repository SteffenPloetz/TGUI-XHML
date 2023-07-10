/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// XHTML styleable interface (zlib license) provided for TGUI - Texus' Graphical User Interface
// Copyright (C) 2023 Steffen Ploetz (Steffen.Ploetz@cityweb.de)
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

#ifndef TGUI_XHTML_STYLEABLE_INTERFACE_HPP
#define TGUI_XHTML_STYLEABLE_INTERFACE_HPP

namespace tgui  { namespace xhtml
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for all XHTML style-able container element classes
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlStyleableInterface
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Virtual destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~XhtmlStyleableInterface() = default;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the style entry
        ///
        /// @return The style entry. Can be nullptr
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual XhtmlStyleEntry::Ptr getStyleEntry() const = 0;
    };

} }

#endif // TGUI_XHTML_STYLEABLE_INTERFACE_HPP
