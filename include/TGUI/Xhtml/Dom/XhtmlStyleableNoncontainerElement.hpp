/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// XHTML styleable non-container (zlib license) provided for TGUI - Texus' Graphical User Interface
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

#ifndef TGUI_XHTML_STYLEABLE_NONCONTAINER_HPP
#define TGUI_XHTML_STYLEABLE_NONCONTAINER_HPP

namespace tgui  { namespace xhtml
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for all XHTML stylable container element classes
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlStyleableNoncontainerElement : public XhtmlElement , public XhtmlStyleableInterface
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibilities of the caller!
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleableNoncontainerElement()
            : XhtmlElement()
        {  ;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibilities of the caller!
        ///
        /// @param typeName  The type name of the element type to associate
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleableNoncontainerElement(const char* typeName)
            : XhtmlElement(typeName)
        {   ;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibilities of the caller!
        ///
        /// @param typeName    The type name of the element type to associate
        /// @param styleEntry  The style entry to register
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleableNoncontainerElement(const char* typeName, const XhtmlStyleEntry::Ptr styleEntry)
            : XhtmlElement(typeName)
        {   addAttribute(styleEntry);   }

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The virtual default destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~XhtmlStyleableNoncontainerElement()
        {   ;   }


    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the style entry
        ///
        /// @return The style entry. Can be nullptr
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual XhtmlStyleEntry::Ptr getStyleEntry() const
        {   return investigateStyleEntry();   }
    };

} }

#endif // TGUI_XHTML_STYLEABLE_NONCONTAINER_HPP
