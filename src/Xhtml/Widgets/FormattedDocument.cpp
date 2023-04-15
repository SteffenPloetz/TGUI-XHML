#include <TGUI/Config.hpp>
#include <TGUI/TGUI.hpp>

// MSC needs a clear distiction between "__declspec(dllimport)" (above) and "__declspec(dllexport)" (below) this comment.
// So in the case of direct source file integration (in contrast to library creation and linking), the API must be 'dllexport'.
#ifndef TGUI_STATIC
#ifdef TGUI_SYSTEM_WINDOWS
#undef TGUI_API
#define TGUI_API __declspec(dllexport)
#endif
#endif

#include "TGUI/String.hpp"
#include "TGUI/Xhtml/StringHelper.hpp"
#include "TGUI/Xhtml/Widgets/FormattedDocument.hpp"

namespace tgui  { namespace xhtml
{

    std::shared_ptr<FormattedDocument::FontCollection> FormattedDocument::FontCollection::platformOptimizedFontCollection()
    {
        auto fontCollection = std::make_shared<FormattedDocument::FontCollection>();

        fontCollection->Sans = std::make_shared<FormattedDocument::FontPack>();
        fontCollection->Serif = std::make_shared<FormattedDocument::FontPack>();
        fontCollection->Mono = std::make_shared<FormattedDocument::FontPack>();

        // ------------------------------------------------------------------------------------------------
        // We can achieve a higher display quality for bold text if - instead of setting m_textStyleCached
        // to TextStyle::Bold - we use our own font. Therefore, the Bold and BoldItalic fonts are provided.
        // ------------------------------------------------------------------------------------------------

#if defined(TGUI_SYSTEM_WINDOWS)
        tgui::String fontRoot = L"C:\\Windows\\Fonts\\";

        fontCollection->Sans->Regular = tgui::Font(fontRoot + L"arial.ttf");
        fontCollection->Sans->Bold = tgui::Font(fontRoot + L"arialbd.ttf");
        fontCollection->Sans->Italic = tgui::Font(fontRoot + L"ariali.ttf");
        fontCollection->Sans->BoldItalic = tgui::Font(fontRoot + L"arialbi.ttf");

        fontCollection->Serif->Regular = tgui::Font(fontRoot + L"times.ttf");
        fontCollection->Serif->Bold = tgui::Font(fontRoot + L"timesbd.ttf");
        fontCollection->Serif->Italic = tgui::Font(fontRoot + L"timesi.ttf");
        fontCollection->Serif->BoldItalic = tgui::Font(fontRoot + L"timesbi.ttf");

        fontCollection->Mono->Regular = tgui::Font(fontRoot + L"consola.ttf"); // L"cour.ttf");
        fontCollection->Mono->Bold = tgui::Font(fontRoot + L"consolab.ttf"); // L"courbd.ttf");
        fontCollection->Mono->Italic = tgui::Font(fontRoot + L"consolai.ttf"); // L"couri.ttf");
        fontCollection->Mono->BoldItalic = tgui::Font(fontRoot + L"consolaz.ttf"); // L"courbi.ttf");
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
            fontCollection->Sans->Regular = tgui::Font(fontRoot + L"truetype/Roboto-Regular.ttf");
            fontCollection->Sans->Bold = tgui::Font(fontRoot + L"truetype/Roboto-Bold.ttf");
            fontCollection->Sans->Italic = tgui::Font(fontRoot + L"truetype/Roboto-Italic.ttf");
            fontCollection->Sans->BoldItalic = tgui::Font(fontRoot + L"truetype/Roboto-BoldItalic.ttf");

            fontCollection->Serif->Regular = tgui::Font(fontRoot + L"truetype/DejaVuSerif.ttf");
            fontCollection->Serif->Bold = tgui::Font(fontRoot + L"truetype/DejaVuSerif-Bold.ttf");
            fontCollection->Serif->Italic = tgui::Font(fontRoot + L"truetype/DejaVuSerif-Italic.ttf");
            fontCollection->Serif->BoldItalic = tgui::Font(fontRoot + L"truetype/DejaVuSerif-BoldItalic.ttf");

            fontCollection->Mono->Regular = tgui::Font(fontRoot + L"truetype/DejaVuSansMono.ttf");
            fontCollection->Mono->Bold = tgui::Font(fontRoot + L"truetype/DejaVuSansMono-Bold.ttf");
            fontCollection->Mono->Italic = tgui::Font(fontRoot + L"truetype/DejaVuSansMono-Oblique.ttf");
            fontCollection->Mono->BoldItalic = tgui::Font(fontRoot + L"truetype/DejaVuSansMono-BoldOblique.ttf");
        }
        // Manjaro
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        else if (std::filesystem::exists(manjaroSansPath))
#else
        if (std::experimental::filesystem::exists(manjaroSansPath))
#endif
        {
            fontCollection->Sans->Regular = tgui::Font(fontRoot + L"TTF/DejaVuSans.ttf");
            fontCollection->Sans->Bold = tgui::Font(fontRoot + L"TTF/DejaVuSans-Bold.ttf");
            fontCollection->Sans->Italic = tgui::Font(fontRoot + L"TTF/DejaVuSans-Oblique.ttf");
            fontCollection->Sans->BoldItalic = tgui::Font(fontRoot + L"TTF/DejaVuSans-BoldOblique.ttf");

            fontCollection->Serif->Regular = tgui::Font(fontRoot + L"TTF/DejaVuSerif.ttf");
            fontCollection->Serif->Bold = tgui::Font(fontRoot + L"TTF/DejaVuSerif-Bold.ttf");
            fontCollection->Serif->Italic = tgui::Font(fontRoot + L"TTF/DejaVuSerif-Italic.ttf");
            fontCollection->Serif->BoldItalic = tgui::Font(fontRoot + L"TTF/DejaVuSerif-BoldItalic.ttf");

            fontCollection->Mono->Regular = tgui::Font(fontRoot + L"TTF/DejaVuSansMono.ttf");
            fontCollection->Mono->Bold = tgui::Font(fontRoot + L"TTF/DejaVuSansMono-Bold.ttf");
            fontCollection->Mono->Italic = tgui::Font(fontRoot + L"TTF/DejaVuSansMono-Oblique.ttf");
            fontCollection->Mono->BoldItalic = tgui::Font(fontRoot + L"TTF/DejaVuSansMono-BoldOblique.ttf");
        }
        else
        {
            std::cerr << "ERROR: The fonts can not be loaded.\n";
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
        fontCollection->Sans->Regular.setSmooth(false);
        fontCollection->Sans->Bold.setSmooth(false);
        fontCollection->Sans->Italic.setSmooth(false);
        fontCollection->Sans->BoldItalic.setSmooth(false);

        return fontCollection;
    }

} }
