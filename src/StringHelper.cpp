#include <TGUI/Config.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Vector2.hpp>

#include <cuchar>
#include <codecvt>
#include <locale>
#include <string>

// MSC needs a clear distiction between "__declspec(dllimport)" (above) and "__declspec(dllexport)" (below) this comment.
// So in the case of direct source file integration (in contrast to library creation and linking), the API must be 'dllexport'.
#ifndef TGUI_STATIC
#ifdef TGUI_SYSTEM_WINDOWS
#undef TGUI_API
#define TGUI_API __declspec(dllexport)
#endif
#endif

#include "TGUI/StringHelper.hpp"

namespace ext
{
    std::vector<tgui::String> String::split(const tgui::String& s, char32_t separator, bool suppressEmpty)
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<tgui::String> String::split(const tgui::String& s, const tgui::String& separator, bool suppressEmpty)
    {
        std::vector<tgui::String> output;
        std::size_t prev_pos = 0, pos = 0;

        while((pos = s.find(separator, pos)) != tgui::String::npos)
        {
            tgui::String substring(s.substr(prev_pos, pos - prev_pos));
            if (substring.size() > 0 || !suppressEmpty)
                output.push_back(substring.c_str());
            pos += separator.size();
            prev_pos = pos;
            pos++;
        }
        if (s.size() > 0 && prev_pos < s.size())
            output.push_back(s.substr(prev_pos, s.size() - prev_pos));
        else if (!suppressEmpty)
            output.push_back(U"");

        return output;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

namespace ext
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int string::strcasecmp(const char* s1, const char* s2)
    {
        while (*s1 != 0 && *s2 != 0 && tolower(*s1) == tolower(*s2))
        {
            ++s1;
            ++s2;
        }

        return (*s2 == 0) ? (*s1 != 0) : ((*s1 == 0) ? -1 : (tolower(*s1) - tolower(*s2)));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
