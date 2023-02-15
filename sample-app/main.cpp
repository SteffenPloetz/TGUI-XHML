// The SFML graphic module (sf::RenderWindow) is more sophisticated than the SFML OpenGL window (sf::Window).
// - It adds features to draw SFML specific entities, but it requires extra care when mixing with native OpenGL calls.
// - It accepts OpenGL version 1.1 and above, while the SFML window (sf::Window) requires OpenGL version 3.3 at least.
// The SFML OpenGL window (sf::Window) doesn't necessarily require the TGUI_HAS_FONT_BACKEND_FREETYPE to be set.
// It works fine with the TGUI_HAS_FONT_BACKEND_SFML_GRAPHICS, if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS is also set.
// See "https://www.sfml-dev.org/tutorials/2.5/window-opengl.php" (last section) for details.
#include <TGUI/Config.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Base64.hpp>

#ifdef TGUI_HAS_BACKEND_SFML_GRAPHICS
#include <TGUI/Backend/SFML-Graphics.hpp>
#else
#include <TGUI/Backend/SFML-OpenGL3.hpp>
#endif

#include "TGUI/StringHelper.hpp"
#include "TGUI/Xhtml/XhtmlEntityResolver.hpp"
#include "TGUI/Xhtml/XhtmlAttributes.hpp"
#include "TGUI/Xhtml/XhtmlElements.hpp"
#include "TGUI/Widgets/FormattedElements.hpp"
#include "TGUI/Widgets/FormattedTextXhtmlDocument.hpp"
#include "TGUI/Renderers/FormattedTextRenderer.hpp"
#include "TGUI/Widgets/FormattedTextView.hpp"
#include "TGUI/Xhtml/XhtmlParser.hpp"
#include "TGUI/UtfHelper.hpp"

#if TGUI_COMPILED_WITH_CPP_VER >= 17
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
#include <iostream>
#include <fstream>
#include <thread>
#include <cmath>

#define UNUSED(x) (void)(x)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief My application main form
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class XHTMLViewerMainForm
{
private:
    sf::Window&                   m_window;                       //!< The main frame window
    tgui::BackendGui&             m_gui;                          //!< The underlying GUI, that organizes the widgets
    tgui::Color                   m_clearColor;                   //!< The main form background color
    tgui::TabContainer::Ptr       m_pageTabContainer;             //!< The page switcher
    tgui::FormattedTextView::Ptr  m_formattedTextView1;           //!< The XHTML formatted text view 1
    tgui::FormattedTextView::Ptr  m_formattedTextView2;           //!< The XHTML formatted text view 2
    tgui::Label::Ptr              m_zoomLabel1;                   //!< The zoom display for text view 1
    tgui::Label::Ptr              m_zoomLabel2;                   //!< The zoom display for text view 2

    const float                   m_tabFixedSize = 150.0f;        //!< The fixed size of the tab.

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
    XHTMLViewerMainForm(sf::Window& window, tgui::BackendGui& gui, tgui::Color clearColor = tgui::Color(240, 240, 240)) :
        m_window(window),  m_gui(gui), m_clearColor(clearColor), m_pageTabContainer(), m_formattedTextView1(), m_formattedTextView2()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Returns the back-end GUI (containing native window and renderer)
    ///
    /// @return Back-end GUI (containing native window and renderer)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    tgui::BackendGui& getGui() const
    {   return m_gui;    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Returns the background color of the back-end window
    ///
    /// @return Background color of the back-end window
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    tgui::Color getClearColor() const
    {   return m_clearColor;   }

    std::shared_ptr<tgui::FormattedTextDocument::FontCollection> createFontCollection()
    {
        auto fontCollection = std::make_shared<tgui::FormattedTextDocument::FontCollection>();

        fontCollection->Sans = std::make_shared<tgui::FormattedTextDocument::FontPack>();
        fontCollection->Serif = std::make_shared<tgui::FormattedTextDocument::FontPack>();
        fontCollection->Mono = std::make_shared<tgui::FormattedTextDocument::FontPack>();

        // ------------------------------------------------------------------------------------------------
        // We can achieve a higher display quality for bold text if - instead of setting m_textStyleCached
        // to TextStyle::Bold - we use our own font. Therefore, the Bold and BoldItalic fonts are provided.
        // ------------------------------------------------------------------------------------------------

#if defined(TGUI_SYSTEM_WINDOWS)
        tgui::String fontRoot = L"C:\\Windows\\Fonts\\";

        fontCollection->Sans->Regular = std::make_shared<tgui::Font>(fontRoot + L"arial.ttf");
        fontCollection->Sans->Bold = std::make_shared<tgui::Font>(fontRoot + L"arialbd.ttf");
        fontCollection->Sans->Italic = std::make_shared<tgui::Font>(fontRoot + L"ariali.ttf");
        fontCollection->Sans->BoldItalic = std::make_shared<tgui::Font>(fontRoot + L"arialbi.ttf");

        fontCollection->Serif->Regular = std::make_shared<tgui::Font>(fontRoot + L"times.ttf");
        fontCollection->Serif->Bold = std::make_shared<tgui::Font>(fontRoot + L"timesbd.ttf");
        fontCollection->Serif->Italic = std::make_shared<tgui::Font>(fontRoot + L"timesi.ttf");
        fontCollection->Serif->BoldItalic = std::make_shared<tgui::Font>(fontRoot + L"timesbi.ttf");

        fontCollection->Mono->Regular = std::make_shared<tgui::Font>(fontRoot + L"consola.ttf"); // L"cour.ttf");
        fontCollection->Mono->Bold = std::make_shared<tgui::Font>(fontRoot + L"consolab.ttf"); // L"courbd.ttf");
        fontCollection->Mono->Italic = std::make_shared<tgui::Font>(fontRoot + L"consolai.ttf"); // L"couri.ttf");
        fontCollection->Mono->BoldItalic = std::make_shared<tgui::Font>(fontRoot + L"consolaz.ttf"); // L"courbi.ttf");
#elif defined(TGUI_SYSTEM_LINUX)
        tgui::String fontRoot = L"/usr/share/fonts/";

        // Alternative sans fonts (Linux Firefox uses Roboto by default): OpenSans, Roboto, DejaVuSans
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        std::filesystem::path suseSansPath{ "/usr/share/fonts/truetype/Roboto-Regular.ttf" };
        std::filesystem::path manjaroSansPath{ "/usr/share/fonts/TTF/DejaVuSans.ttf" };
#else
        std::experimental::filesystem::path suseSansPath{ "/usr/share/fonts/truetype/Roboto-Regular.ttf" };
        std::experimental::filesystem::path manjaroSansPath{ "/usr/share/fonts/TTF/DejaVuSans.ttf" };
#endif
        // openSUSE
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        if (std::filesystem::exists(suseSansPath))
#else
        if (std::experimental::filesystem::exists(suseSansPath))
#endif
        {
            fontCollection->Sans->Regular = std::make_shared<tgui::Font>(fontRoot + L"truetype/Roboto-Regular.ttf");
            fontCollection->Sans->Bold = std::make_shared<tgui::Font>(fontRoot + L"truetype/Roboto-Bold.ttf");
            fontCollection->Sans->Italic = std::make_shared<tgui::Font>(fontRoot + L"truetype/Roboto-Italic.ttf");
            fontCollection->Sans->BoldItalic = std::make_shared<tgui::Font>(fontRoot + L"truetype/Roboto-BoldItalic.ttf");

            fontCollection->Serif->Regular = std::make_shared<tgui::Font>(fontRoot + L"truetype/DejaVuSerif.ttf");
            fontCollection->Serif->Bold = std::make_shared<tgui::Font>(fontRoot + L"truetype/DejaVuSerif-Bold.ttf");
            fontCollection->Serif->Italic = std::make_shared<tgui::Font>(fontRoot + L"truetype/DejaVuSerif-Italic.ttf");
            fontCollection->Serif->BoldItalic = std::make_shared<tgui::Font>(fontRoot + L"truetype/DejaVuSerif-BoldItalic.ttf");

            fontCollection->Mono->Regular = std::make_shared<tgui::Font>(fontRoot + L"truetype/DejaVuSansMono.ttf");
            fontCollection->Mono->Bold = std::make_shared<tgui::Font>(fontRoot + L"truetype/DejaVuSansMono-Bold.ttf");
            fontCollection->Mono->Italic = std::make_shared<tgui::Font>(fontRoot + L"truetype/DejaVuSansMono-Oblique.ttf");
            fontCollection->Mono->BoldItalic = std::make_shared<tgui::Font>(fontRoot + L"truetype/DejaVuSansMono-BoldOblique.ttf");
        }
        // Manjaro
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        else if (std::filesystem::exists(manjaroSansPath))
#else
        if (std::experimental::filesystem::exists(manjaroSansPath))
#endif
        {
            fontCollection->Sans->Regular = std::make_shared<tgui::Font>(fontRoot + L"TTF/DejaVuSans.ttf");
            fontCollection->Sans->Bold = std::make_shared<tgui::Font>(fontRoot + L"TTF/DejaVuSans-Bold.ttf");
            fontCollection->Sans->Italic = std::make_shared<tgui::Font>(fontRoot + L"TTF/DejaVuSans-Oblique.ttf");
            fontCollection->Sans->BoldItalic = std::make_shared<tgui::Font>(fontRoot + L"TTF/DejaVuSans-BoldOblique.ttf");

            fontCollection->Serif->Regular = std::make_shared<tgui::Font>(fontRoot + L"TTF/DejaVuSerif.ttf");
            fontCollection->Serif->Bold = std::make_shared<tgui::Font>(fontRoot + L"TTF/DejaVuSerif-Bold.ttf");
            fontCollection->Serif->Italic = std::make_shared<tgui::Font>(fontRoot + L"TTF/DejaVuSerif-Italic.ttf");
            fontCollection->Serif->BoldItalic = std::make_shared<tgui::Font>(fontRoot + L"TTF/DejaVuSerif-BoldItalic.ttf");

            fontCollection->Mono->Regular = std::make_shared<tgui::Font>(fontRoot + L"TTF/DejaVuSansMono.ttf");
            fontCollection->Mono->Bold = std::make_shared<tgui::Font>(fontRoot + L"TTF/DejaVuSansMono-Bold.ttf");
            fontCollection->Mono->Italic = std::make_shared<tgui::Font>(fontRoot + L"TTF/DejaVuSansMono-Oblique.ttf");
            fontCollection->Mono->BoldItalic = std::make_shared<tgui::Font>(fontRoot + L"TTF/DejaVuSansMono-BoldOblique.ttf");
        }
        else
        {
            std::cerr << "ERROR: The fonts can not be loaded.\n";

            std::shared_ptr<tgui::Font> fallbackFont = std::make_shared<tgui::Font>(std::move(this->getGui().getFont()));
            fontCollection->Sans->Regular = fallbackFont;
            fontCollection->Sans->Bold = fallbackFont;
            fontCollection->Sans->Italic = fallbackFont;
            fontCollection->Sans->BoldItalic = fallbackFont;


            fontCollection->Serif->Regular = fallbackFont;
            fontCollection->Serif->Bold = fallbackFont;
            fontCollection->Serif->Italic = fallbackFont;
            fontCollection->Serif->BoldItalic = fallbackFont;


            fontCollection->Mono->Regular = fallbackFont;
            fontCollection->Mono->Bold = fallbackFont;
            fontCollection->Mono->Italic = fallbackFont;
            fontCollection->Mono->BoldItalic = fallbackFont;
        }
#elif defined(TGUI_SYSTEM_IOS)
        throw "IOS fonts currently not supported.";
#elif defined(TGUI_SYSTEM_MACOS)
        throw "MACOS fonts currently not supported.";
#elif defined(TGUI_SYSTEM_ANDROID)
        throw "ANDROID fonts currently not supported.";
#else
        throw "Unsupported platform.";
#endif

        fontCollection->assertValid();
        fontCollection->Sans->Regular->setSmooth(false);
        fontCollection->Sans->Bold->setSmooth(false);
        fontCollection->Sans->Italic->setSmooth(false);
        fontCollection->Sans->BoldItalic->setSmooth(false);

        return fontCollection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Build the Document-Object-Model (DOM) for the indicated view.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void buildDom(tgui::FormattedTextXhtmlDocument::Ptr formattedTextDocument1)
    {
        // create a base 64 stream from PNG for test purposes
        // std::string filePath("/home/steffen/Projects/CodeBlocks/XHTMLViewerTGUI/html5.png"); //"C:\\Projects\\XHTMLViewerTGUI\\html5.png"
        // std::size_t fileSize = 0;
        // std::unique_ptr<std::uint8_t[]> data = tgui::readFileToMemory(filePath, fileSize));
        // if (fileSize > 0)
        //     std::string b64 = tgui::base64Encode((const std::uint8_t*)data.data(), data.size());

        const tgui::String imageSource = "data:image/png;base64,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";

        auto htmlRoot1 = formattedTextDocument1->getRootElement();
        auto htmlHead = tgui::XhtmlElement::createHead(htmlRoot1);
        auto thmlStyle = tgui::XhtmlElement::createStyle(htmlHead);
        thmlStyle->setEntry(U"cp", std::make_shared<tgui::XhtmlStyleEntry>(tgui::Color(U"#666666"), U"Monospace", tgui::OneDimSize(tgui::SizeType::Relative, 1.0f), static_cast<tgui::TextStyle>(tgui::TextStyle::Italic  | tgui::TextStyle::Bold)));
        thmlStyle->setEntry(U"kt", std::make_shared<tgui::XhtmlStyleEntry>(tgui::Color(U"#666666"), U"Monospace", tgui::OneDimSize(tgui::SizeType::Relative, 1.0f), static_cast<tgui::TextStyle>(tgui::TextStyle::Regular | tgui::TextStyle::Bold)));
        thmlStyle->setEntry(U"nf", std::make_shared<tgui::XhtmlStyleEntry>(tgui::Color(U"#aa4400"), U"Monospace", tgui::OneDimSize(tgui::SizeType::Relative, 1.0f), static_cast<tgui::TextStyle>(tgui::TextStyle::Regular | tgui::TextStyle::Bold)));
        thmlStyle->setEntry(U"p",  std::make_shared<tgui::XhtmlStyleEntry>(tgui::Color(U"#666666"), U"Monospace", tgui::OneDimSize(tgui::SizeType::Relative, 1.0f), static_cast<tgui::TextStyle>(tgui::TextStyle::Regular | tgui::TextStyle::Regular)));
        thmlStyle->setEntry(U"n",  std::make_shared<tgui::XhtmlStyleEntry>(tgui::Color(U"#666666"), U"Monospace", tgui::OneDimSize(tgui::SizeType::Relative, 1.0f), static_cast<tgui::TextStyle>(tgui::TextStyle::Regular | tgui::TextStyle::Regular)));
        thmlStyle->setEntry(U"o",  std::make_shared<tgui::XhtmlStyleEntry>(tgui::Color(U"#666666"), U"Monospace", tgui::OneDimSize(tgui::SizeType::Relative, 1.0f), static_cast<tgui::TextStyle>(tgui::TextStyle::Regular | tgui::TextStyle::Bold)));
        thmlStyle->setEntry(U"mi", std::make_shared<tgui::XhtmlStyleEntry>(tgui::Color(U"#009999"), U"Monospace", tgui::OneDimSize(tgui::SizeType::Relative, 1.0f), static_cast<tgui::TextStyle>(tgui::TextStyle::Regular | tgui::TextStyle::Regular)));
        thmlStyle->setEntry(U"s",  std::make_shared<tgui::XhtmlStyleEntry>(tgui::Color(U"#cc4444"), U"Monospace", tgui::OneDimSize(tgui::SizeType::Relative, 1.0f), static_cast<tgui::TextStyle>(tgui::TextStyle::Regular | tgui::TextStyle::Regular)));
        thmlStyle->setEntry(U"cl", std::make_shared<tgui::XhtmlStyleEntry>(tgui::Color(U"#aaaaaa"), U"Monospace", tgui::OneDimSize(tgui::SizeType::Relative, 1.0f), static_cast<tgui::TextStyle>(tgui::TextStyle::Regular | tgui::TextStyle::Regular)));
        auto grayBG = std::make_shared<tgui::XhtmlStyleEntry>(); grayBG->setBackgroundColor(tgui::Color(208, 208, 208, 255));
        auto body = tgui::XhtmlElement::createBody(htmlRoot1, grayBG);

        auto image1 = tgui::XhtmlElement::createImage(nullptr);
        image1->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"src", imageSource.toUtf32()));
        image1->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"alt", U"HTML 5"));
        image1->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"width", U"36"));
        image1->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"height", U"36"));
        auto image2 = tgui::XhtmlElement::createImage(nullptr);
        image2->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"src", imageSource.toUtf32()));
        image2->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"alt", U"HTML 5"));
        image2->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"width", U"24"));
        image2->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"height", U"24"));
        auto image3 = tgui::XhtmlElement::createImage(nullptr);
        image3->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"src", imageSource.toUtf32()));
        image3->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"alt", U"HTML 5"));
        image3->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"width", U"48"));
        image3->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"height", U"48"));
        auto image4 = tgui::XhtmlElement::createImage(nullptr);
        image4->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"src", U"file://./url-icon.png"));
        image4->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"alt", U"URL icon"));
        image4->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"width", U"32"));
        image4->addAttribute(std::make_shared<tgui::XhtmlAttribute>(U"height", U"32"));
        tgui::XhtmlElement::addChildrenAndSetPatent(body, {
           tgui::XhtmlElement::createH1(nullptr, {tgui::XhtmlElement::createInnerText(nullptr, U"FormattedTextArea - XHTML markup "),
                                                   tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#008800"), tgui::XhtmlElement::createInnerText(nullptr, U"tests")),
                                                   tgui::XhtmlElement::createInnerText(nullptr, U" (h1)")}),
            tgui::XhtmlElement::createH2(nullptr, {tgui::XhtmlElement::createInnerText(nullptr, U"Introduction to the "),
                                                   tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#880000"), tgui::XhtmlElement::createInnerText(nullptr, U"FormattedTextArea")),
                                                   tgui::XhtmlElement::createInnerText(nullptr, U" widget (h2)")}),
            tgui::XhtmlElement::createInnerText(nullptr, U"The "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#880000"), tgui::XhtmlElement::createInnerText(nullptr, U"FormattedTextArea")),
            tgui::XhtmlElement::createInnerText(nullptr, U" widget displays markup formatted text. Currently supported is a sub-set of XHTML, that includes the HTML block elements "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;header&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"header"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/header&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;body&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"body"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/body&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;p&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"paragraph"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/p&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;div&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"division"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/div&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;span&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"span"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/span&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;h1&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"headline 1"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/h1&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U" ... "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;h6&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"headline 6"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/h6&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;em&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"emphasized"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/em&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;strong&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"strong"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/strong&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;i&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"italic"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/i&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;b&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"bold"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/b&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;u&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"underline"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/u&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;sup&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"superscript"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/sup&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U" for "),
            tgui::XhtmlElement::createSuperscript(nullptr, U"superscript text"),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;sub&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"subscript"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/sub&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U" for "),
            tgui::XhtmlElement::createSubscript(nullptr, U"subscript text"),
            tgui::XhtmlElement::createInnerText(nullptr, U", the XHTML empty elements "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;br/&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U" for line break and "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;image/&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U" for images like "),
            image1,
            tgui::XhtmlElement::createInnerText(nullptr, U" (100%) and XHTML "),
            tgui::XhtmlElement::createEmphasized(nullptr, U"inner text"),
            tgui::XhtmlElement::createInnerText(nullptr, U" of any block. In addition to that, all block elements support the "),
            tgui::XhtmlElement::createEmphasized(nullptr, U"style"),
            tgui::XhtmlElement::createInnerText(nullptr, U" attribute (currently the color style only) and the images can be zoomed like this "),
            image2,
            tgui::XhtmlElement::createInnerText(nullptr, U" (67%) or this "),
            image3,
            tgui::XhtmlElement::createInnerText(nullptr, U" (133%)."),
            tgui::XhtmlElement::createH2(nullptr, {tgui::XhtmlElement::createInnerText(nullptr, U"Various text "),
                                                   tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#008800"), tgui::XhtmlElement::createInnerText(nullptr, U"tests")),
                                                   tgui::XhtmlElement::createInnerText(nullptr, U" (h2)")}),
            tgui::XhtmlElement::createH3(nullptr, {tgui::XhtmlElement::createInnerText(nullptr, U"Floating text and block text "),
                                                  tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#008800"), tgui::XhtmlElement::createInnerText(nullptr, U"tests")),
                                                   tgui::XhtmlElement::createInnerText(nullptr, U" (h3)")}),
            tgui::XhtmlElement::createH4(nullptr, {tgui::XhtmlElement::createInnerText(nullptr, U"Change the witdth of the window to "),
                                                   tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#008800"), tgui::XhtmlElement::createInnerText(nullptr, U"test")),
                                                   tgui::XhtmlElement::createInnerText(nullptr, U" automatic line break functionality. (h4)")}),
            tgui::XhtmlElement::createInnerText(nullptr, U"This is the first "),
            tgui::XhtmlElement::createUnderline(nullptr, U"floating"),
            tgui::XhtmlElement::createInnerText(nullptr, U" text line (not embedded in a "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;span&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"span"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/span&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;div&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"division"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/div&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U" or "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;p&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"paragraph"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/p&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"). It is followed by a line break."),
            tgui::XhtmlElement::createBreak(),
            tgui::XhtmlElement::createInnerText(nullptr, U"This is the second "),
            tgui::XhtmlElement::createUnderline(nullptr, U"floating"),
            tgui::XhtmlElement::createInnerText(nullptr, U" text line (not embedded in a "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;span&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"span"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/span&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U", "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;div&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"division"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/div&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U" or "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;p&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"paragraph"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/p&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U").")});
        auto div01 = tgui::XhtmlElement::createDivision(nullptr, tgui::Color::Blue);
        tgui::XhtmlElement::addChildrenAndSetPatent(div01, {
            tgui::XhtmlElement::createInnerText(nullptr, U"This is the first "),
            tgui::XhtmlElement::createUnderline(nullptr, U"block"),
            tgui::XhtmlElement::createInnerText(nullptr, U" text (embedded in a "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;div&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"division"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/div&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"). It shall demonstrate no vertical spacing around this block and automatic line breaks.")});
        tgui::XhtmlElement::addChildAndSetPatent(body, div01);
        auto par01 = tgui::XhtmlElement::createParagraph(nullptr, tgui::Color(U"#880088"));
        tgui::XhtmlElement::addChildrenAndSetPatent(par01, {
            tgui::XhtmlElement::createInnerText(nullptr, U"This is the second "),
            tgui::XhtmlElement::createUnderline(nullptr, U"block"),
            tgui::XhtmlElement::createInnerText(nullptr, U" text (embedded in a "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;p&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"paragraph"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/p&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"). It shall demonstrate vertical spacing around this block and a floating text and besides automatic line breaks.")});
        tgui::XhtmlElement::addChildAndSetPatent(body, par01);
        auto par02 = tgui::XhtmlElement::createParagraph(nullptr);
        tgui::XhtmlElement::addChildrenAndSetPatent(par02, {
            tgui::XhtmlElement::createInnerText(nullptr, U"While the images so far have been set from a base64 memory stream, this image is loaded from a PNG file (which might not work in browsers): "), image4});
        tgui::XhtmlElement::addChildAndSetPatent(body, par02);
        tgui::XhtmlElement::addChildrenAndSetPatent(body, {
            tgui::XhtmlElement::createInnerText(nullptr, U"This is the "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color::Green, {tgui::XhtmlElement::createInnerText(nullptr, U"final "),
                                                    tgui::XhtmlElement::createUnderline(nullptr, U"floating"),
                                                    tgui::XhtmlElement::createInnerText(nullptr, U" text line ")}),
            tgui::XhtmlElement::createInnerText(nullptr, U"with a "),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;span&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U"span"),
            tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#884400"), tgui::XhtmlElement::createInnerText(nullptr, U"&lt;/span&gt;")),
            tgui::XhtmlElement::createInnerText(nullptr, U" (inline-style = color:green)."),
            tgui::XhtmlElement::createH3(nullptr, {tgui::XhtmlElement::createInnerText(nullptr, U"List "),
                                                   tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#008800"), tgui::XhtmlElement::createInnerText(nullptr, U"tests")),
                                                   tgui::XhtmlElement::createInnerText(nullptr, U" (h3)")}),
            tgui::XhtmlElement::createH4(nullptr, {tgui::XhtmlElement::createInnerText(nullptr, U"Nested list "),
                                                   tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#008800"), tgui::XhtmlElement::createInnerText(nullptr, U"tests")),
                                                   tgui::XhtmlElement::createInnerText(nullptr, U" (h4)")}),
            tgui::XhtmlElement::createInnerText(nullptr, U"Example of unordered lists:")});
        auto yellowBG = std::make_shared<tgui::XhtmlStyleEntry>();
        yellowBG->setBackgroundColor(tgui::Color(255, 255, 0));
        auto pinkOverlayBG = std::make_shared<tgui::XhtmlStyleEntry>();
        pinkOverlayBG->setBackgroundColor(tgui::Color(255, 0, 255));
        pinkOverlayBG->setOpacity(0.6f);
        auto list01 = tgui::XhtmlElement::createUnorderedList(nullptr, yellowBG);
        tgui::XhtmlElement::addChildrenAndSetPatent(list01, {
            tgui::XhtmlElement::createListItem(nullptr, tgui::XhtmlElement::createInnerText(nullptr, U"First unordered list item")),
            tgui::XhtmlElement::createListItem(nullptr, {tgui::XhtmlElement::createInnerText(nullptr, U"Second unordered list item"),
                                                         tgui::XhtmlElement::createUnorderedList(nullptr, pinkOverlayBG, {tgui::XhtmlElement::createListItem(nullptr, tgui::XhtmlElement::createInnerText(nullptr, U"Coffee")),
                                                                                                                          tgui::XhtmlElement::createListItem(nullptr, tgui::XhtmlElement::createInnerText(nullptr, U"Tee"))})
                                                       }),
            tgui::XhtmlElement::createListItem(nullptr, tgui::XhtmlElement::createInnerText(nullptr, U"Third unordered list item"))});
        auto pinkBG = std::make_shared<tgui::XhtmlStyleEntry>();
        pinkBG->setBackgroundColor(tgui::Color(255, 0, 255));
        auto yellowOverlayBG = std::make_shared<tgui::XhtmlStyleEntry>();
        yellowOverlayBG->setBackgroundColor(tgui::Color(255, 255, 0));
        yellowOverlayBG->setOpacity(0.6f);
        auto list02 = tgui::XhtmlElement::createOrderedList(nullptr, pinkBG);
        tgui::XhtmlElement::addChildrenAndSetPatent(list02, {
            tgui::XhtmlElement::createListItem(nullptr, tgui::XhtmlElement::createInnerText(nullptr, U"First ordered list item")),
            tgui::XhtmlElement::createListItem(nullptr, tgui::XhtmlElement::createInnerText(nullptr, U"Second ordered list item")),
            tgui::XhtmlElement::createOrderedList(nullptr, yellowOverlayBG, {tgui::XhtmlElement::createListItem(nullptr, tgui::XhtmlElement::createInnerText(nullptr, U"Apple")),
                                                                             tgui::XhtmlElement::createListItem(nullptr, tgui::XhtmlElement::createInnerText(nullptr, U"Orange"))}),
            tgui::XhtmlElement::createListItem(nullptr, tgui::XhtmlElement::createInnerText(nullptr, U"Third ordered list item"))});
        auto silverBG = std::make_shared<tgui::XhtmlStyleEntry>(); silverBG->setBackgroundColor(tgui::Color(248, 248, 248, 255));
        silverBG->setBorderColor(tgui::Color(160, 160, 160, 192));
        silverBG->setBorderWidth(tgui::FourDimSize(tgui::SizeType::Pixel, 1.0f));
        silverBG->setPadding(tgui::FourDimSize(tgui::SizeType::Pixel, 0.0f, 4.0f));
        auto code01 = tgui::XhtmlElement::createCode(nullptr);
        tgui::XhtmlElement::addChildrenAndSetPatent(body, {
            list01,
            tgui::XhtmlElement::createInnerText(nullptr, U"Example of ordered lists:"),
            list02,
            tgui::XhtmlElement::createH3(nullptr, {tgui::XhtmlElement::createInnerText(nullptr, U"Code and style-sheet "),
                                                   tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#008800"), tgui::XhtmlElement::createInnerText(nullptr, U"tests")),
                                                   tgui::XhtmlElement::createInnerText(nullptr, U" (h3)")}),
            tgui::XhtmlElement::createH4(nullptr, {tgui::XhtmlElement::createInnerText(nullptr, U"Code with style sheet "),
                                                   tgui::XhtmlElement::createSpan(nullptr, tgui::Color(U"#008800"), tgui::XhtmlElement::createInnerText(nullptr, U"test")),
                                                   tgui::XhtmlElement::createInnerText(nullptr, U" (h4)")}),
            tgui::XhtmlElement::createDivision(nullptr, tgui::XhtmlElement::createPreformatted(nullptr, silverBG, { tgui::XhtmlElement::createBreak(), code01})) });
        tgui::XhtmlElement::addChildrenAndSetPatent(code01, {
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"cp"), tgui::XhtmlElement::createInnerText(nullptr, U"#include <TGUI/TGUI.hpp>")),
            tgui::XhtmlElement::createBreak(),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"cp"), tgui::XhtmlElement::createInnerText(nullptr, U"#include <TGUI/Backend/SFML-Graphics.hpp>")),
            tgui::XhtmlElement::createBreak(),
            tgui::XhtmlElement::createBreak(),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"kt"), tgui::XhtmlElement::createInnerText(nullptr, U"int")),
            tgui::XhtmlElement::createInnerText(nullptr, U" "),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"nf"), tgui::XhtmlElement::createInnerText(nullptr, U"main")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), tgui::XhtmlElement::createInnerText(nullptr, U"()")),
            tgui::XhtmlElement::createBreak(),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), tgui::XhtmlElement::createInnerText(nullptr, U"{")),
            tgui::XhtmlElement::createBreak(),
            tgui::XhtmlElement::createInnerText(nullptr, U"    "),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), tgui::XhtmlElement::createInnerText(nullptr, U"sf")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"o"), tgui::XhtmlElement::createInnerText(nullptr, U"::")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), tgui::XhtmlElement::createInnerText(nullptr, U"RenderWindow")),
            tgui::XhtmlElement::createInnerText(nullptr, U" "),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), tgui::XhtmlElement::createInnerText(nullptr, U"window")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), tgui::XhtmlElement::createInnerText(nullptr, U"{{")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"mi"), tgui::XhtmlElement::createInnerText(nullptr, U"800")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), tgui::XhtmlElement::createInnerText(nullptr, U",")),
            tgui::XhtmlElement::createInnerText(nullptr, U" "),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"mi"), tgui::XhtmlElement::createInnerText(nullptr, U"600")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), tgui::XhtmlElement::createInnerText(nullptr, U"},")),
            tgui::XhtmlElement::createInnerText(nullptr, U" "),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"s"), tgui::XhtmlElement::createInnerText(nullptr, U"\"TGUI example - SFML_GRAPHICS backend\"")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), tgui::XhtmlElement::createInnerText(nullptr, U"};")),
            tgui::XhtmlElement::createBreak(),
            tgui::XhtmlElement::createInnerText(nullptr, U"    "),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), tgui::XhtmlElement::createInnerText(nullptr, U"tgui")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"o"), tgui::XhtmlElement::createInnerText(nullptr, U"::")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), tgui::XhtmlElement::createInnerText(nullptr, U"Gui")),
            tgui::XhtmlElement::createInnerText(nullptr, U" "),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), tgui::XhtmlElement::createInnerText(nullptr, U"gui")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), tgui::XhtmlElement::createInnerText(nullptr, U"{")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), tgui::XhtmlElement::createInnerText(nullptr, U"window")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), tgui::XhtmlElement::createInnerText(nullptr, U"};")),
            tgui::XhtmlElement::createBreak(),
            tgui::XhtmlElement::createInnerText(nullptr, U"    "),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), tgui::XhtmlElement::createInnerText(nullptr, U"gui")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), tgui::XhtmlElement::createInnerText(nullptr, U".")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"n"), tgui::XhtmlElement::createInnerText(nullptr, U"mainLoop")),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), tgui::XhtmlElement::createInnerText(nullptr, U"();")),
            tgui::XhtmlElement::createInnerText(nullptr, U" "),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"cl"), tgui::XhtmlElement::createInnerText(nullptr, U"// See below for how to use your own main loop")),
            tgui::XhtmlElement::createBreak(),
            tgui::XhtmlElement::createSpan(nullptr, thmlStyle->getEntry(U"p"), tgui::XhtmlElement::createInnerText(nullptr, U"}"))});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Handle the onZoomChanged signal for the formatted text view 1.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void onZoomChangeView1(float newZoom)
    {
        if (m_zoomLabel1 != nullptr)
            m_zoomLabel1->setText(U"Current zoom is: " + tgui::String::fromNumber((int)(newZoom * 100.0f + 0.1f)) + U"%");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Handle the onZoomChanged signal for the formatted text view 2.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void onZoomChangeView2(float newZoom)
    {
        if (m_zoomLabel2 != nullptr)
            m_zoomLabel2->setText(U"Current zoom is: " + tgui::String::fromNumber((int)(newZoom * 100.0f + 0.1f)) + U"%");
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
        m_pageTabContainer = tgui::TabContainer::create();
        m_pageTabContainer->setPosition(0.0f, 0.0f);
        m_pageTabContainer->setSize("100%", "100%");
        auto tabPanel1 = m_pageTabContainer->addTab(U"created", true);
        auto tabPanel2 = m_pageTabContainer->addTab(U"parsed", true);
        this->getGui().add(m_pageTabContainer, U"MyTabContainer");

        std::shared_ptr<tgui::FormattedTextDocument::FontCollection> fontCollection = createFontCollection();
#ifdef _DEBUG
        std::cout << "'createFrameContent()'->font collection established" << std::endl;
#endif // _DEBUG

        auto formattedTextDocument1 = tgui::FormattedTextXhtmlDocument::create();
        m_formattedTextView1 = tgui::FormattedTextView::create();
        m_formattedTextView1->setFontCollection(*fontCollection);
        m_formattedTextView1->setDocument(formattedTextDocument1);
        m_formattedTextView1->setPosition("16", "16");
        tgui::Layout widthLayout1(tgui::Layout::Operation::Minus, std::make_unique<tgui::Layout>("100%"), std::make_unique<tgui::Layout>("32"));
        tgui::Layout hightLayout1(tgui::Layout::Operation::Minus, std::make_unique<tgui::Layout>("100%"), std::make_unique<tgui::Layout>("64"));
        m_formattedTextView1->setSize(widthLayout1, hightLayout1);
        tabPanel1->add(m_formattedTextView1, "MyFormattedTextArea-created");

        m_zoomLabel1 = tgui::Label::create(U"Current zoom is: 100%");
        tgui::Layout posYLayout1(tgui::Layout::Operation::Minus, std::make_unique<tgui::Layout>("100%"), std::make_unique<tgui::Layout>("48"));
        m_zoomLabel1->setPosition(32, posYLayout1);
        m_zoomLabel1->setSize(widthLayout1, 32);
        tabPanel1->add(m_zoomLabel1, "MyZoomLabel-created");

        auto formattedTextDocument2 = tgui::FormattedTextXhtmlDocument::create();
        m_formattedTextView2 = tgui::FormattedTextView::create();
        m_formattedTextView2->setFontCollection(*fontCollection);
        m_formattedTextView2->setDocument(formattedTextDocument2);
        m_formattedTextView2->setPosition("16", "16");
        tgui::Layout widthLayout2(tgui::Layout::Operation::Minus, std::make_unique<tgui::Layout>("100%"), std::make_unique<tgui::Layout>("32"));
        tgui::Layout hightLayout2(tgui::Layout::Operation::Minus, std::make_unique<tgui::Layout>("100%"), std::make_unique<tgui::Layout>("64"));
        m_formattedTextView2->setSize(widthLayout2, hightLayout2);
        tabPanel2->add(m_formattedTextView2, "MyFormattedTextArea-parsed");

        m_zoomLabel2 = tgui::Label::create(U"Current zoom is: 100%");
        tgui::Layout posYLayout2(tgui::Layout::Operation::Minus, std::make_unique<tgui::Layout>("100%"), std::make_unique<tgui::Layout>("48"));
        m_zoomLabel2->setPosition(32, posYLayout1);
        m_zoomLabel2->setSize(widthLayout2, 32);
        tabPanel2->add(m_zoomLabel2, "MyZoomLabel-parsed");

        buildDom(formattedTextDocument1);
        m_formattedTextView1->rearrangeText(true);
        m_formattedTextView1->onZoomChange(&XHTMLViewerMainForm::onZoomChangeView1, this);

        tgui::String hypertextString;
        tgui::String fileType;
        std::string testFilePath = "./test.htm";
        std::ifstream hypertextFile(testFilePath, std::ios::in | std::ios::binary);
        if (hypertextFile.is_open())
        {
            hypertextFile.seekg (0, std::ios::end);
            size_t fileSizeInByte = hypertextFile.tellg();
            hypertextFile.seekg (0, std::ios::beg);
            std::vector<char> data;
            data.resize(fileSizeInByte);
            hypertextFile.read(&data[0], fileSizeInByte);

            if (fileSizeInByte <= 2)
            {
                std::cerr << "ERROR: The XHTML test file seems to be empty.\n";
            }
            else if ((unsigned char)data[0] == 254 && (unsigned char)data[1] == 255)
            {
                if ((unsigned char)data[0] == 0)
                    fileType = U"UTF-16 BE";
                else
                    fileType = U"UTF-16 LE";
            }
            else if ((unsigned char)data[0] == 254 && (unsigned char)data[1] == 239)
            {
                fileType = U"UTF-8";
            }
            else
            {
                if (tgui::UtfHelper::checkValid(data))
                    fileType = U"UTF-8";
                else
                    fileType = U"ASCII";

                data.push_back(0);
                hypertextString = tgui::String(data.data());
            }

            hypertextFile.close();
        }
        else
            std::cerr << "ERROR: Unable to load XHTML test file.\n";

        tgui::XhtmlParser hypertextParser(hypertextString);
        hypertextParser.parseDocument();
        auto htmlElement = hypertextParser.getFirstRootElement("html");
        if (htmlElement != nullptr)
            htmlElement->trace(L"");
        auto errorMessages = hypertextParser.getErrorMessages();
        for (size_t index = 0; index < errorMessages->size(); index++)
            std::wcerr << errorMessages->at(index) << std::endl;
        auto warningMessages = hypertextParser.getWarningMessages();
        for (size_t index = 0; index < warningMessages->size(); index++)
            std::wcout << warningMessages->at(index) << std::endl;
        auto html = std::dynamic_pointer_cast<tgui::XhtmlContainerElement>(htmlElement);
        if (html != nullptr)
            formattedTextDocument2->setRootElement(html);
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
    tgui::Gui gui(window);
    std::string themeFilePath = "./themes/BabyBlue.txt";
    std::ifstream themeFile(themeFilePath);
    if (!themeFile.is_open())
        std::cerr << "ERROR: Unable to load theme.\n";
    else
        tgui::Theme::setDefault(themeFilePath);
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
