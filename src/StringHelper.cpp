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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const tgui::String String::m_emptyString = U"";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int String::compareIgnoreCase(const tgui::String& nocaseLeft, const tgui::String& caseRight)
    {
        tgui::String buffLeft = nocaseLeft;
        std::transform(buffLeft.begin(), buffLeft.end(), buffLeft.begin(), ::tolower);

        return buffLeft.compare(caseRight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool String::containsIgnoreCase(const tgui::String& nocaseLeft, const tgui::String& pattern)
    {
        tgui::String buffLeft = nocaseLeft;
        std::transform(buffLeft.begin(), buffLeft.end(), buffLeft.begin(), ::tolower);

        return buffLeft.find(pattern, 0) != tgui::String::npos;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool String::containsAnyIgnoreCase(const tgui::String& nocaseLeft, const std::vector<tgui::String>& patterns)
    {
        for (auto pattern : patterns)
            if (String::containsIgnoreCase(nocaseLeft, pattern) == 0)
                return true;

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::remove(tgui::String& text, char32_t pattern)
    {
        text.erase(std::remove(text.begin(), text.end(), pattern), text.end());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void String::replace(tgui::String& text, const tgui::String& pattern, const tgui::String& replacer, bool recursive)
    {
        std::size_t pos = 0;
        while ((pos = text.find(pattern, pos)) != tgui::String::npos)
        {
            text.replace(pos, pattern.length(), replacer);
            if (recursive)
                pos = 0;
            else
                pos += replacer.length();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    std::string String::toCharString(const tgui::String& text)
    {
        size_t textLen = text.size();
        if (textLen == 0)
            return "";

        auto result = std::string(textLen, '\0');
        for (size_t index = 0; index < textLen; index++)
            result[index] = static_cast<char>(text[index]);

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::String String::fromUtf8(const std::vector<char>& data)
    {
#if _MSC_VER >= 1900
#if _MSVC_LANG < 201703L
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        $$ // Library is compiled with version >= C++17! This code can not be compiled with version < C++17!
            throw "Compiler version mismatch!";
#else
        std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> utf32conv;
        auto result = utf32conv.from_bytes(data.data());
        // use: reinterpret_cast<const char32_t *>(result.c_str())
        return result;
#endif
#else
        if (data.size() == 0)
            return String::m_emptyString;

        std::shared_ptr<char32_t> spResult(new char32_t[data.size() + 1], std::default_delete<char32_t[]>());

        size_t      nLoops = 0;
        const char* pc8start = data.data();
        const char* pc8end = data.data() + data.size();
        char32_t    c32 = 0;
        mbstate_t   state{}; // zero-initialized
        size_t      length = 0;

        while ((length = std::mbrtoc32(&c32, pc8start, pc8end - pc8start, &state)) >= 0)
        {
            assert(length != (size_t)-3);    // no surrogates in UTF-32
            if (length == (size_t)-1) break; // for bad
            if (length == (size_t)-2) break; // for bad
            if (length == (size_t)0) break; // for good

            spResult.get()[nLoops] = c32;
            nLoops++;
            pc8start += length;
        }
        spResult.get()[nLoops] = 0;

        return spResult.get();
#endif
#elif _MSC_VER >= 600
        $$ // Visual Studio must be at least Version 2015!
            throw "Compiler version outdated!";
#else // None Microsoft compiler.
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32conv;
        auto result = utf32conv.from_bytes(data.data());
        // use: result.c_str()
        return result;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::String String::fromInt(int value)
    {
        std::ostringstream s;
        s << value;
        tgui::String result(s.str().c_str());

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned long String::toULong(const char32_t* value, int radix)
    {
        std::string buffer;
        while (*value == U'-' || *value == U'0' || *value == U'1' || *value == U'2' || *value == U'3' || *value == U'4' ||
            *value == U'5' || *value == U'6' || *value == U'7' || *value == U'8' || *value == U'9')
        {
            buffer.push_back((char)*value);
            value++;
        }
        return std::stoul(buffer, nullptr, radix);
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
