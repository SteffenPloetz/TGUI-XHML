// The SFML graphic module (sf::RenderWindow) is more sophisticated than the SFML OpenGL window (sf::Window).
// - It adds features to draw SFML specific entities, but it requires extra care when mixing with native OpenGL calls.
// - It accepts OpenGL version 1.1 and above, while the SFML window (sf::Window) requires OpenGL version 3.3 at least.
// The SFML OpenGL window (sf::Window) doesn't necessarily require the TGUI_HAS_FONT_BACKEND_FREETYPE to be set.
// It works fine with the TGUI_HAS_FONT_BACKEND_SFML_GRAPHICS, if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS is also set.
// See "https://www.sfml-dev.org/tutorials/2.5/window-opengl.php" (last section) for details.

#include <fstream>
#include <thread>
#include <cmath>
#include <cstdint>

#include <TGUI/Config.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Base64.hpp>

#ifdef TGUI_HAS_BACKEND_SFML_GRAPHICS
#include <TGUI/Backend/SFML-Graphics.hpp>
#else
#include <TGUI/Backend/SFML-OpenGL3.hpp>
#endif

#include "TGUI/Xhtml/StringHelper.hpp"
#include "TGUI/Xhtml/Dom/XhtmlEntityResolver.hpp"
#include "TGUI/Xhtml/Dom/XhtmlAttributes.hpp"
#include "TGUI/Xhtml/Dom/XhtmlElements.hpp"
#include "TGUI/Xhtml/Dom/XhtmlParser.hpp"
#include "TGUI/Xhtml/Renderers/FormattedTextRenderer.hpp"
#include "TGUI/Xhtml/Widgets/FormattedElements.hpp"
#include "TGUI/Xhtml/Widgets/FormattedXhtmlDocument.hpp"
#include "TGUI/Xhtml/Widgets/FormattedTextView.hpp"

#define UNUSED(x) (void)(x)

using namespace tgui;
using namespace tgui::xhtml;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief My application main form
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class XHTMLViewerMainForm
{
private:
    sf::Window&             m_window;                       //!< The main frame window
    BackendGui&             m_gui;                          //!< The underlying GUI, that organizes the widgets
    Color                   m_clearColor;                   //!< The main form background color
    TabContainer::Ptr       m_pageTabContainer;             //!< The page switcher
    FormattedTextView::Ptr  m_formattedTextView1;           //!< The XHTML formatted text view 1
    FormattedTextView::Ptr  m_formattedTextView2;           //!< The XHTML formatted text view 2
    Label::Ptr              m_zoomLabel1;                   //!< The zoom display for text view 1
    Label::Ptr              m_zoomLabel2;                   //!< The zoom display for text view 2

    const float             m_tabFixedSize = 150.0f;        //!< The fixed size of the tab.

private:
    XHTMLViewerMainForm() = default;

public:
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Constructor
    ///
    /// @param window     The main form window
    /// @param gui        The back-end gui (containing native window and renderer)
    /// @param clearColor The background color of the back-end window
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    XHTMLViewerMainForm(sf::Window& window, BackendGui& gui, Color clearColor = Color(240, 240, 240)) :
        m_window(window),  m_gui(gui), m_clearColor(clearColor), m_pageTabContainer(), m_formattedTextView1(), m_formattedTextView2()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Returns the back-end GUI (containing native window and renderer)
    ///
    /// @return Back-end GUI (containing native window and renderer)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BackendGui& getGui() const
    {   return m_gui;    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Returns the background color of the back-end window
    ///
    /// @return Background color of the back-end window
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Color getClearColor() const
    {   return m_clearColor;   }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Build the Document-Object-Model (DOM) for the indicated view.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void buildDom(FormattedXhtmlDocument::Ptr formattedDocument1)
    {
        // create a base 64 stream from PNG for test purposes
        // std::string filePath("/home/steffen/Projects/CodeBlocks/XHTMLViewerTGUI/html5.png"); //"C:\\Projects\\XHTMLViewerTGUI\\html5.png"
        // std::size_t fileSize = 0;
        // std::unique_ptr<std::uint8_t[]> data = readFileToMemory(filePath, fileSize));
        // if (fileSize > 0)
        //     std::string b64 = base64Encode((const std::uint8_t*)data.data(), data.size());

        const String imageSource = "data:image/png;base64,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";

        auto htmlRoot1 = formattedDocument1->getRootElement();
        auto htmlHead  = XhtmlElement::createHead(htmlRoot1);
        auto thmlStyle = XhtmlElement::createStyle(htmlHead);

        thmlStyle->setEntry(U"cp", std::make_shared<XhtmlStyleEntry>(XhtmlStyleEntryInitializer().SetForeColor(Color(U"#666666")).SetFontFamily(U"Monospace").SetFontStyle(static_cast<TextStyle>(TextStyle::Italic  | TextStyle::Bold))));
        thmlStyle->setEntry(U"kt", std::make_shared<XhtmlStyleEntry>(XhtmlStyleEntryInitializer().SetForeColor(Color(U"#666666")).SetFontFamily(U"Monospace").SetFontStyle(static_cast<TextStyle>(TextStyle::Regular | TextStyle::Bold))));
        thmlStyle->setEntry(U"nf", std::make_shared<XhtmlStyleEntry>(XhtmlStyleEntryInitializer().SetForeColor(Color(U"#aa4400")).SetFontFamily(U"Monospace").SetFontStyle(static_cast<TextStyle>(TextStyle::Regular | TextStyle::Bold))));
        thmlStyle->setEntry(U"p",  std::make_shared<XhtmlStyleEntry>(XhtmlStyleEntryInitializer().SetForeColor(Color(U"#666666")).SetFontFamily(U"Monospace").SetFontStyle(static_cast<TextStyle>(TextStyle::Regular | TextStyle::Regular))));
        thmlStyle->setEntry(U"n",  std::make_shared<XhtmlStyleEntry>(XhtmlStyleEntryInitializer().SetForeColor(Color(U"#666666")).SetFontFamily(U"Monospace").SetFontStyle(static_cast<TextStyle>(TextStyle::Regular | TextStyle::Regular))));
        thmlStyle->setEntry(U"o",  std::make_shared<XhtmlStyleEntry>(XhtmlStyleEntryInitializer().SetForeColor(Color(U"#666666")).SetFontFamily(U"Monospace").SetFontStyle(static_cast<TextStyle>(TextStyle::Regular | TextStyle::Bold))));
        thmlStyle->setEntry(U"mi", std::make_shared<XhtmlStyleEntry>(XhtmlStyleEntryInitializer().SetForeColor(Color(U"#009999")).SetFontFamily(U"Monospace").SetFontStyle(static_cast<TextStyle>(TextStyle::Regular | TextStyle::Regular))));
        thmlStyle->setEntry(U"s",  std::make_shared<XhtmlStyleEntry>(XhtmlStyleEntryInitializer().SetForeColor(Color(U"#cc4444")).SetFontFamily(U"Monospace").SetFontStyle(static_cast<TextStyle>(TextStyle::Regular | TextStyle::Regular))));
        thmlStyle->setEntry(U"cl", std::make_shared<XhtmlStyleEntry>(XhtmlStyleEntryInitializer().SetForeColor(Color(U"#aaaaaa")).SetFontFamily(U"Monospace").SetFontStyle(static_cast<TextStyle>(TextStyle::Regular | TextStyle::Regular))));

        auto grayBG = std::make_shared<XhtmlStyleEntry>(); grayBG->setBackgroundColor(Color(208, 208, 208, 255));
        auto body   = XhtmlElement::createBody(htmlRoot1, grayBG);

        auto image1 = XhtmlElement::createImage(nullptr);
        image1->addAttribute(std::make_shared<XhtmlAttribute>(U"src", imageSource.toUtf32()));
        image1->addAttribute(std::make_shared<XhtmlAttribute>(U"alt", U"HTML 5"));
        image1->addAttribute(std::make_shared<XhtmlAttribute>(U"width", U"36"));
        image1->addAttribute(std::make_shared<XhtmlAttribute>(U"height", U"36"));
        auto image2 = XhtmlElement::createImage(nullptr);
        image2->addAttribute(std::make_shared<XhtmlAttribute>(U"src", imageSource.toUtf32()));
        image2->addAttribute(std::make_shared<XhtmlAttribute>(U"alt", U"HTML 5"));
        image2->addAttribute(std::make_shared<XhtmlAttribute>(U"width", U"24"));
        image2->addAttribute(std::make_shared<XhtmlAttribute>(U"height", U"24"));
        auto image3 = XhtmlElement::createImage(nullptr);
        image3->addAttribute(std::make_shared<XhtmlAttribute>(U"src", imageSource.toUtf32()));
        image3->addAttribute(std::make_shared<XhtmlAttribute>(U"alt", U"HTML 5"));
        image3->addAttribute(std::make_shared<XhtmlAttribute>(U"width", U"48"));
        image3->addAttribute(std::make_shared<XhtmlAttribute>(U"height", U"48"));
        auto image4 = XhtmlElement::createImage(nullptr);
        image4->addAttribute(std::make_shared<XhtmlAttribute>(U"src", U"file://./testdata/url-icon.png"));
        image4->addAttribute(std::make_shared<XhtmlAttribute>(U"alt", U"URL icon"));
        image4->addAttribute(std::make_shared<XhtmlAttribute>(U"width", U"32"));
        image4->addAttribute(std::make_shared<XhtmlAttribute>(U"height", U"32"));
        auto h1_HTML = XhtmlElement::createH1(nullptr, {XhtmlElement::createInnerText(nullptr, U"FormattedTextArea - XHTML markup "),
                                                        XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#008800")), XhtmlElement::createInnerText(nullptr, U"tests")),
                                                        XhtmlElement::createInnerText(nullptr, U" (h1)")});
        h1_HTML->addAttribute(std::make_shared<XhtmlAttribute>(U"id", U"HTML-MU"));
        auto h1_BG01 = std::make_shared<XhtmlStyleEntry>();
        h1_BG01->setBackgroundColor(Color(U"#DFD0D0"))
                .setMargin(FourDimSize(SizeType::Pixel, 16.0f))
                .setBorderStyle(FourDimBorderStyle(BorderStyle::Dashed))
                .setBorderWidth(FourDimSize(SizeType::Pixel, 5.0f))
                .setBorderColor(Color(U"#BFB0B0"))
                .setPadding(FourDimSize(SizeType::Pixel, 16.0f));
        h1_HTML->addAttribute(h1_BG01);
        auto h2_TOC = XhtmlElement::createH2(nullptr, XhtmlElement::createInnerText(nullptr, U"Table of contents (h2)"));
        h2_TOC->addAttribute(std::make_shared<XhtmlAttribute>(U"id", U"TOC"));
        auto a_BG = std::make_shared<XhtmlStyleEntry>();
        a_BG->setBackgroundColor(Color(U"#DFD0D0"));
        auto a01 = XhtmlElement::createAnchor(nullptr, a_BG, XhtmlElement::createInnerText(nullptr, U"Introduction to the FormattedTextArea (h2)"));
        a01->addAttribute(std::make_shared<XhtmlAttribute>(U"href", U"#INTRO"));
        auto a02 = XhtmlElement::createAnchor(nullptr, a_BG, XhtmlElement::createInnerText(nullptr, U"Various text tests (h2)"));
        a02->addAttribute(std::make_shared<XhtmlAttribute>(U"href", U"#TEXTS"));
        auto a03 = XhtmlElement::createAnchor(nullptr, a_BG, XhtmlElement::createInnerText(nullptr, U"Floating text and block text tests (h3)"));
        a03->addAttribute(std::make_shared<XhtmlAttribute>(U"href", U"#FLOATING"));
        auto a04 = XhtmlElement::createAnchor(nullptr, a_BG, XhtmlElement::createInnerText(nullptr, U"List tests (h3)"));
        a04->addAttribute(std::make_shared<XhtmlAttribute>(U"href", U"#LISTS"));
        auto a05 = XhtmlElement::createAnchor(nullptr, a_BG, XhtmlElement::createInnerText(nullptr, U"Code and style-sheet tests (h3)"));
        a05->addAttribute(std::make_shared<XhtmlAttribute>(U"href", U"#CODE"));
        auto h2_BG00 = std::make_shared<XhtmlStyleEntry>();
        h2_BG00->setBackgroundColor(Color(U"#D0DAD0"))
                .setMargin(FourDimSize(SizeType::Pixel, 12.0f))
                .setBorderStyle(FourDimBorderStyle(BorderStyle::Dotted))
                .setBorderWidth(FourDimSize(SizeType::Pixel, 3.0f))
                .setBorderColor(Color(U"#B0BAB0"))
                .setPadding(FourDimSize(SizeType::Pixel, 12.0f));
        h2_TOC->addAttribute(h2_BG00);
        auto h2_INTRO = XhtmlElement::createH2(nullptr, {XhtmlElement::createInnerText(nullptr, U"Introduction to the "),
                                                         XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#880000")), XhtmlElement::createInnerText(nullptr, U"FormattedTextArea")),
                                                         XhtmlElement::createInnerText(nullptr, U" widget (h2)")});
        h2_INTRO->addAttribute(std::make_shared<XhtmlAttribute>(U"id", U"INTRO"));
        auto h2_BG01 = std::make_shared<XhtmlStyleEntry>();
        h2_BG01->setBackgroundColor(Color(U"#D0DAD0"))
                .setMargin(FourDimSize(SizeType::Pixel, 12.0f))
                .setBorderStyle(FourDimBorderStyle(BorderStyle::Dotted))
                .setBorderWidth(FourDimSize(SizeType::Pixel, 3.0f))
                .setBorderColor(Color(U"#B0BAB0"))
                .setPadding(FourDimSize(SizeType::Pixel, 12.0f));
        h2_INTRO->addAttribute(h2_BG01);
        XhtmlElement::addChildrenAndSetPatent(body, {
            h1_HTML,
            h2_TOC,
            a01,
            XhtmlElement::createBreak(),
            a02,
            XhtmlElement::createBreak(),
            XhtmlElement::createInnerText(nullptr, U"   "),
            a03,
            XhtmlElement::createBreak(),
            XhtmlElement::createInnerText(nullptr, U"   "),
            a04,
            XhtmlElement::createBreak(),
            XhtmlElement::createInnerText(nullptr, U"   "),
            a05,
            h2_INTRO,
            XhtmlElement::createInnerText(nullptr, U"The "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#880000")), XhtmlElement::createInnerText(nullptr, U"FormattedTextArea")),
            XhtmlElement::createInnerText(nullptr, U" widget displays markup formatted text. Currently supported is a sub-set of XHTML, that includes the HTML block elements "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;header&gt;")),
            XhtmlElement::createInnerText(nullptr, U"header"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/header&gt;")),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;body&gt;")),
            XhtmlElement::createInnerText(nullptr, U"body"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/body&gt;")),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;p&gt;")),
            XhtmlElement::createInnerText(nullptr, U"paragraph"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/p&gt;")),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;div&gt;")),
            XhtmlElement::createInnerText(nullptr, U"division"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/div&gt;")),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;span&gt;")),
            XhtmlElement::createInnerText(nullptr, U"span"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/span&gt;")),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;h1&gt;")),
            XhtmlElement::createInnerText(nullptr, U"headline 1"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/h1&gt;")),
            XhtmlElement::createInnerText(nullptr, U" ... "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;h6&gt;")),
            XhtmlElement::createInnerText(nullptr, U"headline 6"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/h6&gt;")),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;em&gt;")),
            XhtmlElement::createInnerText(nullptr, U"emphasized"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/em&gt;")),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;strong&gt;")),
            XhtmlElement::createInnerText(nullptr, U"strong"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/strong&gt;")),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;i&gt;")),
            XhtmlElement::createInnerText(nullptr, U"italic"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/i&gt;")),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;b&gt;")),
            XhtmlElement::createInnerText(nullptr, U"bold"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/b&gt;")),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;u&gt;")),
            XhtmlElement::createInnerText(nullptr, U"underline"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/u&gt;")),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;sup&gt;")),
            XhtmlElement::createInnerText(nullptr, U"superscript"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/sup&gt;")),
            XhtmlElement::createInnerText(nullptr, U" for "),
            XhtmlElement::createSuperscript(nullptr, U"superscript text"),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;sub&gt;")),
            XhtmlElement::createInnerText(nullptr, U"subscript"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/sub&gt;")),
            XhtmlElement::createInnerText(nullptr, U" for "),
            XhtmlElement::createSubscript(nullptr, U"subscript text"),
            XhtmlElement::createInnerText(nullptr, U", the XHTML empty elements "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;br/&gt;")),
            XhtmlElement::createInnerText(nullptr, U" for line break and "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;image/&gt;")),
            XhtmlElement::createInnerText(nullptr, U" for images like "),
            image1,
            XhtmlElement::createInnerText(nullptr, U" (100%) and XHTML "),
            XhtmlElement::createEmphasized(nullptr, U"inner text"),
            XhtmlElement::createInnerText(nullptr, U" of any block. In addition to that, all block elements support the "),
            XhtmlElement::createEmphasized(nullptr, U"style"),
            XhtmlElement::createInnerText(nullptr, U" attribute (currently the color style only) and the images can be zoomed like this "),
            image2,
            XhtmlElement::createInnerText(nullptr, U" (67%) or this "),
            image3,
            XhtmlElement::createInnerText(nullptr, U" (133%).")});

        auto h2_TEXTS = XhtmlElement::createH2(nullptr, {XhtmlElement::createInnerText(nullptr, U"Various text "),
                                                         XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#008800")), XhtmlElement::createInnerText(nullptr, U"tests")),
                                                         XhtmlElement::createInnerText(nullptr, U" (h2)")});
        h2_TEXTS->addAttribute(std::make_shared<XhtmlAttribute>(U"id", U"TEXTS"));
        auto h2_BG02 = std::make_shared<XhtmlStyleEntry>();
        h2_BG02->setBackgroundColor(Color(U"#D0DAD0"))
                .setMargin(FourDimSize(SizeType::Pixel, 12.0f))
                .setBorderStyle(FourDimBorderStyle(BorderStyle::Dotted))
                .setBorderWidth(FourDimSize(SizeType::Pixel, 3.0f))
                .setBorderColor(Color(U"#B0BAB0"))
                .setPadding(FourDimSize(SizeType::Pixel, 12.0f));
        h2_TEXTS->addAttribute(h2_BG02);
        auto h3_FLOATING = XhtmlElement::createH3(nullptr, {XhtmlElement::createInnerText(nullptr, U"Floating text and block text "),
                                                            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#008800")), XhtmlElement::createInnerText(nullptr, U"tests")),
                                                            XhtmlElement::createInnerText(nullptr, U" (h3)")});
        h3_FLOATING->addAttribute(std::make_shared<XhtmlAttribute>(U"id", U"FLOATING"));
        auto h3_BG01 = std::make_shared<XhtmlStyleEntry>();
        h3_BG01->setBackgroundColor(Color(U"#D0D0DF"))
                .setMargin(FourDimSize(SizeType::Pixel, 8.0f))
                .setBorderStyle(FourDimBorderStyle(BorderStyle::Solid))
                .setBorderWidth(FourDimSize(SizeType::Pixel, 1.0f))
                .setBorderColor(Color(U"#B0B0BF"))
                .setPadding(FourDimSize(SizeType::Pixel, 8.0f));
        h3_FLOATING->addAttribute(h3_BG01);
        auto h4_CANGE_WIDTH = XhtmlElement::createH4(nullptr, {XhtmlElement::createInnerText(nullptr, U"Change the witdth of the window to "),
                                                               XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#008800")), XhtmlElement::createInnerText(nullptr, U"test")),
                                                               XhtmlElement::createInnerText(nullptr, U" automatic line break functionality. (h4)") });
        h4_CANGE_WIDTH->addAttribute(std::make_shared<XhtmlAttribute>(U"id", U"CHANGE_WIDTH"));
        auto h4_BG01 = std::make_shared<XhtmlStyleEntry>();
        h4_BG01->setBackgroundColor(Color(U"#D8D0D8"))
                .setMargin(FourDimSize(SizeType::Pixel, 4.0f))
                .setBorderStyle(FourDimBorderStyle(BorderStyle::Solid))
                .setBorderWidth(FourDimSize(SizeType::Pixel, 1.0f))
                .setBorderColor(Color(U"#B8B0B8"))
                .setPadding(FourDimSize(SizeType::Pixel, 4.0f));
        h4_CANGE_WIDTH->addAttribute(h4_BG01);
        XhtmlElement::addChildrenAndSetPatent(body, {
            h2_TEXTS,
            h3_FLOATING,
            h4_CANGE_WIDTH,
            XhtmlElement::createInnerText(nullptr, U"This is the first "),
            XhtmlElement::createUnderline(nullptr, U"floating"),
            XhtmlElement::createInnerText(nullptr, U" text line (not embedded in a "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;span&gt;")),
            XhtmlElement::createInnerText(nullptr, U"span"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/span&gt;")),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;div&gt;")),
            XhtmlElement::createInnerText(nullptr, U"division"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/div&gt;")),
            XhtmlElement::createInnerText(nullptr, U" or "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;p&gt;")),
            XhtmlElement::createInnerText(nullptr, U"paragraph"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/p&gt;")),
            XhtmlElement::createInnerText(nullptr, U"). It is followed by a line break."),
            XhtmlElement::createBreak(),
            XhtmlElement::createInnerText(nullptr, U"This is the second "),
            XhtmlElement::createUnderline(nullptr, U"floating"),
            XhtmlElement::createInnerText(nullptr, U" text line (not embedded in a "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;span&gt;")),
            XhtmlElement::createInnerText(nullptr, U"span"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/span&gt;")),
            XhtmlElement::createInnerText(nullptr, U", "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;div&gt;")),
            XhtmlElement::createInnerText(nullptr, U"division"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/div&gt;")),
            XhtmlElement::createInnerText(nullptr, U" or "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;p&gt;")),
            XhtmlElement::createInnerText(nullptr, U"paragraph"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/p&gt;")),
            XhtmlElement::createInnerText(nullptr, U").")});
        auto div_STY = std::make_shared<XhtmlStyleEntry>(XhtmlStyleEntryInitializer().SetForeColor(Color::Blue).SetBackColor(Color(U"#D8D0D8")));
        auto div01 = XhtmlElement::createDivision(nullptr, div_STY);
        XhtmlElement::addChildrenAndSetPatent(div01, {
            XhtmlElement::createInnerText(nullptr, U"This is the first "),
            XhtmlElement::createUnderline(nullptr, U"block"),
            XhtmlElement::createInnerText(nullptr, U" text (embedded in a "),
            XhtmlElement::createSpan(nullptr,std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;div&gt;")),
            XhtmlElement::createInnerText(nullptr, U"division"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/div&gt;")),
            XhtmlElement::createInnerText(nullptr, U"). It shall demonstrate no vertical spacing around this block and automatic line breaks.")});
        XhtmlElement::addChildAndSetPatent(body, div01);
        auto par_STY = std::make_shared<XhtmlStyleEntry>();
        par_STY->setColor(Color(U"#880088")).setBackgroundColor(Color(U"#D8D8D0"));
        auto par01 = XhtmlElement::createParagraph(nullptr, par_STY);
        XhtmlElement::addChildrenAndSetPatent(par01, {
            XhtmlElement::createInnerText(nullptr, U"This is the second "),
            XhtmlElement::createUnderline(nullptr, U"block"),
            XhtmlElement::createInnerText(nullptr, U" text (embedded in a "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;p&gt;")),
            XhtmlElement::createInnerText(nullptr, U"paragraph"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/p&gt;")),
            XhtmlElement::createInnerText(nullptr, U"). It shall demonstrate vertical spacing around this block and a floating text and besides automatic line breaks.")});
        XhtmlElement::addChildAndSetPatent(body, par01);
        auto par02 = XhtmlElement::createParagraph(nullptr);
        XhtmlElement::addChildrenAndSetPatent(par02, {
            XhtmlElement::createInnerText(nullptr, U"While the images so far have been set from a base64 memory stream, this image is loaded from a PNG file (which might not work in browsers): "),
            image4});
        XhtmlElement::addChildAndSetPatent(body, par02);
        XhtmlElement::addChildrenAndSetPatent(body, {
            XhtmlElement::createInnerText(nullptr, U"This is the "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color::Green), {XhtmlElement::createInnerText(nullptr, U"final "),
                                                                                                XhtmlElement::createUnderline(nullptr, U"floating"),
                                                                                                XhtmlElement::createInnerText(nullptr, U" text line ")}),
            XhtmlElement::createInnerText(nullptr, U"with a "),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;span&gt;")),
            XhtmlElement::createInnerText(nullptr, U"span"),
            XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#884400")), XhtmlElement::createInnerText(nullptr, U"&lt;/span&gt;")),
            XhtmlElement::createInnerText(nullptr, U" (inline-style = color:green).")});

        auto h3_LISTS = XhtmlElement::createH3(nullptr, {XhtmlElement::createInnerText(nullptr, U"List "),
                                                         XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#008800")), XhtmlElement::createInnerText(nullptr, U"tests")),
                                                         XhtmlElement::createInnerText(nullptr, U" (h3)")});
        h3_LISTS->addAttribute(std::make_shared<XhtmlAttribute>(U"id", U"LISTS"));
        auto h3_BG02 = std::make_shared<XhtmlStyleEntry>();
        h3_BG02->setBackgroundColor(Color(U"#D0D0DF"))
                .setMargin(FourDimSize(SizeType::Pixel, 8.0f))
                .setBorderStyle(FourDimBorderStyle(BorderStyle::Solid))
                .setBorderWidth(FourDimSize(SizeType::Pixel, 1.0f))
                .setBorderColor(Color(U"#B0B0BF"))
                .setPadding(FourDimSize(SizeType::Pixel, 8.0f));
        h3_LISTS->addAttribute(h3_BG02);
        auto h4_NESTED_LIST = XhtmlElement::createH4(nullptr, {XhtmlElement::createInnerText(nullptr, U"Nested list "),
                                                               XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#008800")), XhtmlElement::createInnerText(nullptr, U"tests")),
                                                               XhtmlElement::createInnerText(nullptr, U" (h4)") });
        h4_NESTED_LIST->addAttribute(std::make_shared<XhtmlAttribute>(U"id", U"NESTED_LIST"));
        auto h4_BG02 = std::make_shared<XhtmlStyleEntry>();
        h4_BG02->setBackgroundColor(Color(U"#D8D0D8"))
                .setMargin(FourDimSize(SizeType::Pixel, 4.0f))
                .setBorderStyle(FourDimBorderStyle(BorderStyle::Solid))
                .setBorderWidth(FourDimSize(SizeType::Pixel, 1.0f))
                .setBorderColor(Color(U"#B8B0B8"))
                .setPadding(FourDimSize(SizeType::Pixel, 4.0f));
        h4_NESTED_LIST->addAttribute(h4_BG02);
        XhtmlElement::addChildrenAndSetPatent(body, {
            h3_LISTS,
            h4_NESTED_LIST,
            XhtmlElement::createInnerText(nullptr, U"Example of unordered lists:")});
        auto yellowBG = std::make_shared<XhtmlStyleEntry>();
        yellowBG->setBackgroundColor(Color(255, 255, 0));
        auto pinkOverlayBG = std::make_shared<XhtmlStyleEntry>();
        pinkOverlayBG->setBackgroundColor(Color(255, 0, 255)).setOpacity(0.6f);
        auto list01 = XhtmlElement::createUnorderedList(nullptr, yellowBG);
        XhtmlElement::addChildrenAndSetPatent(list01, {
            XhtmlElement::createListItem(nullptr, XhtmlElement::createInnerText(nullptr, U"First unordered list item")),
            XhtmlElement::createListItem(nullptr, {XhtmlElement::createInnerText(nullptr, U"Second unordered list item"),
                                                   XhtmlElement::createUnorderedList(nullptr, pinkOverlayBG, {XhtmlElement::createListItem(nullptr, XhtmlElement::createInnerText(nullptr, U"Coffee")),
                                                                                                              XhtmlElement::createListItem(nullptr, XhtmlElement::createInnerText(nullptr, U"Tea"))})
                                                  }),
            XhtmlElement::createListItem(nullptr, XhtmlElement::createInnerText(nullptr, U"Third unordered list item"))});
        auto pinkBG = std::make_shared<XhtmlStyleEntry>();
        pinkBG->setBackgroundColor(Color(255, 0, 255));
        auto yellowOverlayBG = std::make_shared<XhtmlStyleEntry>();
        yellowOverlayBG->setBackgroundColor(Color(255, 255, 0)).setOpacity(0.6f);
        auto list02 = XhtmlElement::createOrderedList(nullptr, pinkBG);
        XhtmlElement::addChildrenAndSetPatent(list02, {
            XhtmlElement::createListItem(nullptr, XhtmlElement::createInnerText(nullptr, U"First ordered list item")),
            XhtmlElement::createListItem(nullptr, XhtmlElement::createInnerText(nullptr, U"Second ordered list item")),
            XhtmlElement::createOrderedList(nullptr, yellowOverlayBG, {XhtmlElement::createListItem(nullptr, XhtmlElement::createInnerText(nullptr, U"Apple")),
                                                                       XhtmlElement::createListItem(nullptr, XhtmlElement::createInnerText(nullptr, U"Orange"))}),
            XhtmlElement::createListItem(nullptr, XhtmlElement::createInnerText(nullptr, U"Third ordered list item"))});
        auto a10 = XhtmlElement::createAnchor(nullptr, a_BG, XhtmlElement::createInnerText(nullptr, U"Jump to: Top"));
        a10->addAttribute(std::make_shared<XhtmlAttribute>(U"href", U"#HTML-MU"));
        auto h3_CODE = XhtmlElement::createH3(nullptr, {XhtmlElement::createInnerText(nullptr, U"Code and style-sheet "),
                                                        XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#008800")), XhtmlElement::createInnerText(nullptr, U"tests")),
                                                        XhtmlElement::createInnerText(nullptr, U" (h3)")});
        h3_CODE->addAttribute(std::make_shared<XhtmlAttribute>(U"id", U"CODE"));
        auto h3_BG03 = std::make_shared<XhtmlStyleEntry>();
        h3_BG03->setBackgroundColor(Color(U"#D0D0DF"))
                .setMargin(FourDimSize(SizeType::Pixel, 8.0f))
                .setBorderStyle(FourDimBorderStyle(BorderStyle::Solid))
                .setBorderWidth(FourDimSize(SizeType::Pixel, 1.0f))
                .setBorderColor(Color(U"#B0B0BF"))
                .setPadding(FourDimSize(SizeType::Pixel, 8.0f));
        h3_CODE->addAttribute(h3_BG03);
        auto silverBG = std::make_shared<XhtmlStyleEntry>();
        silverBG->setBackgroundColor(Color(248, 248, 248, 255))
                 .setBorderStyle(FourDimBorderStyle(BorderStyle::Solid))
                 .setBorderWidth(FourDimSize(SizeType::Pixel, 1.0f))
                 .setBorderColor(Color(160, 160, 160, 192))
                 .setPadding(FourDimSize(SizeType::Pixel, 0.0f, 4.0f));
        auto h4_STYLED_CODE = XhtmlElement::createH4(nullptr, {XhtmlElement::createInnerText(nullptr, U"Code with style sheet "),
                                                               XhtmlElement::createSpan(nullptr, std::make_shared<XhtmlStyleEntry>(Color(U"#008800")), XhtmlElement::createInnerText(nullptr, U"test")),
                                                               XhtmlElement::createInnerText(nullptr, U" (h4)") });
        h4_STYLED_CODE->addAttribute(std::make_shared<XhtmlAttribute>(U"id", U"STYLED_CODE"));
        auto h4_BG03 = std::make_shared<XhtmlStyleEntry>();
        h4_BG03->setBackgroundColor(Color(U"#D8D0D8"))
                .setBorderColor(Color(U"#B8B0B8"))
                .setMargin(FourDimSize(SizeType::Pixel, 4.0f))
                .setBorderStyle(FourDimBorderStyle(BorderStyle::Solid))
                .setBorderWidth(FourDimSize(SizeType::Pixel, 1.0f))
                .setPadding(FourDimSize(SizeType::Pixel, 4.0f));
        h4_STYLED_CODE->addAttribute(h4_BG03);
        auto code01 = XhtmlElement::createCode(nullptr);
        XhtmlElement::addChildrenAndSetPatent(body, {
            list01,
            XhtmlElement::createInnerText(nullptr, U"Example of ordered lists:"),
            list02,
            XhtmlElement::createInnerText(nullptr, U"left-side text of a link "),
            a10,
            XhtmlElement::createInnerText(nullptr, U" right-side text of a link"),
            h3_CODE,
            h4_STYLED_CODE,
            XhtmlElement::createDivision(nullptr, XhtmlElement::createPreformatted(nullptr, silverBG, code01)) });
        XhtmlElement::addChildrenAndSetPatent(code01, {
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"cp"), XhtmlElement::createInnerText(nullptr, U"#include <TGUI/TGUI.hpp>")),
            XhtmlElement::createBreak(),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"cp"), XhtmlElement::createInnerText(nullptr, U"#include <TGUI/Backend/SFML-Graphics.hpp>")),
            XhtmlElement::createBreak(),
            XhtmlElement::createBreak(),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"kt"), XhtmlElement::createInnerText(nullptr, U"int")),
            XhtmlElement::createInnerText(nullptr, U" "),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"nf"), XhtmlElement::createInnerText(nullptr, U"main")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), XhtmlElement::createInnerText(nullptr, U"()")),
            XhtmlElement::createBreak(),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), XhtmlElement::createInnerText(nullptr, U"{")),
            XhtmlElement::createBreak(),
            XhtmlElement::createInnerText(nullptr, U"    "),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), XhtmlElement::createInnerText(nullptr, U"sf")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"o"), XhtmlElement::createInnerText(nullptr, U"::")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), XhtmlElement::createInnerText(nullptr, U"RenderWindow")),
            XhtmlElement::createInnerText(nullptr, U" "),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), XhtmlElement::createInnerText(nullptr, U"window")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), XhtmlElement::createInnerText(nullptr, U"{{")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"mi"), XhtmlElement::createInnerText(nullptr, U"800")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), XhtmlElement::createInnerText(nullptr, U",")),
            XhtmlElement::createInnerText(nullptr, U" "),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"mi"), XhtmlElement::createInnerText(nullptr, U"600")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), XhtmlElement::createInnerText(nullptr, U"},")),
            XhtmlElement::createInnerText(nullptr, U" "),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"s"), XhtmlElement::createInnerText(nullptr, U"\"TGUI example - SFML_GRAPHICS backend\"")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), XhtmlElement::createInnerText(nullptr, U"};")),
            XhtmlElement::createBreak(),
            XhtmlElement::createInnerText(nullptr, U"    "),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), XhtmlElement::createInnerText(nullptr, U"tgui")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"o"), XhtmlElement::createInnerText(nullptr, U"::")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), XhtmlElement::createInnerText(nullptr, U"Gui")),
            XhtmlElement::createInnerText(nullptr, U" "),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), XhtmlElement::createInnerText(nullptr, U"gui")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), XhtmlElement::createInnerText(nullptr, U"{")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), XhtmlElement::createInnerText(nullptr, U"window")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), XhtmlElement::createInnerText(nullptr, U"};")),
            XhtmlElement::createBreak(),
            XhtmlElement::createInnerText(nullptr, U"    "),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), XhtmlElement::createInnerText(nullptr, U"gui")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), XhtmlElement::createInnerText(nullptr, U".")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), XhtmlElement::createInnerText(nullptr, U"mainLoop")),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), XhtmlElement::createInnerText(nullptr, U"();")),
            XhtmlElement::createInnerText(nullptr, U" "),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"cl"), XhtmlElement::createInnerText(nullptr, U"// See below for how to use your own main loop")),
            XhtmlElement::createBreak(),
            XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), XhtmlElement::createInnerText(nullptr, U"}"))});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Handle the onZoomChanged signal for the formatted text view 1.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void onZoomChangeView1(float newZoom)
    {
        if (m_zoomLabel1 != nullptr)
            m_zoomLabel1->setText(U"Current zoom is: " + String::fromNumber((int)(newZoom * 100.0f + 0.1f)) + U"%");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Handle the onZoomChanged signal for the formatted text view 2.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void onZoomChangeView2(float newZoom)
    {
        if (m_zoomLabel2 != nullptr)
            m_zoomLabel2->setText(U"Current zoom is: " + String::fromNumber((int)(newZoom * 100.0f + 0.1f)) + U"%");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Create the widgets of the main frame
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool createFrameContent()
    {
#ifdef _DEBUG
        std::cout << "'createFrameContent()'->START" << std::endl;
#endif // _DEBUG
        // Fill work area.
        m_pageTabContainer = TabContainer::create();
        m_pageTabContainer->setPosition(0.0f, 0.0f);
        m_pageTabContainer->setSize("100%", "100%");
        auto tabPanel1 = m_pageTabContainer->addTab(U"created", true);
        auto tabPanel2 = m_pageTabContainer->addTab(U"parsed", true);
        this->getGui().add(m_pageTabContainer, U"MyTabContainer");

        auto formattedDocument1 = FormattedXhtmlDocument::create();
        m_formattedTextView1 = FormattedTextView::create();
        m_formattedTextView1->setDocument(formattedDocument1);
        m_formattedTextView1->setPosition("16", "16");
        Layout widthLayout1(Layout::Operation::Minus, std::make_unique<Layout>("100%"), std::make_unique<Layout>("32"));
        Layout hightLayout1(Layout::Operation::Minus, std::make_unique<Layout>("100%"), std::make_unique<Layout>("64"));
        m_formattedTextView1->setSize(widthLayout1, hightLayout1);
        tabPanel1->add(m_formattedTextView1, "MyFormattedTextArea-created");

        m_zoomLabel1 = Label::create(U"Current zoom is: 100%");
        Layout posYLayout1(Layout::Operation::Minus, std::make_unique<Layout>("100%"), std::make_unique<Layout>("48"));
        m_zoomLabel1->setPosition(32, posYLayout1);
        m_zoomLabel1->setSize(widthLayout1, 32);
        tabPanel1->add(m_zoomLabel1, "MyZoomLabel-created");

        auto formattedDocument2 = FormattedXhtmlDocument::create();
        m_formattedTextView2 = FormattedTextView::create();
        m_formattedTextView2->setDocument(formattedDocument2);
        m_formattedTextView2->setPosition("16", "16");
        Layout widthLayout2(Layout::Operation::Minus, std::make_unique<Layout>("100%"), std::make_unique<Layout>("32"));
        Layout hightLayout2(Layout::Operation::Minus, std::make_unique<Layout>("100%"), std::make_unique<Layout>("64"));
        m_formattedTextView2->setSize(widthLayout2, hightLayout2);
        tabPanel2->add(m_formattedTextView2, "MyFormattedTextArea-parsed");

        m_zoomLabel2 = Label::create(U"Current zoom is: 100%");
        Layout posYLayout2(Layout::Operation::Minus, std::make_unique<Layout>("100%"), std::make_unique<Layout>("48"));
        m_zoomLabel2->setPosition(32, posYLayout1);
        m_zoomLabel2->setSize(widthLayout2, 32);
        tabPanel2->add(m_zoomLabel2, "MyZoomLabel-parsed");

        buildDom(formattedDocument1);
        m_formattedTextView1->rearrangeText(true);
        m_formattedTextView1->onZoomChange(&XHTMLViewerMainForm::onZoomChangeView1, this);

        std::string filePath("./testdata/integrationtest.htm");
        formattedDocument2->setErrorNotifyDlgParent(m_formattedTextView2);
        formattedDocument2->loadDocument(filePath, true);

        m_formattedTextView2->rearrangeText(true);
        m_formattedTextView2->onZoomChange(&XHTMLViewerMainForm::onZoomChangeView2, this);

        m_pageTabContainer->select(0);

        return true;
    }
};

// Run main(int, char**) instead main() to be able to debug in Code::Blocks.
int main(int argc, char** argv)
{
    UNUSED(argc);
    UNUSED(argv);

    // Section 1: Native window creation.
#ifdef TGUI_HAS_BACKEND_SFML_GRAPHICS
    sf::RenderWindow window(sf::VideoMode(980, 600), "TGUI window", sf::Style::Default);
#else
    // The OpenGL renderer backend in TGUI requires at least OpenGL version 3.3.
    // Request the OpenGL version 3.3, even if it isn't provided by the hardware.
    // Sometimes you'll have luck. (Mesa 20.2.5 pretends 3.3 on my 3.0 hardware.)
    sf::ContextSettings requestedettings;
    requestedettings.attributeFlags = sf::ContextSettings::Attribute::Core;
    requestedettings.majorVersion = 3;
    requestedettings.minorVersion = 3;

    sf::Window window(sf::VideoMode(980, 800), "TGUI window", sf::Style::Default, requestedettings);
#endif
    auto realizedSettings = window.getSettings();
    std::cout << "SUCCESS creating main frame window with OpenGL " << realizedSettings.majorVersion << "." << realizedSettings.minorVersion << " context.\n";

    // Section 2: TGUI initialization.
    Gui gui(window);
    std::string themeFilePath = "./themes/BabyBlue.txt";
    std::ifstream themeFile(themeFilePath);
    if (!themeFile.is_open())
        std::cerr << "ERROR: Unable to load theme.\n";
    else
        Theme::setDefault(themeFilePath);
    auto container = gui.getContainer();

    // Section 2: Sample application.
    XHTMLViewerMainForm mainForm(window, gui);
    if (!mainForm.createFrameContent())
    {
        std::cerr << "ERROR: Unable to create window content.\n";
        return EXIT_FAILURE;
    }
    std::cout << "SUCCESS creating window content.\n";

    // Section 4: Generic application infrastructure.
    gui.mainLoop(mainForm.getClearColor());

    return EXIT_SUCCESS;
}
