/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// XHTML container (zlib license) provided for TGUI - Texus' Graphical User Interface
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

#ifndef TGUI_XHTML_CONTAINER_HPP
#define TGUI_XHTML_CONTAINER_HPP

namespace tgui  { namespace xhtml
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for all XHTML container element classes
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlContainerElement : public XhtmlElement
    {
    public:
        using Ptr = std::shared_ptr<XhtmlContainerElement>; //!< Shared generic (container) node pointer
        using ConstPtr = std::shared_ptr<const XhtmlContainerElement>; //!< Shared constant generic (container) node pointer

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibilities of the caller!
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlContainerElement()
            : XhtmlElement()
        {   m_children = std::make_shared<std::vector<XhtmlElement::Ptr>>();   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibilities of the caller!
        ///
        /// @param typeName  The type name of the element type to associate
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlContainerElement(const char* typeName)
            : XhtmlElement(typeName)
        {   m_children = std::make_shared<std::vector<XhtmlElement::Ptr>>();   }

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The virtual default destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~XhtmlContainerElement()
        {   m_children->clear();   }
    };

} }

#endif // TGUI_XHTML_CONTAINER_HPP
