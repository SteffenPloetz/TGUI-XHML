/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// XHTML list item (zlib license) provided for TGUI - Texus' Graphical User Interface
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

#ifndef TGUI_XHTML_LIST_ITEM_HPP
#define TGUI_XHTML_LIST_ITEM_HPP

namespace tgui  { namespace xhtml
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The XHTML List item (styleable container) node
    ///
    /// This class represents the HTML **<li style="...">...</li>** node
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlListItem : public XhtmlStyleableContainerElement
    {
    public:
        using Ptr = std::shared_ptr<XhtmlListItem>;            //!< Shared List item (styleable container) node pointer
        using ConstPtr = std::shared_ptr<const XhtmlListItem>; //!< Shared constant List item (styleable container) node pointer

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibilities of the caller!
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlListItem()
            : XhtmlStyleableContainerElement(XhtmlElementType::ListItem), m_bullettype(MarkupListItemType::InheritOrDefault)
        { ; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibilities of the caller!
        ///
        /// @param styleEntry  The style entry to register
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlListItem(XhtmlStyleEntry::Ptr styleEntry)
            : XhtmlStyleableContainerElement(XhtmlElementType::ListItem, styleEntry), m_bullettype(MarkupListItemType::InheritOrDefault)
        { ; }


    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The virtual default destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~XhtmlListItem()
        { ; }


    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the bullettype
        ///
        /// @return The bullettype
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline MarkupListItemType getBulletgetTypeName() const { return m_bullettype; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the bullettype
        ///
        /// @param count   The bullettype
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setBullettype(MarkupListItemType bullettype) { m_bullettype = bullettype; }

    private:
        MarkupListItemType m_bullettype; //!< This list item type (defining the bullet or enumeration of a list item)
    };

} }

#endif // TGUI_XHTML_LIST_ITEM_HPP
