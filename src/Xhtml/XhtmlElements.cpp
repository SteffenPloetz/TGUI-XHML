#include <TGUI/Config.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Vector2.hpp>

#include <fstream>

// MSC needs a clear distiction between "__declspec(dllimport)" (above) and "__declspec(dllexport)" (below) this comment.
// So in the case of direct source file integration (in contrast to library creation and linking), the API must be 'dllexport'.
#ifndef TGUI_STATIC
#ifdef TGUI_SYSTEM_WINDOWS
#undef TGUI_API
#define TGUI_API __declspec(dllexport)
#endif
#endif

#include "TGUI/StringHelper.hpp"
#include "TGUI/Xhtml/XhtmlEntityResolver.hpp"
#include "TGUI/Xhtml/XhtmlAttributes.hpp"
#include "TGUI/Xhtml/XhtmlElements.hpp"

// intense tracing
// #define LOG_ELEMENT 1

#define UNUSED(x) (void)(x)

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17 && !defined(_MSC_VER)
    constexpr const char  XhtmlElementType::DocType[];
    constexpr const char  XhtmlElementType::Document[];
    constexpr const char  XhtmlElementType::Style[];
    constexpr const char  XhtmlElementType::Break[];
    constexpr const char  XhtmlElementType::Text[];
    constexpr const char  XhtmlElementType::Html[];
    constexpr const char  XhtmlElementType::Head[];
    constexpr const char  XhtmlElementType::Meta[];
    constexpr const char  XhtmlElementType::Link[];
    constexpr const char  XhtmlElementType::Body[];
    constexpr const char  XhtmlElementType::Input[];
    constexpr const char  XhtmlElementType::Label[];

    constexpr const char  XhtmlElementType::H1[];
    constexpr const char  XhtmlElementType::H2[];
    constexpr const char  XhtmlElementType::H3[];
    constexpr const char  XhtmlElementType::H4[];
    constexpr const char  XhtmlElementType::H5[];
    constexpr const char  XhtmlElementType::H6[];
    constexpr const char  XhtmlElementType::Emphasized[];
    constexpr const char  XhtmlElementType::Italic[];
    constexpr const char  XhtmlElementType::Strong[];
    constexpr const char  XhtmlElementType::Bold[];
    constexpr const char  XhtmlElementType::Underline[];
    constexpr const char  XhtmlElementType::Superscript[];
    constexpr const char  XhtmlElementType::Subscript[];

    constexpr const char  XhtmlElementType::List[];
    constexpr const char  XhtmlElementType::ListItem[];
    constexpr const char  XhtmlElementType::Span[];
    constexpr const char  XhtmlElementType::Division[];
    constexpr const char  XhtmlElementType::Preformatted[];
    constexpr const char  XhtmlElementType::Code[];
    constexpr const char  XhtmlElementType::Paragraph[];

    constexpr const char  XhtmlElementType::Anchor[];
    constexpr const char  XhtmlElementType::Image[];
    constexpr const char  XhtmlElementType::Script[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlElementType XhtmlElementType::m_dataTypes[] =
    {
        //                                   IsAutoClosed  IsStyleable IsContainer IsStructuringBlock IsTextBlock // IsContainer ==> empty means no closing tag
        {   XhtmlElementType::DocType,       true,         false,      false,      false,             false   },
        {   XhtmlElementType::Document,      false,        false,      false,      false,             false   },
        {   XhtmlElementType::Style,         true,         false,      false,      false,             false   },

        {   XhtmlElementType::Break,         true,         false,      false,      false,             true    },
        {   XhtmlElementType::Text,          true,         false,      false,      false,             false   },
        {   XhtmlElementType::Html,          false,        false,      true,       true,              false   },
        {   XhtmlElementType::Head,          false,        false,      true,       true,              false   },
        {   XhtmlElementType::Meta,          true,         false,      true,       false,             false   },
        {   XhtmlElementType::Link,          true,         false,      true,       false,             false   },
        {   XhtmlElementType::Title,         false,        false,      true,       false,             false   },
        {   XhtmlElementType::Body,          false,        true,       true,       true,              false   },
        {   XhtmlElementType::Input,         true,         false,      true,       false,             false   },
        {   XhtmlElementType::Label,         false,        true,       true,       false,             false   },

        {   XhtmlElementType::H1,            false,        false,      true,       true,              false   },
        {   XhtmlElementType::H2,            false,        false,      true,       true,              false   },
        {   XhtmlElementType::H3,            false,        false,      true,       true,              false   },
        {   XhtmlElementType::H4,            false,        false,      true,       true,              false   },
        {   XhtmlElementType::H5,            false,        false,      true,       true,              false   },
        {   XhtmlElementType::H6,            false,        false,      true,       true,              false   },

        {   XhtmlElementType::Emphasized,    false,        false,      true,       false,             false   },
        {   XhtmlElementType::Italic,        false,        false,      true,       false,             false   },
        {   XhtmlElementType::Strong,        false,        false,      true,       false,             false   },
        {   XhtmlElementType::Bold,          false,        false,      true,       false,             false   },
        {   XhtmlElementType::Underline,     false,        false,      true,       false,             false   },
        {   XhtmlElementType::Superscript,   false,        false,      true,       false,             false   },
        {   XhtmlElementType::Subscript,     false,        false,      true,       false,             false   },

        {   XhtmlElementType::UnorderedList, false,        true,       true,       true,              false   },
        {   XhtmlElementType::OrderedList,   false,        true,       true,       true,              false   },
        {   XhtmlElementType::ListItem,      false,        true,       true,       false,             true    },
        {   XhtmlElementType::Span,          false,        true,       true,       false,             false   },
        {   XhtmlElementType::Division,      false,        true,       true,       false,             true    },
        {   XhtmlElementType::Preformatted,  false,        true,       true,       false,             true    },
        {   XhtmlElementType::Code,          false,        true,       true,       false,             true    },
        {   XhtmlElementType::Paragraph,     false,        true,       true,       false,             true    },

        {   XhtmlElementType::Anchor,        false,        true,       true,       false,             false   },
        {   XhtmlElementType::Image,         false,        true,       false,      false,             false   },
        {   XhtmlElementType::Script,        false,        true,       false,      false,             false   },

        {   "empty",                         false,        false,      false,      false,             false   }
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlElementType XhtmlElementType::getType(const char* typeName)
    {
        auto size = sizeof(tgui::XhtmlElementType::m_dataTypes) / sizeof(m_dataTypes[0]);
        for (size_t index = 0; index < size; index++)
            if (ext::string::strcasecmp(m_dataTypes[index].TypeName, typeName) == 0)
                return m_dataTypes[index];
        return m_dataTypes[size - 1];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& XhtmlElement::decode(String& text)
    {
        text.replace("&lt;", "<");
        text.replace("&gt;", ">");
        text.replace("&amp;", "&");

        return text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String& XhtmlElement::encode(String& text)
    {
        text.replace("<", "&lt;");
        text.replace(">", "&gt;");
        text.replace("&", "&amp;");

        return text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlElement::Ptr XhtmlElement::createAuto(const char* typeName, XhtmlElement::Ptr parent,
        const size_t beginPosition, const size_t endPosition, const bool isParsedAsAutoClosed)
    {
        XhtmlElement::Ptr element = nullptr;
        if (ext::string::strcasecmp(typeName, XhtmlElementType::DocType) == 0 ||
            ext::string::strcasecmp(typeName, XhtmlElementType::Document) == 0)
        {
            element = std::make_shared<XhtmlElement>(typeName);
            addChildAndSetPatent(parent, element);
        }
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Style) == 0)
            element = XhtmlElement::createStyle(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Break) == 0)
            element = XhtmlElement::createBreak(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Text) == 0)
            element = XhtmlElement::createInnerText(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Html) == 0)
            element = XhtmlElement::createHtml(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Head) == 0)
            element = XhtmlElement::createHead(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Meta) == 0 ||
            ext::string::strcasecmp(typeName, XhtmlElementType::Link) == 0 ||
            ext::string::strcasecmp(typeName, XhtmlElementType::Title) == 0)
        {
            element = std::make_shared<XhtmlElement>(typeName);
            addChildAndSetPatent(parent, element);
        }
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Body) == 0)
            element = XhtmlElement::createBody(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Input) == 0 ||
            ext::string::strcasecmp(typeName, XhtmlElementType::Label) == 0)
        {
            element = std::make_shared<XhtmlElement>(typeName);
            addChildAndSetPatent(parent, element);
        }
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::H1) == 0)
            element = XhtmlElement::createH1(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::H2) == 0)
            element = XhtmlElement::createH2(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::H3) == 0)
            element = XhtmlElement::createH3(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::H4) == 0)
            element = XhtmlElement::createH4(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::H5) == 0)
            element = XhtmlElement::createH5(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::H6) == 0)
            element = XhtmlElement::createH6(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Emphasized) == 0)
            element = XhtmlElement::createEmphasized(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Italic) == 0)
            element = XhtmlElement::createItalic(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Strong) == 0)
            element = XhtmlElement::createStrong(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Bold) == 0)
            element = XhtmlElement::createBold(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Underline) == 0)
            element = XhtmlElement::createUnderline(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Superscript) == 0)
            element = XhtmlElement::createSuperscript(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Subscript) == 0)
            element = XhtmlElement::createSubscript(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::UnorderedList) == 0)
            element = XhtmlElement::createUnorderedList(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::OrderedList) == 0)
            element = XhtmlElement::createOrderedList(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::ListItem) == 0)
            element = XhtmlElement::createListItem(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Span) == 0)
            element = XhtmlElement::createSpan(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Division) == 0)
            element = XhtmlElement::createDivision(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Preformatted) == 0)
            element = XhtmlElement::createPreformatted(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Code) == 0)
            element = XhtmlElement::createCode(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Paragraph) == 0)
            element = XhtmlElement::createParagraph(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Anchor) == 0)
        {
            element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Anchor);
            addChildAndSetPatent(parent, element);
        }
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Image) == 0)
            element = XhtmlElement::createImage(parent);
        else if (ext::string::strcasecmp(typeName, XhtmlElementType::Script) == 0)
        {
            element = std::make_shared<XhtmlStyleableNoncontainerElement>(XhtmlElementType::Script);
            addChildAndSetPatent(parent, element);
        }
        else
        {
            element = std::make_shared<XhtmlElement>(typeName);
            addChildAndSetPatent(parent, element);
        }
#if _DEBUG
        element->setStartTagPositions(beginPosition, endPosition);
        element->setParsedAsAutoClosed(isParsedAsAutoClosed);
#else
        UNUSED(beginPosition);
        UNUSED(endPosition);
        UNUSED(isInline);
#endif
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyle> XhtmlElement::createStyle(XhtmlElement::Ptr parent)
    {
        auto element = std::make_shared<XhtmlStyle>();
        addChildAndSetPatent(parent, element);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlElement::Ptr XhtmlElement::createBreak(XhtmlElement::Ptr parent)
    {
        auto element = std::make_shared<XhtmlElement>(XhtmlElementType::Break);
        addChildAndSetPatent(parent, element);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlInnerText> XhtmlElement::createInnerText(XhtmlElement::Ptr parent)
    {
        auto element = std::make_shared<XhtmlInnerText>();
        addChildAndSetPatent(parent, element);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlInnerText> XhtmlElement::createInnerText(XhtmlElement::Ptr parent, String text)
    {
        auto element = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(parent, element);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createHtml(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Html);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createHtml(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Html);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createHead(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Head);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createHead(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Head);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createBody(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Body);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createBody(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Body);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createBody(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Body, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createBody(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Body, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createBody(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Body, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createBody(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Body, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH1(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H1);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH1(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H1);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH1(XhtmlElement::Ptr parent, const String& text)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H1);
        addChildAndSetPatent(parent, element);
        auto innerText = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(element, innerText);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH2(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H2);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH2(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H2);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH2(XhtmlElement::Ptr parent, const String& text)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H2);
        addChildAndSetPatent(parent, element);
        auto innerText = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(element, innerText);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH3(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H3);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH3(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H3);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }


    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH3(XhtmlElement::Ptr parent, const String& text)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H3);
        addChildAndSetPatent(parent, element);
        auto innerText = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(element, innerText);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH4(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H4);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH4(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H4);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH4(XhtmlElement::Ptr parent, const String& text)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H4);
        addChildAndSetPatent(parent, element);
        auto innerText = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(element, innerText);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH5(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H5);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH5(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H5);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH5(XhtmlElement::Ptr parent, const String& text)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H5);
        addChildAndSetPatent(parent, element);
        auto innerText = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(element, innerText);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH6(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H6);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH6(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H6);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createH6(XhtmlElement::Ptr parent, const String& text)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::H6);
        addChildAndSetPatent(parent, element);
        auto innerText = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(element, innerText);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createEmphasized(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Emphasized);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createEmphasized(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Emphasized);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createEmphasized(XhtmlElement::Ptr parent, const String& text)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Emphasized);
        addChildAndSetPatent(parent, element);
        auto innerText = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(element, innerText);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createItalic(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Italic);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createItalic(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Italic);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createItalic(XhtmlElement::Ptr parent, const String& text)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Italic);
        addChildAndSetPatent(parent, element);
        auto innerText = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(element, innerText);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createStrong(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Strong);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createStrong(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Strong);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createStrong(XhtmlElement::Ptr parent, const String& text)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Strong);
        addChildAndSetPatent(parent, element);
        auto innerText = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(element, innerText);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createBold(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Bold);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createBold(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Bold);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createBold(XhtmlElement::Ptr parent, const String& text)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Bold);
        addChildAndSetPatent(parent, element);
        auto innerText = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(element, innerText);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createUnderline(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Underline);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createUnderline(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Underline);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createUnderline(XhtmlElement::Ptr parent, const String& text)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Underline);
        addChildAndSetPatent(parent, element);
        auto innerText = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(element, innerText);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createSuperscript(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Superscript);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createSuperscript(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Superscript);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createSuperscript(XhtmlElement::Ptr parent, const String& text)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Superscript);
        addChildAndSetPatent(parent, element);
        auto innerText = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(element, innerText);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createSubscript(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Subscript);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createSubscript(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Subscript);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlContainerElement> XhtmlElement::createSubscript(XhtmlElement::Ptr parent, const String& text)
    {
        auto element = std::make_shared<XhtmlContainerElement>(XhtmlElementType::Subscript);
        addChildAndSetPatent(parent, element);
        auto innerText = std::make_shared<XhtmlInnerText>(text);
        addChildAndSetPatent(element, innerText);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createUnorderedList(XhtmlElement::Ptr parent, const XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::UnorderedList);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createUnorderedList(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::UnorderedList);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createUnorderedList(XhtmlElement::Ptr parent, const XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::UnorderedList, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createUnorderedList(XhtmlElement::Ptr parent, const XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::UnorderedList, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createUnorderedList(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::UnorderedList, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createUnorderedList(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::UnorderedList, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createOrderedList(XhtmlElement::Ptr parent, const XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::OrderedList);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createOrderedList(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::OrderedList);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createOrderedList(XhtmlElement::Ptr parent, const XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::OrderedList, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createOrderedList(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::OrderedList, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createOrderedList(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::OrderedList, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createOrderedList(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::OrderedList, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlListItem> XhtmlElement::createListItem(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlListItem>();
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlListItem> XhtmlElement::createListItem(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlListItem>();
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlListItem> XhtmlElement::createListItem(XhtmlElement::Ptr parent, const XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlListItem>(styleEntry);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlListItem> XhtmlElement::createListItem(XhtmlElement::Ptr parent, const XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlListItem>(styleEntry);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlListItem> XhtmlElement::createListItem(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlListItem>(std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlListItem> XhtmlElement::createListItem(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlListItem>(std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createSpan(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Span);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createSpan(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Span);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createSpan(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Span, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createSpan(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Span, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createSpan(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Span, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createSpan(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Span, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createDivision(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Division);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createDivision(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Division);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createDivision(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Division, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createDivision(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Division, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createDivision(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Division, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createDivision(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Division, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createPreformatted(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Preformatted);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createPreformatted(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Preformatted);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createPreformatted(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Preformatted, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createPreformatted(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Preformatted, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createPreformatted(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Preformatted, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createPreformatted(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Preformatted, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createCode(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Code);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createCode(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Code);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createCode(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Code, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createCode(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Code, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createCode(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Code, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createCode(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Code, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createParagraph(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Paragraph);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createParagraph(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Paragraph);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createParagraph(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Paragraph, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createParagraph(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Paragraph, styleEntry);
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createParagraph(XhtmlElement::Ptr parent, const Color color, XhtmlElement::Ptr child)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Paragraph, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildAndSetPatent(element, child);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableContainerElement> XhtmlElement::createParagraph(XhtmlElement::Ptr parent, const Color color, const std::vector<XhtmlElement::Ptr> children)
    {
        auto element = std::make_shared<XhtmlStyleableContainerElement>(XhtmlElementType::Paragraph, std::make_shared<XhtmlStyleEntry>(color));
        addChildAndSetPatent(parent, element);
        addChildrenAndSetPatent(element, children);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableNoncontainerElement> XhtmlElement::createImage(XhtmlElement::Ptr parent)
    {
        auto element = std::make_shared<XhtmlStyleableNoncontainerElement>(XhtmlElementType::Image);
        addChildAndSetPatent(parent, element);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<XhtmlStyleableNoncontainerElement> XhtmlElement::createImage(XhtmlElement::Ptr parent, XhtmlStyleEntry::Ptr styleEntry)
    {
        auto element = std::make_shared<XhtmlStyleableNoncontainerElement>(XhtmlElementType::Image, styleEntry);
        addChildAndSetPatent(parent, element);
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool XhtmlElement::isAutoClosedTagByDefinition()
    {
        if (strlen(m_elementType.TypeName) == 0)
            return false;

        if (ext::string::strcasecmp(m_elementType.TypeName, XhtmlElementType::DocType) == 0)
            return true;
        if (ext::string::strcasecmp(m_elementType.TypeName, XhtmlElementType::Meta) == 0)
            return true;
        if (ext::string::strcasecmp(m_elementType.TypeName, XhtmlElementType::Link) == 0)
            return true;
        if (ext::string::strcasecmp(m_elementType.TypeName, XhtmlElementType::Break) == 0)
            return true;
        if (ext::string::strcasecmp(m_elementType.TypeName, XhtmlElementType::Text) == 0)
            return true;
        if (ext::string::strcasecmp(m_elementType.TypeName, XhtmlElementType::Input) == 0)
            return true;

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::u32string& XhtmlElement::getClassNames() const
    {
        if (m_attributes == nullptr)
            return ext::u32string::emptyString();

        auto classAttribute = getAttribute(U"class");
        return (classAttribute != nullptr ? classAttribute->getValue() : ext::u32string::emptyString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlAttribute::Ptr XhtmlElement::getAttribute(const std::u32string& name) const
    {
        if (m_attributes == nullptr)
            return nullptr;

        for (XhtmlAttribute::Ptr attr : *m_attributes)
        {
            if (ext::u32string::compareNoCase(attr->getName(), name) == 0)
                return attr;
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool XhtmlElement::addChildAndSetPatent(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        if (parent == nullptr || child == nullptr)
            return false;

        auto parentContaimerElement = std::dynamic_pointer_cast<XhtmlContainerElement>(parent);
        if (parentContaimerElement == nullptr)
            return false;

        if (parentContaimerElement->m_children == nullptr)
            return false;

        parentContaimerElement->m_children->push_back(child);
        child->setParent(parent);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool XhtmlElement::removeChildAndUnsetPatent(XhtmlElement::Ptr parent, XhtmlElement::Ptr child)
    {
        if (parent == nullptr || child == nullptr)
            return false;

        auto parentContaimerElement = std::dynamic_pointer_cast<XhtmlContainerElement>(parent);
        if (parentContaimerElement == nullptr)
            return false;

        if (parentContaimerElement->m_children == nullptr)
            return false;

        parentContaimerElement->removeChild(child);
        child->setParent(nullptr);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int XhtmlElement::addChildrenAndSetPatent(XhtmlElement::Ptr parent, const std::vector<XhtmlElement::Ptr> children)
    {
        if (parent == nullptr || children.size() == 0)
            return 0;

        auto parentContaimerElement = std::dynamic_pointer_cast<XhtmlContainerElement>(parent);
        if (parentContaimerElement == nullptr)
            return 0;

        if (parentContaimerElement->m_children != nullptr)
        {
            int result = 0;
            for (XhtmlElement::Ptr child : children)
            {
                parentContaimerElement->m_children->push_back(child);
                child->setParent(parent);
                result++;
            }
            return result;
        }
        else
            return 0;  }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlElement::Ptr XhtmlElement::getPreviousSibling(XhtmlElement::Ptr element)
    {
        if (element == nullptr)
            return nullptr;

        auto parent = element->getParent();
        if (parent != nullptr)
        {
            auto index = parent->indexOfChild(element);
            if (index > 0 && index != SIZE_MAX)
                return parent->getChild(index - 1);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<std::vector<XhtmlElement::Ptr>> XhtmlElement::getChildren(const char* typeName) const
    {
        if (m_children == nullptr)
            return nullptr;

        if (typeName != nullptr && strlen(typeName) == 0)
            return m_children;

        auto result = std::make_shared<std::vector<XhtmlElement::Ptr>>();
        for (XhtmlElement::Ptr e : *m_children)
            if (ext::string::strcasecmp(e->getTypeName(), typeName) == 0)
                result->push_back(e);

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlElement::Ptr XhtmlElement::getFirstChild(const char* typeName) const
    {
        if (m_children == nullptr)
            return nullptr;

        for (XhtmlElement::Ptr e : *m_children)
            if (typeName == nullptr || strlen(typeName) == 0 || ext::string::strcasecmp(e->getTypeName(), typeName) == 0)
                return e;
        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool XhtmlElement::removeChild(XhtmlElement::Ptr child)
    {
        if (m_children == nullptr)
            return false;

        auto iter = m_children->begin();
        for (;iter !=  m_children->end() && *iter != child; iter++);
        if (*iter != child)
            return false;

        m_children->erase(iter);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    size_t XhtmlElement::indexOfChild(XhtmlElement::Ptr child) const
    {
        size_t index = 0;

        if (m_children == nullptr)
            return SIZE_MAX;

        for (auto c : (*m_children))
        {
            if (child == c)
                return index;
            index++;
        }

        return SIZE_MAX;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void XhtmlElement::trace(std::wstring indent, bool complete)
    {
        XhtmlAttribute::Ptr attribute;
        std::u32string identity = ((attribute = getAttribute(U"id")) != nullptr ? U", id: '" + (const std::u32string&)(attribute->getValue()) + U"'" :
            ((attribute = getAttribute(U"class")) != nullptr ? U", class: '" + (const std::u32string&)(attribute->getValue()) + U"'" : U""));
        std::u32string parent = U", parent: " + (m_parent == nullptr ? U"none" : U"'" + m_parent->getTypeNameU32() + U"'");
        std::u32string children = (m_children != nullptr ? U", children: " + (wchar_t)(U'0' + m_children->size()) : U"");

        std::u32string content = U"";
        if (ext::string::strcasecmp(getTypeName(), XhtmlElementType::Text) == 0)
        {
            if (((XhtmlInnerText*)this)->getText().length() > 12)
                content += U", content: \"" + ((XhtmlInnerText*)this)->getText().substr(0, 10) + U"...\"";
            else
                content += U", content: \"" + ((XhtmlInnerText*)this)->getText() + U"\"";
        }

        // Attention: A 'std::u32string' streamed to console creates readable output, while a 'U"..."' literal streamed to console creates byte code!
        if (m_children != nullptr)
            std::wcout << indent << L"type: '" << getTypeNameU32() << L"'" << identity << parent << L", children: " << m_children->size() << std::endl;
        else
            std::wcout << indent << L"type: '" << getTypeNameU32() << L"'" << identity << parent << content << std::endl;

        if (complete == true)
        {
            ; // ToDo: trace attributes
        }
        indent.append(L" ");
        if (m_children != nullptr)
            for (XhtmlElement::Ptr e : *m_children)
                e->trace(indent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const XhtmlElement::Ptr XhtmlElement::createElementFromParseStr(std::vector<std::tuple<MessageType, String>>& messages,
        XhtmlElement::Ptr parent, const std::u32string& buffer, const size_t beginPosition,
        bool& isOpeningTag, bool& isClosingTag, bool& isInsideScript, size_t& processedLength, bool parseInnerText)
    {
        processedLength = 0;

        if (buffer.empty())
        {
            std::u32string message(U"XhtmlElement::createFromParseStr() -> Invalid buffer!");
            messages.push_back(std::make_tuple(MessageType::ERROR, message));
            return nullptr;
        }

        bool                             elementOpened = false;
        bool                             elementClosed = false;
        std::u32string                   typeName;
        size_t                           parsedLength = 0;
        size_t                           workPosition = beginPosition;
        size_t                           endPosition = beginPosition;
        std::vector<XhtmlAttribute::Ptr> attributesBuffer;

        // skip leading white-space characters
        while (ext::u32string::isSpace(buffer[workPosition]))
            workPosition++;

        // HTML tags always begin with a less-than symbol
        if (buffer[workPosition] != U'<')
        {
            std::u32string message(U"XhtmlElement::createFromParseStr() -> Invalid start character!");
            messages.push_back(std::make_tuple(MessageType::ERROR, message));
            return nullptr;
        }

        // skip tag's opening delimiter '<'
        workPosition++;

        // skip white-space before typename '<     typename>' or '<     >'
        while (ext::u32string::isSpace(buffer[workPosition]))
            workPosition++;

        // optimization for empty opening tags
        if (buffer[workPosition] == U'>')
        {
            if (!typeName.empty())
            {
                std::u32string message(U"XhtmlElement::createFromParseStr() -> Element type name should be empty, but isn't!");
                messages.push_back(std::make_tuple(MessageType::ERROR, message));
                return nullptr;
            }
            if (elementClosed)
            {
                std::u32string message(U"XhtmlElement::createFromParseStr() -> Closing tag should not be reached, but is!");
                messages.push_back(std::make_tuple(MessageType::ERROR, message));
                return nullptr;
            }

            processedLength = endPosition - workPosition;
            return createElementFromParseData(messages, isOpeningTag, isClosingTag, elementOpened, elementClosed, typeName,
                                              parent, beginPosition, endPosition, attributesBuffer);
        }


        // element type names always begin with an alphabet letter
        if (!ext::u32string::isAlpha(buffer[workPosition]))
        {
            elementClosed = (buffer[workPosition] == U'/');
            if (elementClosed)
                workPosition++;
            else if (buffer[workPosition] == U'!')
            {
                // the '!DOCTYPE' can start with an exclamation mark
            }
            else
            {
                std::u32string message(U"XhtmlElement::createFromParseStr() -> Element type name should start with an alphabet character, but doesn't!");
                messages.push_back(std::make_tuple(MessageType::ERROR, message));
                return nullptr;
            }
        }

        elementOpened = !elementClosed;
        endPosition = workPosition;
        do
        {
            // the '!doctype' can start with a '!'
            if (endPosition == workPosition && buffer[endPosition] == U'!')
                endPosition++;
            // element type name may contain letters (a-z, A-Z), digits (0-9),
            // underscores '_', hyphen '-', colons ':', and periods '.'
            if ((!ext::u32string::isAlnum(buffer[endPosition])) &&
                (buffer[endPosition] != U'-') && (buffer[endPosition] != U':') &&
                (buffer[endPosition] != U'_') && (buffer[endPosition] != U'.'))
            {
                // the script start tag '<script' is typically not flagged as isInsideScript
                if (!isInsideScript)
                {
                    if (endPosition == workPosition)
                    {
                        std::u32string message(U"XhtmlElement::createFromParseStr() -> Element is of length 0, but shouldn't!");
                        messages.push_back(std::make_tuple(MessageType::ERROR, message));
                        return nullptr;
                    }
                }

                // determine end of element type name
                if (buffer[endPosition] == 0 || ext::u32string::isSpace(buffer[endPosition]) ||
                    buffer[endPosition] == U'>' ||
                    (buffer[endPosition] == U'/' && (!elementClosed)))
                {
                    break;
                }

                // any other character will fail parsing process
                std::u32string message(U"XhtmlElement::createFromParseStr() -> Element type name should consist of ");
                message.append(U"alpha-numerical characters, underscores, hyphen, colons and periods only, but doesn't!");
                messages.push_back(std::make_tuple(MessageType::ERROR, message));
                return nullptr;
            }

            endPosition++;
        } while (true);

        // store element type name for later use
        typeName = buffer.substr(workPosition, endPosition - workPosition);

        // ---------------------------------------------------------------------------

        // if is any other than /script
        if (isInsideScript)
        {
            if (!elementClosed)
            {
                std::u32string message(U"XhtmlElement::parseFromStr() -> Element of type '");
                message.append(typeName).append(U"' (anticipated type is script) should be closed, but isn't!");
                message.append(U" We assume it is a tag inside the script code and continue.");
                messages.push_back(std::make_tuple(MessageType::WARNING, message));
                return nullptr;
            }

            if (ext::u32string::compareNoCase(typeName, U"script"))
            {
                std::u32string message(U"XhtmlElement::createFromParseStr() -> Element of type '");
                message.append(typeName).append(U"' (anticipated type is script) closing tag expected, but isn't!");
                message.append(U" We assume it is a tag inside the script code and continue.");
                messages.push_back(std::make_tuple(MessageType::WARNING, message));
                return nullptr;
            }
        }

        // skip white-space after typename '</typename     >' or '<typename     >' or '<typename     attribute-list'
        while (ext::u32string::isSpace(buffer[endPosition]))
            endPosition++;

        // if this is a closing tag (that has no attributes by definition)
        if (elementClosed)
        {
            // in a closing tag, there can be only one symbol after
            // tag name, i.e., the tag end delimiter '>'. Anything
            // else will result in parsing failure
            if (buffer[endPosition] != U'>')
            {
                std::u32string message(U"XhtmlElement::createFromParseStr() -> Element of type '");
                message.append(typeName).append(U"' closing tag end delimiter expected, but isn't!");
                messages.push_back(std::make_tuple(MessageType::ERROR, message));
                return nullptr;
            }

            // skip tag's ending delimiter
            endPosition++;

            if (typeName.empty())
            {
                std::u32string message(U"XhtmlElement::createFromParseStr() -> Element type name should not be empty "
                                       U"for the closing tag, but is!");
                messages.push_back(std::make_tuple(MessageType::ERROR, message));
                return nullptr;
            }
            processedLength = endPosition - beginPosition;
            return createElementFromParseData(messages, isOpeningTag, isClosingTag, elementOpened, elementClosed, typeName,
                                              parent, beginPosition, endPosition, attributesBuffer);
        }

        // determine element inner-text or attribute/value pairs
        if (buffer[endPosition] != U'>')
        {
            workPosition = endPosition;

            // skip white-space characters after typename
            while (ext::u32string::isSpace(buffer[workPosition]))
                workPosition++;

            // parse inner-text or attribute/value pairs
            parsedLength = 0;
            if (parseInnerText)
            {
                // delegate parsing process
                parsedLength = XhtmlElement::createAttributesFromParseStr(messages, attributesBuffer, buffer, workPosition);
            }

            // attribute (key/value pair) parsing is disabled, returns without any attribute or has parsing error
            if (parsedLength == 0)
            {
                attributesBuffer.clear();
                endPosition = workPosition + ext::u32string::find(buffer, U"/>", workPosition);
                if (endPosition == workPosition)
                {
                    std::u32string message(U"XhtmlElement::createFromParseStr() -> Element of type '");
                    message.append(typeName).append(U"' should be delimited by '/>', but isn't!");
                    messages.push_back(std::make_tuple(MessageType::ERROR, message));
                    return nullptr;
                }
            }
            else
            {
                endPosition = workPosition + parsedLength;

                // skip white-space characters after attribute/value pairs
                while (ext::u32string::isSpace(buffer[endPosition]))
                    endPosition++;

                // tag's ending delimiter could not be found?
                if (buffer[endPosition] == 0)
                {
                    attributesBuffer.clear();

                    std::u32string message(U"XhtmlElement::createFromParseStr() -> Element of type '");
                    message.append(typeName).append(U"' should be delimited after the inner text, but isn't (EOF found)!");
                    messages.push_back(std::make_tuple(MessageType::ERROR, message));
                    return nullptr;
                }
            }

            // a tag like this one: <br/>
            if (buffer[endPosition] == U'/')
            {
                if (!elementOpened)
                {
                    std::u32string message(U"XhtmlElement::createFromParseStr() -> Element of type '");
                    message.append(typeName).append(U"' should be opened before the occurence of the end tag symbol'/', but isn't!");
                    messages.push_back(std::make_tuple(MessageType::ERROR, message));
                    return nullptr;
                }
                elementClosed = true;
                endPosition++;
            }
        }
        // skip white-space after attribute/value pairs for example '<tagname atrib="xx" /  >'
        while (ext::u32string::isSpace(buffer[endPosition]))
            endPosition++;

        // elements always end with a greater-than '>' symbol
        if (buffer[endPosition] != U'>')
        {
            attributesBuffer.clear();

            std::u32string message(U"XhtmlElement::createFromParseStr() -> Element of type '");
            message.append(typeName).append(U"' should be delimited by '>', but isn't!");
            messages.push_back(std::make_tuple(MessageType::ERROR, message));
            return nullptr;
        }
        else
            endPosition++;

        processedLength = endPosition - beginPosition;
        return createElementFromParseData(messages, isOpeningTag, isClosingTag, elementOpened, elementClosed, typeName,
                                          parent, beginPosition, endPosition, attributesBuffer);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const XhtmlElement::Ptr XhtmlElement::createElementFromParseData(std::vector<std::tuple<MessageType, String>>& messages,
        bool& isOpeningTag, bool& isClosingTag, bool elementOpened, bool elementClosed, std::u32string& typeName,
        XhtmlElement::Ptr& parent, const size_t& beginPosition, const size_t& endPosition,
        std::vector<XhtmlAttribute::Ptr>& attributesBuffer)
    {
        isOpeningTag = elementOpened;
        isClosingTag = elementClosed;

        ext::u32string::trimLeft(typeName);
        ext::u32string::trimRight(typeName);
        ext::u32string::remove(typeName, U'\n');
        ext::u32string::replace(typeName, U'\r', U' ');
        ext::u32string::replace(typeName, U'\t', U' ');
        ext::u32string::replace(typeName, U'\v', U' ');

#ifdef LOG_ELEMENT
        std::wcout << U"Set name '" << typeName << U"' to" << (isOpeningTag ? " opening" : "") << (isClosingTag ? " closing" : "") << " element with " << attributesBuffer.size() << " attributes.\r\n";
#endif

        auto element = XhtmlElement::createAuto(ext::u32string::stringfromu32string(typeName).c_str(), parent, beginPosition, endPosition, elementClosed && elementOpened);
        element->addAttributes(attributesBuffer);
        attributesBuffer.clear();

        if (ext::string::strcasecmp(element->getTypeName(), "empty") == 0)
        {
            std::u32string message(U"XhtmlElement::createFromParseData() -> Couldn't recognize element type for tag type '");
            message.append(typeName).append(U"', continue with 'empty' - but this is unsafe!");
            messages.push_back(std::make_tuple(MessageType::WARNING, message));
        }

        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    size_t XhtmlElement::createAttributesFromParseStr(std::vector<std::tuple<tgui::MessageType, String>>& messages,
        std::vector<XhtmlAttribute::Ptr>& attributes, const std::u32string& buffer, const size_t beginPosition)
    {
        if (buffer.empty())
        {
            std::u32string message(U"XhtmlElement::parseAttributesFromStr() -> Invalid buffer!");
            messages.push_back(std::make_tuple(MessageType::ERROR, message));
            return 0;
        }

        std::vector<XhtmlAttribute::Ptr> workAttributes;
        const size_t                     remainingPositions = buffer.size() - beginPosition;
        size_t                           offsetFromBegin = 0;
        size_t                           offsetFromWorkPosition = 0;

        do
        {
            // try to parse an attribute/value pair from the rest of the string
            XhtmlAttribute::Ptr workAttribute = XhtmlAttribute::createFromStr(messages, buffer, beginPosition + offsetFromBegin, offsetFromWorkPosition);
            if (offsetFromWorkPosition == 0)
            {
                // parsing not successful
                if (offsetFromBegin == 0)
                    // not even one attribute found
                    return 0;
                else
                    // no further attributes
                    break;
            }

            // add attribute/value pair to collection (offsetFromWorkPosition > beginPosition + offsetFromBegin)
            workAttributes.push_back(workAttribute);

            // advance work position
            offsetFromBegin += offsetFromWorkPosition;
        }
        // do we still have something in the buffer to parse?
        while (offsetFromBegin < remainingPositions);

        for (XhtmlAttribute::Ptr attribute : workAttributes)
            attributes.push_back(attribute);
        workAttributes.clear();

        return offsetFromBegin;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void XhtmlStyle::createEntriesFromParseData(std::vector<std::tuple<MessageType, String>>& messages, const std::u32string& buffer)
    {
        auto styleEntries = ext::u32string::split(buffer, U'}', true);
        for (auto styleEntry : styleEntries)
        {
            ext::u32string::trimLeft(styleEntry);
            ext::u32string::trimRight(styleEntry);

            if (styleEntry.size() == 0)
                continue;

            auto styleEntryParts = ext::u32string::split(styleEntry, U'{', true);
            if (styleEntryParts.size() != 2)
                continue;

            ext::u32string::trimLeft(styleEntryParts[0]);
            ext::u32string::trimRight(styleEntryParts[0]);
            ext::u32string::trimLeft(styleEntryParts[1]);
            ext::u32string::trimRight(styleEntryParts[1]);

            if (styleEntryParts[0].size() == 0 || styleEntryParts[1].size() == 0)
                continue;

            auto se = std::make_shared<XhtmlStyleEntry>();
            se->putValue(messages, styleEntryParts[1]);
            m_styles.insert_or_assign(styleEntryParts[0], se);
        }
        return;
    }

    XhtmlStyleEntry::Ptr XhtmlStyle::getEntry(const char* typeName, const std::u32string& className)
    {
        if (typeName != nullptr && strlen(typeName) > 0 && className.size() > 0)
        {
            auto styleName = ext::u32string::u32stringfromstring(typeName) + U"." + className;
            return ( m_styles.find(styleName) != m_styles.end() ? m_styles[styleName] : nullptr);
        }
        else if (typeName != nullptr && (strlen(typeName) > 0 || className.size() > 0))
        {
            auto styleName = ext::u32string::u32stringfromstring(typeName) + className;
            return (m_styles.find(styleName) != m_styles.end() ? m_styles[styleName] : nullptr);
        }
        return nullptr;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlStyleEntry::Ptr XhtmlStyleableNoncontainerElement::getStyleEntry()
    {
        for (auto it = m_attributes->begin(); it != m_attributes->end(); it++)
        {
            if (ext::u32string::compareNoCase((*it)->getName(), XhtmlStyleEntry::TypeName) == 0)
                return std::dynamic_pointer_cast<XhtmlStyleEntry>((*it));
        }
        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlStyleEntry::Ptr XhtmlStyleableContainerElement::getStyleEntry()
    {
        for (auto it = m_attributes->begin(); it != m_attributes->end(); it++)
        {
            if (ext::u32string::compareNoCase((*it)->getName(), XhtmlStyleEntry::TypeName) == 0)
                return std::dynamic_pointer_cast<XhtmlStyleEntry>((*it));
        }
        return nullptr;
    }

}
