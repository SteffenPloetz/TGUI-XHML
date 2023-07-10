#include <cuchar>
#include <codecvt>
#include <locale>
#include <string>
#include <cstdint>

#include <TGUI/Config.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Vector2.hpp>

// MSC needs a clear distiction between "__declspec(dllimport)" (above) and "__declspec(dllexport)" (below) this comment.
// So in the case of direct source file integration (in contrast to library creation and linking), the API must be 'dllexport'.
#ifndef TGUI_STATIC
#ifdef TGUI_SYSTEM_WINDOWS
#undef TGUI_API
#define TGUI_API __declspec(dllexport)
#endif
#endif

#include "TGUI/Xhtml/StringHelper.hpp"

namespace tgui  { namespace xhtml
{
    std::vector<tgui::String> StringEx::split(const tgui::String& s, char32_t separator, bool suppressEmpty)
    {
        std::vector<tgui::String> output;
        std::size_t prev_pos = 0, pos = 0;

        while((pos = s.find(separator, pos)) != tgui::String::npos)
        {
            tgui::String substring(s.substr(prev_pos, pos-prev_pos));
            if (substring.size() > 0 || !suppressEmpty)
                output.push_back(substring.c_str());
            prev_pos = ++pos;
        }
        if (s.size() > 0 && prev_pos < s.size())
            output.push_back(s.substr(prev_pos, s.size() - prev_pos));
        else if (!suppressEmpty)
            output.push_back(U"");

        return output;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::String StringEx::trimLeft(const tgui::String& s)
    {
        const auto firstIt = std::find_if(s.begin(), s.end(), [](char32_t c) { return (c >= 256) || !std::isspace(static_cast<unsigned char>(c)); });
        if (firstIt == s.end())
            return {};

        return tgui::String(firstIt, s.end());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::String StringEx::trimRight(const tgui::String& s)
    {
        const auto lastIt = std::find_if(s.rbegin(), s.rend(), [](char32_t c) { return (c >= 256) || !std::isspace(static_cast<unsigned char>(c)); }).base();
        return tgui::String(s.begin(), lastIt);
    }

} }
