/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// XHTML style (zlib license) provided for TGUI - Texus' Graphical User Interface
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

#ifndef TGUI_XHTML_STYLE_HPP
#define TGUI_XHTML_STYLE_HPP

namespace tgui  { namespace xhtml
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The XHTML style element
    ///
    /// This class represents inner text of any THML element, that supports inner text
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlStyle : public XhtmlElement
    {
    public:
        using Ptr = std::shared_ptr<XhtmlStyle>; //!< Shared style element (base implementation) pointer
        using ConstPtr = std::shared_ptr<const XhtmlStyle>; //!< Shared constant style element (base implementation) pointer

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibilities of the caller!
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyle()
            : XhtmlElement(XhtmlElementType::Style), m_entries()
        { ; }

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The virtual default destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~XhtmlStyle()
        {   m_entries.clear();   }


    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets a new or re-assigns an existing style entry
        ///
        /// @param styleName      The name of the style entry
        /// @param styleEntry     The style entry to register
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setEntry(const tgui::String& styleName, XhtmlStyleEntry::Ptr styleEntry)
        {   if(styleName.size() != 0 && styleEntry != nullptr) m_entries[styleName] = styleEntry; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets an style entry
        ///
        /// @param styleName      The name of the style entry
        ///
        /// @return The requested style entry on success, or nullptr otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry::Ptr getEntry(const tgui::String& styleName)
        {   return (styleName.size() != 0 && m_entries.find(styleName) != m_entries.end() ? m_entries[styleName] : nullptr);   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the style entry
        ///
        /// @param typeName    The element type name to get the style entry for. Can be empty
        /// @param className   The element class name to get the style entry for. Can be empty
        ///
        /// @return The style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleEntry::Ptr getEntry(const char* typeName, const tgui::String& className);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates XHTML style entries from parsed data
        ///
        /// @param messages         The collection of generated error/warning messages
        /// @param buffer           The string that has to be parsed for style entries
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void createEntriesFromParseData(std::vector<std::tuple<MessageType, String>>& messages, const tgui::String& buffer);

    private:
        std::map<tgui::String, XhtmlStyleEntry::Ptr> m_entries;                   //!< The collection of registered styles
    };

} }

#endif // TGUI_XHTML_STYLE_HPP
