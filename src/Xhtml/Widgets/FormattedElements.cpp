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
#include "TGUI/Xhtml/Dom/XhtmlEntityResolver.hpp"
#include "TGUI/Xhtml/Dom/XhtmlAttributes.hpp"
#include "TGUI/Xhtml/Dom/XhtmlElements.hpp"

#include "TGUI/Xhtml/Widgets/FormattedElements.hpp"

namespace tgui  { namespace xhtml
{

} }
