/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// XHTML elements (zlib license) provided for TGUI - Texus' Graphical User Interface
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

#ifndef TGUI_XHTML_ELEMENTS_HPP
#define TGUI_XHTML_ELEMENTS_HPP

#include <cstddef>
#include <memory>
#include <vector>

#include <TGUI/String.hpp>

namespace tgui
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Collection of implemented XHTML element types
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct TGUI_API XhtmlElementType
    {
    public:
        static constexpr const char  DocType[] = "!doctype";   //!< The tag name of an !DOCTYPE element
        static constexpr const char  Document[] = "document";  //!< The tag name of a DOCUMENT element
        static constexpr const char  Style[] = "style";        //!< The tag name of a STYLE element
        static constexpr const char  Break[] = "br";           //!< The tag name of a BREAK element
        static constexpr const char  Text[] = "text";          //!< The virtual tag name of an INNER-TEXT element
        static constexpr const char  Html[] = "html";          //!< The tag name of a HTML element
        static constexpr const char  Head[] = "head";          //!< The tag name of a HEAD element
        static constexpr const char  Meta[] = "meta";          //!< The tag name of a META element
        static constexpr const char  Link[] = "link";          //!< The tag name of a LINK element
        static constexpr const char  Title[] = "title";        //!< The tag name of a TITLE element
        static constexpr const char  Body[] = "body";          //!< The tag name of a BODY element
        static constexpr const char  Input[] = "input";        //!< The tag name of an INPUT element
        static constexpr const char  Label[] = "label";        //!< The tag name of a LABEL element

        static constexpr const char  H1[] = "h1";              //!< The tag name of a HEADLINE-1 element
        static constexpr const char  H2[] = "h2";              //!< The tag name of a HEADLINE-2 element
        static constexpr const char  H3[] = "h3";              //!< The tag name of a HEADLINE-3 element
        static constexpr const char  H4[] = "h4";              //!< The tag name of a HEADLINE-4 element
        static constexpr const char  H5[] = "h5";              //!< The tag name of a HEADLINE-5 element
        static constexpr const char  H6[] = "h6";              //!< The tag name of a HEADLINE-6 element
        static constexpr const char  Emphasized[] = "em";      //!< The tag name of an EMPHASIZED element
        static constexpr const char  Italic[] = "i";           //!< The tag name of an ITALIC element
        static constexpr const char  Strong[] = "s";           //!< The tag name of a STRONG element
        static constexpr const char  Bold[] = "b";             //!< The tag name of a BOLD element
        static constexpr const char  Underline[] = "u";        //!< The tag name of an UNDERLINE element
        static constexpr const char  Superscript[] = "sup";    //!< The tag name of a SUPERSCRIPT element
        static constexpr const char  Subscript[] = "sub";      //!< The tag name of a SUBSCRIPT element

        static constexpr const char  UnorderedList[] = "ul";   //!< The tag name of an UNORDERED-LIST element
        static constexpr const char  OrderedList[] = "ol";     //!< The tag name of an ORDERED-LIST element
        static constexpr const char  ListItem[] = "li";        //!< The tag name of a LIST-ITEM element
        static constexpr const char  Span[] = "span";          //!< The tag name of a SPAN element
        static constexpr const char  Division[] = "div";       //!< The tag name of a DIVISION element
        static constexpr const char  Preformatted[] = "pre";   //!< The tag name of a PREFORMATTED element
        static constexpr const char  Code[] = "code";          //!< The tag name of a CODE element
        static constexpr const char  Paragraph[] = "p";        //!< The tag name of a PARAGRAPH element

        static constexpr const char  Anchor[] = "a";           //!< The tag name of an ANCHOR element
        static constexpr const char  Image[] = "img";          //!< The tag name of an IMAGE element
        static constexpr const char  Script[] = "script";      //!< The tag name of a SCRIPT element

    private:
        static XhtmlElementType m_dataTypes[]; //!< The supported XHTML element types

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determine the element type from the element type name
        ///
        /// @param text      The element type from the element type name on success, or the element type 'empty' otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static XhtmlElementType getType(const char* typeName = "empty");

    public:
        const char*             TypeName;            //!< The element type name
        bool                    IsAutoClosed;        //!< The flag indicating whether element can contain inner text
        bool                    IsStyleable;         //!< The flag indicating whether element can have a style entry
        bool                    IsContainer;         //!< The flag indicating whether element can have children
        bool                    IsStructuringBlock;  //!< The flag indicating whether element is a structuring block (H1, ...)
        bool                    IsTextBlock;         //!< The flag indicating whether element is a text block (div, pre, ...)
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Forward declarations
    class TGUI_API XhtmlStyle;
    class TGUI_API XhtmlInnerText;
    class TGUI_API XhtmlStyleableElement;
    class TGUI_API XhtmlContainerElement;
    class TGUI_API XhtmlStyleableNoncontainerElement;
    class TGUI_API XhtmlStyleableContainerElement;
    class TGUI_API XhtmlListItem;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for **all** XHTML element (text and node) classes
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlElement
    {
    public:
        using Ptr = std::shared_ptr<XhtmlElement>; //!< Shared XHTML element (base implementation) pointer
        using ConstPtr = std::shared_ptr<const XhtmlElement>; //!< Shared constant XHTML element (base implementation) pointer

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlElement()
            : m_elementType(XhtmlElementType::getType()), m_parent(nullptr), m_attributes(), m_children(),
#if _DEBUG
              m_startTagBeginPosition(0), m_startTagEndPosition(0),m_stopTagBeginPosition(0), m_stopTagEndPosition(0),
#endif
              m_isParsedAsAutoClosed(false)
        {   m_attributes = std::make_shared<std::vector<XhtmlAttribute::Ptr>>();   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
        ///
        /// @param typeName  The type name of the element type to associate
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlElement(const char* typeName)
            : m_elementType(XhtmlElementType::getType(typeName)), m_parent(nullptr), m_attributes(), m_children(),
#if _DEBUG
              m_startTagBeginPosition(0), m_startTagEndPosition(0),m_stopTagBeginPosition(0), m_stopTagEndPosition(0),
#endif
              m_isParsedAsAutoClosed(false)
        {   m_attributes = std::make_shared<std::vector<XhtmlAttribute::Ptr>>();   }


    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The virtual default destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~XhtmlElement()
        {   m_attributes->clear();   }

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Decode special XHTML characters (like '&lt;' --> '<', '&gt;' --> '>', '&amp;' --> '&', ...
        ///
        /// @param text      The text to decode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static String& decode(String& text);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Encode special XHTML characters (like '<' --> '&lt;', '>' --> '&gt;', '&' --> '&amp;', ...
        ///
        /// @param text      The text to decode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static String& encode(String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new XHTML element, according to the indicated typeName
        ///
        /// @param typeName              The type name of the requested XHTML element type
        /// @param parent                The parent to register
        /// @param beginPosition         The cursor position of the XHTML parser, the start tag of this element begins
        /// @param endPosition           The cursor position of the XHTML parser, the start tag of this element ends
        /// @param isParsedAsAutoClosed  The flag indicating whether the XHTML parser detected this element has no closing tag
        ///
        /// @return The new XHTML element of the requested type on success, or a generic XHTML element otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static XhtmlElement::Ptr createAuto(const char* typeName, XhtmlElement::Ptr parent,
            const size_t beginPosition, const size_t endPosition, const bool isParsedAsAutoClosed);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Style element
        ///
        /// Prefer the factory method in cases where the Style element needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The optional parent to register
        ///
        /// @return The new Style element
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyle> createStyle(XhtmlElement::Ptr parent = nullptr);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Break element
        ///
        /// Prefer the factory method in cases where the Break element needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The optional parent to register
        ///
        /// @return The new Break element
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static XhtmlElement::Ptr createBreak(XhtmlElement::Ptr parent = nullptr);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Text element
        ///
        /// Prefer the factory method in cases where the Text element needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The optional parent to register
        ///
        /// @return The new Text element
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlInnerText> createInnerText(XhtmlElement::Ptr parent = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Text element with text initialization
        ///
        /// Prefer the factory method in cases where the Text element needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new Text element
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlInnerText> createInnerText(XhtmlElement::Ptr parent, String text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new HTML (container) node
        ///
        /// Prefer the factory method in cases where the HTML (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new HTML (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createHtml(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new HTML (container) node
        ///
        /// Prefer the factory method in cases where the HTML (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new HTML (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createHtml(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Header (container) node
        ///
        /// Prefer the factory method in cases where the Header (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new Header (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createHead(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Header (container) node
        ///
        /// Prefer the factory method in cases where the Header (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new Header (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createHead(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Body (container) node
        ///
        /// Prefer the factory method in cases where the Body (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new Body (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createBody(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Body (container) node
        ///
        /// Prefer the factory method in cases where the Body (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new Body (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createBody(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Body (container) node
        ///
        /// Prefer the factory method in cases where the Body (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param child       The optional child to register
        ///
        /// @return The new Body (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createBody(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Body (container) node
        ///
        /// Prefer the factory method in cases where the Body (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param children    The collection of children to register
        ///
        /// @return The new Body (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createBody(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Body (container) node
        ///
        /// Prefer the factory method in cases where the Body (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param child   The optional child to register
        ///
        /// @return The new Body (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createBody(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Body (container) node
        ///
        /// Prefer the factory method in cases where the Body (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param color     The color to apply to the current style entry
        /// @param children  The collection of children to register
        ///
        /// @return The new Body (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createBody(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H1 (container) node
        ///
        /// Prefer the factory method in cases where the H1 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new H1 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH1(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H1 (container) node
        ///
        /// Prefer the factory method in cases where the H1 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new H1 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH1(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H1 (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the H1 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new H1 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH1(XhtmlElement::Ptr parent, const String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H2 (container) node
        ///
        /// Prefer the factory method in cases where the H2 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new H2 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH2(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H2 (container) node
        ///
        /// Prefer the factory method in cases where the H2 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new H2 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH2(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H2 (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the H2 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new H2 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH2(XhtmlElement::Ptr parent, const String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H3 (container) node
        ///
        /// Prefer the factory method in cases where the H3 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new H3 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH3(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H3 (container) node
        ///
        /// Prefer the factory method in cases where the H3 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new H3 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH3(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H3 (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the H3 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new H3 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH3(XhtmlElement::Ptr parent, const String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H4 (container) node
        ///
        /// Prefer the factory method in cases where the H4 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new H4 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH4(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H4 (container) node
        ///
        /// Prefer the factory method in cases where the H4 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new H4 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH4(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H4 (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the H4 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new H4 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH4(XhtmlElement::Ptr parent, const String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H5 (container) node
        ///
        /// Prefer the factory method in cases where the H5 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new H5 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH5(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H5 (container) node
        ///
        /// Prefer the factory method in cases where the H5 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new H5 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH5(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H5 (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the H5 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new H5 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH5(XhtmlElement::Ptr parent, const String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H6 (container) node
        ///
        /// Prefer the factory method in cases where the H6 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new H6 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH6(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H6 (container) node
        ///
        /// Prefer the factory method in cases where the H6 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new H6 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH6(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new H6 (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the H6 (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new H6 (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createH6(XhtmlElement::Ptr parent, const String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new EM (container) node
        ///
        /// Prefer the factory method in cases where the EM (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new EM (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createEmphasized(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new EM (container) node
        ///
        /// Prefer the factory method in cases where the EM (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new EM (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createEmphasized(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new EM (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the EM (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new EM (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createEmphasized(XhtmlElement::Ptr parent, const String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new I (container) node
        ///
        /// Prefer the factory method in cases where the I (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new I (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createItalic(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new I (container) node
        ///
        /// Prefer the factory method in cases where the I (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new I (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createItalic(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new I (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the I (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new I (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createItalic(XhtmlElement::Ptr parent, const String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Strong (container) node
        ///
        /// Prefer the factory method in cases where the Strong (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new Strong (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createStrong(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Strong (container) node
        ///
        /// Prefer the factory method in cases where the Strong (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new Strong (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createStrong(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Strong (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Strong (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new Strong (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createStrong(XhtmlElement::Ptr parent, const String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Bold (container) node
        ///
        /// Prefer the factory method in cases where the Bold (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new Bold (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createBold(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Bold (container) node
        ///
        /// Prefer the factory method in cases where the Bold (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new Bold (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createBold(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Bold (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Bold (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new Bold (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createBold(XhtmlElement::Ptr parent, const String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Underline/Misspelled (container) node
        ///
        /// Prefer the factory method in cases where the Underline/Misspelled (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new Underline/Misspelled (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createUnderline(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Underline/Misspelled (container) node
        ///
        /// Prefer the factory method in cases where the Underline/Misspelled (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new Underline/Misspelled (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createUnderline(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Underline/Misspelled (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Underline/Misspelled (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new Underline/Misspelled (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createUnderline(XhtmlElement::Ptr parent, const String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Superscript (container) node
        ///
        /// Prefer the factory method in cases where the Superscript (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new Superscript (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createSuperscript(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Superscript (container) node
        ///
        /// Prefer the factory method in cases where the Superscript (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new Superscript (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createSuperscript(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Superscript (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Superscript (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new Superscript (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createSuperscript(XhtmlElement::Ptr parent, const String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Subscript (container) node
        ///
        /// Prefer the factory method in cases where the Subscript (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new Subscript (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createSubscript(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Subscript (container) node
        ///
        /// Prefer the factory method in cases where the Subscript (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new Subscript (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createSubscript(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Subscript (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Subscript (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param text    The text to apply
        ///
        /// @return The new Subscript (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlContainerElement> createSubscript(XhtmlElement::Ptr parent, const String& text);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new unordered List (container) node
        ///
        /// Prefer the factory method in cases where the unordered List (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new unordered List (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createUnorderedList(XhtmlElement::Ptr parent, const XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new unordered List (container) node
        ///
        /// Prefer the factory method in cases where the unordered List (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new unordered List (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createUnorderedList(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new unordered List (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the unordered List (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param child       The optional child to register
        ///
        /// @return The new unordered List (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createUnorderedList(XhtmlElement::Ptr parent, const XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new unordered List (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the unordered List (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param children    The collection of children to register
        ///
        /// @return The new unordered List (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createUnorderedList(XhtmlElement::Ptr parent, const XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new unordered List (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the unordered List (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param child   The optional child to register
        ///
        /// @return The new unordered List (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createUnorderedList(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new unordered List (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the unordered List (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param children  The collection of children to register
        ///
        /// @return The new unordered List (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createUnorderedList(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new ordered List (container) node
        ///
        /// Prefer the factory method in cases where the ordered List (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new ordered List (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createOrderedList(XhtmlElement::Ptr parent, const XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new ordered List (container) node
        ///
        /// Prefer the factory method in cases where the ordered List (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new ordered List (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createOrderedList(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new ordered List (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the ordered List (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param child       The optional child to register
        ///
        /// @return The new ordered List (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createOrderedList(XhtmlElement::Ptr parent, const XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new ordered List (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the ordered List (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param children    The collection of children to register
        ///
        /// @return The new ordered List (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createOrderedList(XhtmlElement::Ptr parent, const XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new ordered List (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the ordered List (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param child   The optional child to register
        ///
        /// @return The new ordered List (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createOrderedList(XhtmlElement::Ptr paren, const Color color, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new ordered List (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the ordered List (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param children  The collection of children to register
        ///
        /// @return The new ordered List (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createOrderedList(XhtmlElement::Ptr paren, const Color color, const std::vector<XhtmlElement::Ptr> children);
        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new List item (container) node
        ///
        /// Prefer the factory method in cases where the List item (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new List item (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlListItem> createListItem(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new List item (container) node
        ///
        /// Prefer the factory method in cases where the List item (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new List item (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlListItem> createListItem(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new List item (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the List item (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param child       The optional child to register
        ///
        /// @return The new List item (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlListItem> createListItem(XhtmlElement::Ptr parent, const XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new List item (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the List item (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param children    The collection of children to register
        ///
        /// @return The new List item (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlListItem> createListItem(XhtmlElement::Ptr parent, const XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new List item (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the List item (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param child   The optional child to register
        ///
        /// @return The new List item (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlListItem> createListItem(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new List item (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the List item (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param children  The collection of children to register
        ///
        /// @return The new List item (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlListItem> createListItem(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children);
        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Span (container) node
        ///
        /// Prefer the factory method in cases where the Span (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new Span (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createSpan(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Span (container) node
        ///
        /// Prefer the factory method in cases where the Span (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new Span (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createSpan(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Span (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the Span (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param child       The optional child to register
        ///
        /// @return The new Span (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createSpan(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Span (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the Span (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param children    The collection of children to register
        ///
        /// @return The new Span (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createSpan(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Span (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Span (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param child   The optional child to register
        ///
        /// @return The new Span (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createSpan(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Span (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Span (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param color     The color to apply to the current style entry
        /// @param children  The collection of children to register
        ///
        /// @return The new Span (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createSpan(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Division (container) node
        ///
        /// Prefer the factory method in cases where the Division (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new Division (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createDivision(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Division (container) node
        ///
        /// Prefer the factory method in cases where the Division (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new Division (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createDivision(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Division (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the Division (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param child       The optional child to register
        ///
        /// @return The new Division (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createDivision(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Division (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the Division (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param children    The collection of children to register
        ///
        /// @return The new Division (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createDivision(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Division (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Division (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param child   The optional child to register
        ///
        /// @return The new Division (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createDivision(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Division (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Division (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param children  The collection of children to register
        ///
        /// @return The new Division (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createDivision(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Preformatted (container) node
        ///
        /// Prefer the factory method in cases where the Preformatted (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new Preformatted (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createPreformatted(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Preformatted (container) node
        ///
        /// Prefer the factory method in cases where the Preformatted (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new Preformatted (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createPreformatted(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Preformatted (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the Preformatted (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param child       The optional child to register
        ///
        /// @return The new Preformatted (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createPreformatted(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Preformatted (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the Preformatted (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style to register
        /// @param children    The collection of children to register
        ///
        /// @return The new Preformatted (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createPreformatted(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Preformatted (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Preformatted (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param child   The optional child to register
        ///
        /// @return The new Preformatted (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createPreformatted(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Preformatted (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Preformatted (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param children  The collection of children to register
        ///
        /// @return The new Preformatted (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createPreformatted(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children);
        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Code (container) node
        ///
        /// Prefer the factory method in cases where the Code (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new Code (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createCode(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Code (container) node
        ///
        /// Prefer the factory method in cases where the Code (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new Code (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createCode(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Code (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the Code (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param child       The optional child to register
        ///
        /// @return The new Code (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createCode(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Code (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the Code (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param children    The collection of children to register
        ///
        /// @return The new Code (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createCode(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Code (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Code (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param child   The optional child to register
        ///
        /// @return The new Code (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createCode(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Code (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Code (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param children  The collection of children to register
        ///
        /// @return The new Code (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createCode(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Paragraph (container) node
        ///
        /// Prefer the factory method in cases where the Paragraph (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param child   The optional child to register
        ///
        /// @return The new Paragraph (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createParagraph(XhtmlElement::Ptr parent, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Paragraph (container) node
        ///
        /// Prefer the factory method in cases where the Paragraph (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent    The parent to register. Can be nullptr
        /// @param children  The collection of children to register
        ///
        /// @return The new Paragraph (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createParagraph(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Paragraph (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the Paragraph (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param child       The optional child to register
        ///
        /// @return The new Paragraph (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createParagraph(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Paragraph (container) node with style entry initialization
        ///
        /// Prefer the factory method in cases where the Paragraph (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        /// @param children    The collection of children to register
        ///
        /// @return The new Paragraph (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createParagraph(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Paragraph (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Paragraph (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param child   The optional child to register
        ///
        /// @return The new Paragraph (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createParagraph(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Paragraph (container) node with foreground color initialization
        ///
        /// Prefer the factory method in cases where the Paragraph (container) node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        /// @param color   The color to apply to the current style entry
        /// @param children  The collection of children to register
        ///
        /// @return The new Paragraph (container) node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableContainerElement> createParagraph(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Image node
        ///
        /// Prefer the factory method in cases where the Image node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent  The parent to register. Can be nullptr
        ///
        /// @return The new Image node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableNoncontainerElement> createImage(XhtmlElement::Ptr parent);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new Image node with style entry initialization
        ///
        /// Prefer the factory method in cases where the Image node needs to be shared, otherwise prefer the constructor.
        ///
        /// @param parent      The parent to register. Can be nullptr
        /// @param styleEntry  The style entry to register
        ///
        /// @return The new Image node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<XhtmlStyleableNoncontainerElement> createImage(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry);

        //-----------------------------------------------------------------------------------------------------------------------

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether the element is based on an empty by definition tag
        ///
        /// @return The flag indicating whether the element is based on an empty by definition tag
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isAutoClosedTagByDefinition();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether the element is an empty element (has no closing tag)
        ///
        /// @return The flag indicating whether the element is an empty element (has no closing tag)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool isAutoClosed() const
        {   return m_elementType.IsAutoClosed;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the type name of the element
        ///
        /// @return The type name of the element
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline const char* getTypeName() const
        {   return m_elementType.TypeName;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the class name of the element
        ///
        /// @return The class name of the element
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const tgui::String& getClassNames() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the type name of the element as wide character string
        ///
        /// @return The type name of the element as wide character string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline tgui::String getTypeNameU32() const
        {   return m_elementType.TypeName;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether the element can contain child elements
        ///
        /// @return The flag indicating whether the element can contain child elements
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline bool isContainer() const
        {   return m_elementType.IsContainer;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether the element is a structuring block
        ///
        /// This specifies the global document structure (creates document sections with linebreak/ carriage return behavior)
        ///
        /// @return The flag indicating whether the element is a structuring block (specifies the global document structure)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline bool isStructuringBlock() const
        { return m_elementType.IsStructuringBlock; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether the element is a text block
        ///
        /// This specifies the local document structure (creates paragraphs with linebreak/ carriage return behavior)
        ///
        /// @return The flag indicating whether the element can contain child elements (specifies the local document structure)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline bool isTextBlock() const
        {   return m_elementType.IsTextBlock;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether the element supports styling
        ///
        /// @return The flag indicating whether the element supports styling
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual inline bool isStylable() const
        {   return m_elementType.IsStyleable; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the element's collection of attributes. Can be empty
        ///
        /// @return The element's collection of attributes. Can be empty
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline std::shared_ptr<std::vector<XhtmlAttribute::Ptr>> getAttributes() const
        {   return m_attributes;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds one attribute to the element's collection of attributes
        ///
        /// @param attribute  The attribute to add to the element's collection of attributes
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void addAttribute(XhtmlAttribute::Ptr attribute)
        {   return m_attributes->push_back(attribute);   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds one attribute to the element's collection of attributes
        ///
        /// @param attributes  The collection of attributes to add to the element's collection of attributes
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void addAttributes(const std::vector<XhtmlAttribute::Ptr>& attributes)
        {   for (size_t index = 0; index < attributes.size(); index++) m_attributes->push_back(attributes[index]);   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the indicated attribute
        ///
        /// @param attributes  The zero-based index of the attribute to get
        ///
        /// @return The indicated attribute on success, or nullptr otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlAttribute::Ptr getAttribute(size_t index) const
        {   return index < m_attributes->size() ? m_attributes->at(index) : nullptr;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the indicated attribute
        ///
        /// @param name       The name of the attribute to get
        ///
        /// @return The indicated attribute on success, or nullptr otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlAttribute::Ptr getAttribute(const tgui::String& name) const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the element's parent element. Can be nullptr
        ///
        /// @param parent  The parent to set
        ///
        /// @return The previously set parent
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlElement::Ptr setParent(XhtmlElement::Ptr parent)
        {   XhtmlElement::Ptr oldParent = m_parent; m_parent = parent; return oldParent;  }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the element's parent element. Can be nullptr
        ///
        /// @return The element's parent element. Can be nullptr
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlElement::Ptr getParent() const
        {   return m_parent;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Appends a child to the collection of parent's children and registers the parent as the child's parent
        ///
        /// @param parent  The parent element to register to the child element
        /// @param child   The child element to append to the collection of parent's children and to be set the parent
        ///
        /// @return The flag indicating the success of the add operation
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool addChildAndSetPatent(XhtmlElement::Ptr parent, XhtmlElement::Ptr child);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes the child from the collection of parent's childrenand unregisters the parent as the child's parent
        ///
        /// @param parent  The parent element to unregister from the child element
        /// @param child   The child  element to remove from the collection of parent's children and to be unset the parent
        ///
        /// @return The flag indicating the success of the remove operation
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool removeChildAndUnsetPatent(XhtmlElement::Ptr parent, XhtmlElement::Ptr child);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Appends the children to the collection of parent's children and registers the parent as the children's parent
        ///
        /// @param parent    The parent element to register to the child elements
        /// @param children  The child  elements to append to the collection of parent's children and to be set the parent
        ///
        /// @return The number of added children
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static int addChildrenAndSetPatent(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines the previous sibling from the parent's children
        ///
        /// @param element    The element todetermines the previous sibling for
        ///
        /// @return The previous sibling from the parent's children on success or nullptr otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static XhtmlElement::Ptr getPreviousSibling(XhtmlElement::Ptr element);

#if _DEBUG
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the element's start tag positions
        ///
        /// @return The element's start tag positions
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setStartTagPositions(const size_t beginPosition, const size_t endPosition)
        {
            m_startTagBeginPosition = beginPosition;
            m_startTagEndPosition = endPosition;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the element's start tag begin position
        ///
        /// @return The element's start tag begin position
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline size_t getStartTagBeginPos() const
        {   return m_startTagBeginPosition;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the element's start tag end position
        ///
        /// @return The element's start tag end position
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline size_t getStartTagEndPos() const
        {   return m_startTagEndPosition;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the element's stop tag positions
        ///
        /// @return The element's stop tag positions
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setStopTagPositions(const size_t beginPosition, const size_t endPosition)
        {
            m_stopTagBeginPosition = beginPosition;
            m_stopTagEndPosition = endPosition;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the element's stop tag begin position
        ///
        /// @return The element's stop tag begin position
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline size_t getStopTagBeginPos() const
        {   return m_stopTagBeginPosition;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the element's stop tag end position
        ///
        /// @return The element's stop tag end position
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline size_t getStopTagEndPos() const
        {   return m_stopTagEndPosition;   }
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the flag indicating whether the element is parsed as auto-closed
        ///
        /// @param  The flag indicating whether the element is parsed as auto-closed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setParsedAsAutoClosed(bool isParsedAsAutoClosed)
        {   m_isParsedAsAutoClosed = isParsedAsAutoClosed;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the flag indicating whether the element is parsed as auto-closed
        ///
        /// @return The flag indicating whether the element is parsed as auto-closed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline bool getParsedAsAutoClosed() const
        {   return m_isParsedAsAutoClosed;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the number of child elements
        ///
        /// @return The number of child elements
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline size_t countChildren() const
         {   if (m_children != nullptr) return m_children->size(); else return 0;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the element's collection of children. Can be nullptr
        ///
        /// @return The element's collection of children. Can be nullptr
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::shared_ptr<std::vector<XhtmlElement::Ptr>> getChildren(const char* typeName = "") const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the indicated child element of this container
        ///
        /// @return The indicated child element of this container on success, or **nullptr* otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlElement::Ptr getChild(size_t index)
        {   if (m_children != nullptr) return (index < m_children->size() ? m_children->at(index) : nullptr); else return nullptr;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the element's first child with the indicated type name. Can be nullptr
        ///
        /// @param typeName  The type name of the requested first child. Can be nullptr or empty to get the very first child
        ///
        /// @return The element's first child with the indicated type name on success, or nullptr otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlElement::Ptr getFirstChild(const char* typeName) const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes a child from the collection of children
        ///
        /// @param child  The child to remove from the collection of children
        ///
        /// @return The flag indicating the success of the removal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool removeChild(XhtmlElement::Ptr child);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines the index of the indicated child within the collection of children
        ///
        /// @param child  The child to determine the index for
        ///
        /// @return The index of the indicated child on success, or SIZE_MAX otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        size_t indexOfChild(XhtmlElement::Ptr child) const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates trace output to the console
        ///
        /// @param indent    The current line indent
        /// @param complete  The flag determinining whether to integrate children
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void trace(std::wstring indent, bool complete = true);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a collection XHTML attributes from raw string (on success)
        ///
        /// @param messages        The collection of generated error/warning messages
        /// @param attributes      The collection of attributes to add the created attributes to
        /// @param buffer          The raw string to parse from
        /// @param beginPosition   The offset to the parse start position within the buffer
        ///
        /// @return The number of character processes in the buffer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static size_t createAttributesFromParseStr(std::vector<std::tuple<tgui::MessageType, String>>& messages,
            std::vector<XhtmlAttribute::Ptr>& attributes, const tgui::String& buffer, const size_t beginPosition);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates an XHTML element from raw string (on success)
        ///
        /// @param messages         The collection of generated error/warning messages
        /// @param parent           The parent XHTML element. Can be nullptr
        /// @param buffer           The raw string to parse from
        /// @param beginPosition    The offset to the parse start position within the buffer
        /// @param isOpeningTag     Determine whether element is inside opening tag
        /// @param isClosingTag     Determine whether element is inside closing tag
        /// @param isInsideScript   Determine whether element is is between '<script ...>' and '</script>'
        /// @param processedLength  The number of characters that have been parsed from the buffer
        /// @param parseInnerText   Determine whether to parse the inner text of this element
        ///
        /// @return The XHTML element on success, or nullptr otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static const XhtmlElement::Ptr createElementFromParseStr(std::vector<std::tuple<tgui::MessageType, tgui::String>>& messages,
            XhtmlElement::Ptr parent, const tgui::String& buffer, const size_t beginPosition,
            bool& isOpeningTag, bool& isClosingTag, bool& isInsideScript, size_t& processedLength, bool parseInnerText = true);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates an XHTML element from parsed data
        ///
        /// @param messages         The collection of generated error/warning messages
        /// @param isOpeningTag     Determine whether tag parse currently is an opening tag
        /// @param isClosingTag     Determine whether tag parse currently is an closing tag
        /// @param elementOpened    Determine whether parse position within the buffer is after/within opening tag '<script ...>'
        /// @param elementClosed    Determine whether parse position within the buffer is within/after closing tag '</script>'
        /// @param typeName         The tyte name of XHTML element to parse
        /// @param parent           The parent XHTML element. Can be nullptr
        /// @param beginPosition    The offset to the parse position within the buffer, indicating the element start
        /// @param endPosition      The offset to the parse position within the buffer, indicating the element end
        /// @param attributesBuffer The collection of parsed attributes so far
        ///
        /// @return The XHTML element on success, or nullptr otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static const XhtmlElement::Ptr createElementFromParseData(std::vector<std::tuple<MessageType, tgui::String>>& messages,
            bool& isOpeningTag, bool& isClosingTag, bool elementOpened, bool elementClosed, tgui::String& typeName,
            XhtmlElement::Ptr& parent, const size_t& beginPosition, const size_t& endPosition, std::vector<XhtmlAttribute::Ptr>& attributesBuffer);

    protected:
        XhtmlElementType                                  m_elementType; //!< The associated XHTML element type
        XhtmlElement::Ptr                                 m_parent;      //!< This element's parent element
        std::shared_ptr<std::vector<XhtmlAttribute::Ptr>> m_attributes;  //!< This element's attribute list
        std::shared_ptr<std::vector<XhtmlElement::Ptr>>   m_children;    //!< This element's child elements

    private:
#if _DEBUG
        size_t                                            m_startTagBeginPosition;
        size_t                                            m_startTagEndPosition;
        size_t                                            m_stopTagBeginPosition;
        size_t                                            m_stopTagEndPosition;
#endif
        bool                                              m_isParsedAsAutoClosed;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyle()
            : XhtmlElement(XhtmlElementType::Style), m_styles()
        { ; }

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The virtual default destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~XhtmlStyle()
        {   m_styles.clear();   }


    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets a new or re-assigns an existing style entry
        ///
        /// @param styleName      The name of the style entry
        /// @param styleEntry     The style entry to register
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setEntry(const tgui::String& styleName, XhtmlStyleEntry::Ptr styleEntry)
        {   if(styleName.size() != 0 && styleEntry != nullptr) m_styles[styleName] = styleEntry; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets an style entry
        ///
        /// @param styleName      The name of the style entry
        ///
        /// @return The requested style entry on success, or nullptr otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry::Ptr getEntry(const tgui::String& styleName)
        {   return (styleName.size() != 0 && m_styles.find(styleName) != m_styles.end() ? m_styles[styleName] : nullptr);   }

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
        std::map<tgui::String, XhtmlStyleEntry::Ptr> m_styles;                   //!< The collection of registered styles
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The XHTML text element
    ///
    /// This class represents inner text of any THML element, that supports inner text
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlInnerText : public XhtmlElement
    {
    public:
        using Ptr = std::shared_ptr<XhtmlInnerText>; //!< Shared inner text element (base implementation) pointer
        using ConstPtr = std::shared_ptr<const XhtmlInnerText>; //!< Shared constant inner text element (base implementation) pointer

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlInnerText()
            : XhtmlElement(XhtmlElementType::Text), m_text()
        {   ;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
        ///
        /// @param text    The text to apply
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlInnerText(String text)
            : XhtmlElement(XhtmlElementType::Text), m_text(L"")
        {   setText(text);   }

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The virtual default destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~XhtmlInnerText()
        {   ;   }


    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the text of this element
        ///
        /// @param text  The new text to set
        /// @param decode  The flag, determining whether to encode the text, default is true
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setText(String text, bool decode = true) { m_text = (decode ? XhtmlElement::decode(text) : text); }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the text of the element
        ///
        /// @return The text of the element. The text will typically be decoded. To get encoded text use XhtmlHelper.encode()
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline String getText() { return m_text; }

    private:
        String m_text;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for all XHTML style-able container element classes
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlStyleableInterface
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the style entry
        ///
        /// @return The style entry. Can be nullptr
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual XhtmlStyleEntry::Ptr getStyleEntry() = 0;
    };

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
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlContainerElement()
            : XhtmlElement()
        {   m_children = std::make_shared<std::vector<XhtmlElement::Ptr>>();   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for all XHTML stylable container element classes
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlStyleableNoncontainerElement : public XhtmlElement , public XhtmlStyleableInterface
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleableNoncontainerElement()
            : XhtmlElement()
        {  ;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
        ///
        /// @param typeName  The type name of the element type to associate
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleableNoncontainerElement(const char* typeName)
            : XhtmlElement(typeName)
        {   ;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
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
        XhtmlStyleEntry::Ptr getStyleEntry();
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for all XHTML stylable container element classes
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlStyleableContainerElement : public XhtmlContainerElement, public XhtmlStyleableInterface
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleableContainerElement()
            : XhtmlContainerElement()
        {   m_children = std::make_shared<std::vector<XhtmlElement::Ptr>>();   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
        ///
        /// @param typeName  The type name of the element type to associate
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleableContainerElement(const char* typeName)
            : XhtmlContainerElement(typeName)
        {   m_children = std::make_shared<std::vector<XhtmlElement::Ptr>>();   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
        ///
        /// @param typeName    The type name of the element type to associate
        /// @param styleEntry  The style to register
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleableContainerElement(const char* typeName, XhtmlStyleEntry::Ptr styleEntry)
            : XhtmlContainerElement(typeName)
        {   m_children = std::make_shared<std::vector<XhtmlElement::Ptr>>();   addAttribute(styleEntry);   }

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The virtual default destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~XhtmlStyleableContainerElement()
        {   m_children->clear();   }


    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the style entry
        ///
        /// @return The style entry. Can be nullptr
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleEntry::Ptr getStyleEntry();
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The enumeration of list item types
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    enum class ListItemType
    {
        InheritOrDefault = 1 << 0,
        None = 1 << 1,
        Disc = 1 << 2,
        Circle = 1 << 3,
        Square = 1 << 4,
        Arabic = 1 << 5,
        UpperRoman = 1 << 6,
        LowerRoman = 1 << 7,
        UpperAlpha = 1 << 8,
        LowerAlpha = 1 << 9
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The XHTML List item (container) node
    ///
    /// This class represents the HTML **<li style="...">...</li>** node
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlListItem : public XhtmlStyleableContainerElement
    {
    public:
        using Ptr = std::shared_ptr<XhtmlListItem>;            //!< Shared unordered/ordered List (container) node pointer
        using ConstPtr = std::shared_ptr<const XhtmlListItem>; //!< Shared constant unordered/ordered List (container) node pointer

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlListItem()
            : XhtmlStyleableContainerElement(XhtmlElementType::ListItem), m_bullettype(ListItemType::InheritOrDefault)
        { ; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// ATTENTION: To register a parent and to register this new object to a parent are the responsibility of the caller!
        ///
        /// @param styleEntry  The style entry to register
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlListItem(XhtmlStyleEntry::Ptr styleEntry)
            : XhtmlStyleableContainerElement(XhtmlElementType::ListItem, styleEntry), m_bullettype(ListItemType::InheritOrDefault)
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
        inline ListItemType getBulletgetTypeName() const { return m_bullettype; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the bullettype
        ///
        /// @param count   The bullettype
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setBullettype(ListItemType bullettype) { m_bullettype = bullettype; }

    private:
        ListItemType m_bullettype;
    };

}

#endif // TGUI_XHTML_ELEMENTS_HPP
