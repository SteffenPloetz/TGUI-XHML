#include <TGUI/Config.hpp>
#include <TGUI/TGUI.hpp>

#include <algorithm>

// MSC needs a clear distiction between "__declspec(dllimport)" (above) and "__declspec(dllexport)" (below) this comment.
// So in the case of direct source file integration (in contrast to library creation and linking), the API must be 'dllexport'.
#ifndef TGUI_STATIC
#ifdef TGUI_SYSTEM_WINDOWS
#undef TGUI_API
#define TGUI_API __declspec(dllexport)
#endif
#endif

#include "TGUI/StringHelper.hpp"
#include "TGUI/Xhtml/XhtmlEntityResolver.hpp"
namespace ext
{
    int isSpace(char32_t character)
    {
#if _WIN32
        return std::iswspace((wchar_t)character);
#else
        return std::iswspace(character);
#endif
    }

    int isAlpha(char32_t character)
    {
#if _WIN32
        return std::iswalpha((wchar_t)character);
#else
        return std::iswalpha(character);
#endif
    }

    int isAlnum(char32_t character)
    {
#if _WIN32
        return std::iswalnum((wchar_t)character);
#else
        return std::iswalnum(character);
#endif
    }

    int isUpper(char32_t character)
    {
#if _WIN32
        return std::iswupper((wchar_t)character);
#else
        return std::iswupper(character);
#endif
    }
}


namespace tgui
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<tgui::String, char32_t> XhtmlEntityResolver::m_charEntityRefs
    {
        /** C0 Controls and Basic Latin */
        {  U"quot",    U'\x22'  },
        {  U"amp",     U'\x26'  },
        {  U"apos",    U'\x27'  },
        {  U"lt",      U'\x3C'  },
        {  U"gt",      U'\x3E'  },
        /** ISO 8859-1 (Latin-1) characters */
#if defined(TGUI_SYSTEM_WINDOWS)
        {  U"nbsp",    U'\x20'  },
#else
        {  U"nbsp",    U'\xA0'  },
#endif
        {  U"iexcl",   U'\xA1'  },
        {  U"cent",    U'\xA2'  },
        {  U"pound",   U'\xA3'  },
        {  U"current", U'\xA4'  },
        {  U"yen",     U'\xA5'  },
        {  U"brvbar",  U'\xA6'  },
        {  U"sect",    U'\xA7'  },
        {  U"uml",     U'\xA8'  },
        {  U"copy",    U'\xA9'  },
        {  U"ordf",    U'\xAA'  },
        {  U"laquo",   U'\xAB'  },
        {  U"not",     U'\xAC'  },
        {  U"shy",     U'\xAD'  },
        {  U"reg",     U'\xAE'  },
        {  U"macr",    U'\xAF'  },
        {  U"deg",     U'\xB0'  },
        {  U"plusmn",  U'\xB1'  },
        {  U"sup2",    U'\xB2'  },
        {  U"sup3",    U'\xB3'  },
        {  U"acute",   U'\xB4'  },
        {  U"micro",   U'\xB5'  },
        {  U"para",    U'\xB6'  },
        {  U"middot",  U'\xB7'  },
        {  U"cedil",   U'\xB8'  },
        {  U"sup1",    U'\xB9'  },
        {  U"ordm",    U'\xBA'  },
        {  U"raquo",   U'\xBB'  },
        {  U"frac14",  U'\xBC'  },
        {  U"frac12",  U'\xBD'  },
        {  U"frac34",  U'\xBE'  },
        {  U"iquest",  U'\xBF'  },
        {  U"Agrave",  U'\xC0'  },
        {  U"Aacute",  U'\xC1'  },
        {  U"Acirc",   U'\xC2'  },
        {  U"Atilde",  U'\xC3'  },
        {  U"Auml",    U'\xC4'  },
        {  U"Aring",   U'\xC5'  },
        {  U"AElig",   U'\xC6'  },
        {  U"Ccedil",  U'\xC7'  },
        {  U"Egrave",  U'\xC8'  },
        {  U"Eacute",  U'\xC9'  },
        {  U"Ecirc",   U'\xCA'  },
        {  U"Euml",    U'\xCB'  },
        {  U"Igrave",  U'\xCC'  },
        {  U"Iacute",  U'\xCD'  },
        {  U"Icirc",   U'\xCE'  },
        {  U"Iuml",    U'\xCF'  },
        {  U"ETH",     U'\xD0'  },
        {  U"Ntilde",  U'\xD1'  },
        {  U"Ograve",  U'\xD2'  },
        {  U"Oacute",  U'\xD3'  },
        {  U"Ocirc",   U'\xD4'  },
        {  U"Otilde",  U'\xD5'  },
        {  U"Ouml",    U'\xD6'  },
        {  U"times",   U'\xD7'  },
        {  U"Oslash",  U'\xD8'  },
        {  U"Ugrave",  U'\xD9'  },
        {  U"Uacute",  U'\xDA'  },
        {  U"Ucirc",   U'\xDB'  },
        {  U"Uuml",    U'\xDC'  },
        {  U"Yacute",  U'\xDD'  },
        {  U"THORN",   U'\xDE'  },
        {  U"szlig",   U'\xDF'  },
        {  U"agrave",  U'\xE0'  },
        {  U"aacute",  U'\xE1'  },
        {  U"acirc",   U'\xE2'  },
        {  U"atilde",  U'\xE3'  },
        {  U"auml",    U'\xE4'  },
        {  U"aring",   U'\xE5'  },
        {  U"aelig",   U'\xE6'  },
        {  U"ccedil",  U'\xE7'  },
        {  U"egrave",  U'\xE8'  },
        {  U"eacute",  U'\xE9'  },
        {  U"ecirc",   U'\xEA'  },
        {  U"euml",    U'\xEB'  },
        {  U"igrave",  U'\xEC'  },
        {  U"iacute",  U'\xED'  },
        {  U"icirc",   U'\xEE'  },
        {  U"iuml",    U'\xEF'  },
        {  U"eth",     U'\xF0'  },
        {  U"ntilde",  U'\xF1'  },
        {  U"ograve",  U'\xF2'  },
        {  U"oacute",  U'\xF3'  },
        {  U"ocirc",   U'\xF4'  },
        {  U"otilde",  U'\xF5'  },
        {  U"ouml",    U'\xF6'  },
        {  U"divide",  U'\xF7'  },
        {  U"oslash",  U'\xF8'  },
        {  U"ugrave",  U'\xF9'  },
        {  U"uacute",  U'\xFA'  },
        {  U"ucirc",   U'\xFB'  },
        {  U"uuml",    U'\xFC'  },
        {  U"yacute",  U'\xFD'  },
        {  U"thorn",   U'\xFE'  },
        {  U"yuml",    U'\xFF'  }
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int XhtmlEntityResolver::resolveEntity(std::vector<std::tuple<tgui::MessageType,String>>& messages, char32_t* encode, char32_t& substitute)
    {
        if (m_charEntityRefs.empty())
        {
            tgui::String message(U"TinyXhtmlEntityResolver::resolveEntity() -> Entity references not initialized!");
            messages.push_back(std::make_tuple(MessageType::ERROR, message));
            return 0;
        }

        if (encode == nullptr)
        {
            tgui::String message(U"TinyXhtmlEntityResolver::resolveEntity() -> Entity to encode not valid!");
            messages.push_back(std::make_tuple(MessageType::ERROR, message));
            return 0;
        }

        char32_t* begin = encode;
        int      length = 0;
        while (encode[length] != U';' && length < 10)
            length++;
        if (length >= 10)
            return 0; // throw "TinyXhtmlEntityResolver::resolveEntity() -> Buffer overrun on search for end marker!"

        // entity references always end with a semi-colon ';'
        if (length == 0)
            return 0; // throw "TinyXhtmlEntityResolver::resolveEntity() -> No end marker found!"

        // skip leading white-space characters
        while (ext::isSpace(*begin) && length > 0)
        {
            begin++; length--;
        }

        // remaining string (including semi-colon)
        // must be at least 4 characters in length
        if (length < 3)
            return 0; // throw "TinyXhtmlEntityResolver::resolveEntity() -> Buffer underrun on search for end marker!"

        // entity references always begin with an ampersand '&' symbol
        if (*begin != U'&')
            return 0; // throw "TinyXhtmlEntityResolver::resolveEntity() -> No begin marker!"

        begin++;

        // numeric (decimal or hexadecimal) entity reference?
        if (*begin == U'#')
        {
            begin++;
            char32_t chTemp = *begin;
            int	radix = (std::isdigit(chTemp) ? 10 :
                (chTemp == U'x' || chTemp == U'X' ? 16 : 0));
            if (radix != 0)
            {
                if (radix == 16)
                    begin++;

                unsigned long ulNum = toULong(begin, radix);
                substitute = (char32_t)ulNum;
                length++;
                return length;
            }
        }

        // character entity reference
        else
        {
            tgui::String	strKey(begin, length - 1);

            // because some character entity references are
            // case-sensitive, we must fix them manually
            if (strKey.equalIgnoreCase(U"eth") ||
                strKey.equalIgnoreCase(U"thorn"))
            {
                if (ext::isUpper(strKey[0]))
                    std::transform(strKey.begin(), strKey.end(), strKey.begin(), ::toupper);
                else
                    std::transform(strKey.begin(), strKey.end(), strKey.begin(), ::tolower);
            }
            else if (strKey.equalIgnoreCase(U"Oslash"))
            {
                std::transform(strKey.begin(), strKey.end(), strKey.begin(), ::tolower);
                strKey[0] = U'O';
            }
            else if (strKey.equalIgnoreCase(U"AElig"))
            {
                std::transform(strKey.begin(), strKey.end(), strKey.begin(), ::tolower);
                strKey[0] = U'A';
                strKey[1] = U'E';
            }
            else
            {
                char32_t     chrHead = strKey[0];
                tgui::String strTail = strKey.substr(1);
                std::transform(strKey.begin(), strKey.end(), strKey.begin(), ::tolower);
                if (strTail.equalIgnoreCase(U"grave") ||
                    strTail.equalIgnoreCase(U"acute") ||
                    strTail.equalIgnoreCase(U"circ")  ||
                    strTail.equalIgnoreCase(U"uml")   ||
                    strTail.equalIgnoreCase(U"tilde") ||
                    strTail.equalIgnoreCase(U"cedil") ||
                    strTail.equalIgnoreCase(U"ring")     )
                {
                    strKey[0] = chrHead; // strTail[0];
                }
            }

            // is this a known entity reference?
            auto iterator = m_charEntityRefs.find(strKey);
            if (iterator != m_charEntityRefs.end())
            {
                substitute = iterator->second;
                length++;
                return length;
            }
        }

        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned long XhtmlEntityResolver::toULong(const char32_t* value, int radix)
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
