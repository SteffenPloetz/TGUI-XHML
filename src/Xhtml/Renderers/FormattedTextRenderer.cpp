#include <cstdint>

#include <TGUI/Config.hpp>
#include <TGUI/RendererDefines.hpp>
#include <TGUI/Renderers/WidgetRenderer.hpp>

// MSC needs a clear distiction between "__declspec(dllimport)" (above) and "__declspec(dllexport)" (below) this comment.
// So in the case of direct source file integration (in contrast to library creation and linking), the API must be 'dllexport'.
#ifndef TGUI_STATIC
#ifdef TGUI_SYSTEM_WINDOWS
#undef TGUI_API
#define TGUI_API __declspec(dllexport)
#endif
#endif

#include "TGUI/Xhtml/Renderers/FormattedTextRenderer.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui  { namespace xhtml
{

    TGUI_RENDERER_PROPERTY_OUTLINE(FormattedTextRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(FormattedTextRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(FormattedTextRenderer, BackgroundColor, Color(255, 255, 255))
    TGUI_RENDERER_PROPERTY_COLOR(FormattedTextRenderer, TextColor, Color(60, 60, 60))
    TGUI_RENDERER_PROPERTY_COLOR(FormattedTextRenderer, DefaultTextColor, Color(60, 60, 60))
    TGUI_RENDERER_PROPERTY_COLOR(FormattedTextRenderer, SelectedTextColor, Color(255, 255, 255))
    TGUI_RENDERER_PROPERTY_COLOR(FormattedTextRenderer, SelectedTextBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(FormattedTextRenderer, BorderColor, Color(0, 0, 0))
    TGUI_RENDERER_PROPERTY_COLOR(FormattedTextRenderer, CaretColor, Color(0, 0, 0))

    TGUI_RENDERER_PROPERTY_NUMBER(FormattedTextRenderer, CaretWidth, 1)

    TGUI_RENDERER_PROPERTY_RENDERER(FormattedTextRenderer, Scrollbar, "Scrollbar")
    TGUI_RENDERER_PROPERTY_NUMBER(FormattedTextRenderer, ScrollbarWidth, 0)
} }
