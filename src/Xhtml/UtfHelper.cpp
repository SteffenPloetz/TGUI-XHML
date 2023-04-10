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

#include "TGUI/Xhtml/UtfHelper.hpp"

namespace tgui  { namespace xhtml
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool UtfHelper::checkValid(const std::vector<char>& data, bool utf8)
    {
        int    c;
        size_t ix = data.size();
        int    n; // number of bytes, used to encode the code point

        for (size_t i = 0; i < ix; i++)
        {
            c = (unsigned char)data[i];
            //if (c==0x09 || c==0x0a || c==0x0d || (0x20 <= c && c <= 0x7e) ) n = 0; // is_printable_ascii
            if      (0x00 <= c && c <= 0x7f)      n=0; // 0bbbbbbb
            else if (         (c & 0xE0) == 0xC0) n=1; // 110bbbbb
            else if (c == 0xed && i<(ix-1) && ((unsigned char)data[i+1] & 0xa0)==0xa0) return false; //U+d800 to U+dfff
            else if (         (c & 0xF0) == 0xE0) n=2; // 1110bbbb
            else if (         (c & 0xF8) == 0xF0) n=3; // 11110bbb
            else if (!utf8 && (c & 0xFC) == 0xF8) n=4; // 111110bb //byte 5, unnecessary in 4 byte UTF-8
            else if (!utf8 && (c & 0xFE) == 0xFC) n=5; // 1111110b //byte 6, unnecessary in 4 byte UTF-8
            else return false;

            for (int j = 0; j < n && i < ix; j++)
            {   // n bytes matching 10bbbbbb follow ?
                if ((++i == ix) || (( (unsigned char)data[i] & 0xC0) != 0x80))
                    return false;
            }
        }
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} }
