#include <fstream>
#include <cstdint>

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

#include "TGUI/Xhtml/MarkupLanguageElement.hpp"

namespace tgui  { namespace xhtml
{
#if TGUI_COMPILED_WITH_CPP_VER < 17 && !defined(_MSC_VER)
    constexpr const char  MarkupLanguageElement::Anchor[];
#endif
} }
