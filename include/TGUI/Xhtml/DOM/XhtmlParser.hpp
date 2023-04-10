/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// XHTML parser (zlib license) provided for TGUI - Texus' Graphical User Interface
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This parser is heavily inspired by the great work 'HTML Reader C++ Class Library', version dated 29 March 2004
// (https://www.codeproject.com/Articles/6561/HTML-Reader-C-Class-Library), written by 'Gurmeet S. Kochar'
// <gomzygotit@hotmail.com> aka 'gUrM33T' (https://www.codeproject.com/script/Membership/View.aspx?mid=547523).
//
// Unfortunately, the original code is not cross-platform (uses MS Windows data types instead of STL data types) and has an
// customized infecting license. For these reasons, the parser included here is a complete re-implementation.
//
// Nevertheless, a visit to the original is worthwhile in any case.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Another source of inspiration was the 'htmlcxx - html and css APIs for C++' library (http://sf.net/projects/htmlcxx)
// by Davi de Castro Reis and Robson Braga Araújo.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TINY_XHTML_PARSER
#define TINY_XHTML_PARSER

#include <tuple>
#include <vector>
#include <map>
#include <algorithm>

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The XHTML parser
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlParser
    {
    public:
        using MsgType = std::tuple<MessageType, tgui::String>; //!< The type of error/warning messages to use while parsing

    private:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlParser()
            : m_buffer(), m_bufferPos(0), m_messages(), m_rootElements()
        {}

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// @param buffer  The buffer of the character stream to parse
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlParser(const tgui::String& buffer)
            : m_buffer(buffer), m_bufferPos(0), m_messages(), m_rootElements()
        {}

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Acquires the next character from the buffer and update the cursor
        ///
        /// @return The next character from the buffer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        char32_t acquireChar()
        { return m_buffer[m_bufferPos++]; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Rejects the last acquired character from the buffer and update the cursor
        ///
        /// @return The rejected character from the buffer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        char32_t rejectChar()
        { return m_buffer[--m_bufferPos]; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Parses a comment tag
        ///
        /// @param comment  The parsed comment text
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool parseComment(tgui::String& comment);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Parses the complete XHTML document
        ///
        /// @param resolveEntities  Determine whether to resolve entities
        ///
        /// @return The number of parsed characters
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        size_t parseDocument(bool resolveEntities = true);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Cleans breaks ('\r' '\n') and spaces ('\t', '\v', ' ') from characters string
        ///
        /// @param parentElement  The parent XHTML element to determine cleaning details
        ///                       - partial cleaning, if string isn't an inner text (breaks and repeated spaces)
        ///                       - complete cleaning otherwise (breaks and all spaces)
        /// @param characters     The cuurently parsed string to clean
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void parseDocument_cleanBreaksAndSpaces(const XhtmlElement::Ptr parentElement, tgui::String& characters);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Processes breaks ('<br>') and spaces (' ') from characters string, depending on the preformattedDepth
        ///
        /// @param parentElement      The parent XHTML element to add new XHTML child elements to
        /// @param characters         The cuurently parsed string to process regarding breaks and spaces
        /// @param preformattedDepth  The number of not closed '<pre>' tags (inside a '<pre>' tag the spaces should not be suppressed)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void parseDocument_createInnerText(XhtmlElement::Ptr parentElement, tgui::String& characters, int preformattedDepth);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks the parity of XHTML start tag and XHTML end tag
        ///
        /// @param startTagElement  The XHTML element, that should represent the start tag
        /// @param endTagElement  The XHTML element, that should represent the end tag
        ///
        /// @return The flag whether the XHTML elements match (true on success, or false otherwise)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool parseDocument_checkStartEndTagParity(XhtmlElement::Ptr startTagElement, XhtmlElement::Ptr endTagElement);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates an XHTML element from raw string (on success)
        ///
        /// @param parent          The parent element to register the parsed element to
        /// @param isOpeningTag    Determine whether element is inside opening tag
        /// @param isClosingTag    Determine whether element is inside closing tag
        /// @param isInsideScript  Determine whether element is is between '<script ...>' and '</script>'
        ///
        /// @return The XHTML element on success, or nullptr otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlElement::Ptr parseElement(XhtmlElement::Ptr parent, bool& isOpeningTag, bool& isClosingTag, bool& isInsideScript);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the collection of root elements. Can be empty
        ///
        /// @return The collection of root elements. Can be empty
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline std::vector<XhtmlElement::Ptr> getRootElements() const
        {   return m_rootElements;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the first root element with the indicated type name. Can be nullptr
        ///
        /// @param typeName  The type name of the requested first root element
        ///
        /// @return The first root element with the indicated type name. Can be nullptr
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlElement::Ptr getFirstRootElement(const char* typeName = "") const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the complete collection of parser (error/warning) messages. Can be empty (if parser could recognize 100%)
        ///
        /// @return The complete collection of parser (error/warning) messages. Can be empty
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline std::vector<std::tuple<tgui::MessageType, tgui::String>> getMessages() const
        {   return m_messages;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the collection of parser error messages. Can be empty (if o parser errors occurred)
        ///
        /// @return The collection of parser error messages. Can be empty
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::shared_ptr<std::vector<tgui::String>> getErrorMessages() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the collection of parser warning messages. Can be empty (if o parser warnings occurred)
        ///
        /// @return The collection of parser warning messages. Can be empty
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::shared_ptr<std::vector<tgui::String>> getWarningMessages() const;

    private:
        tgui::String                   m_buffer;       //!< The UTF character stream to parse
        size_t                         m_bufferPos;    //!< The actual cursor position within the UTF character stream to parse
        std::vector<MsgType>           m_messages;     //!< The error/warning messages that occurred when parsing the UTF character stream
        std::vector<XhtmlElement::Ptr> m_rootElements; //!< The collection of root elements, parsed from the UTF character stream
    };

}

#endif    // TINY_XHTML_PARSER
