#include <TGUI/Config.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Vector2.hpp>

#include <algorithm>

// #define LOG_ATTRIBUTE 1

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
#include "TGUI/Xhtml/XhtmlAttributes.hpp"

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The list of known web color names mapped to their color value
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::map<tgui::String, tgui::String> WebColorMap = {
        {  U"maroon"                ,	U"#800000"   },
        {  U"darkred"               ,	U"#8B0000"   },
        {  U"brown"                 ,	U"#A52A2A"   },
        {  U"firebrick"             ,	U"#B22222"   },
        {  U"crimson"               ,	U"#DC143C"   },
        {  U"red"                   ,	U"#FF0000"   },
        {  U"tomato"                ,	U"#FF6347"   },
        {  U"coral"                 ,	U"#FF7F50"   },
        {  U"indianred"             ,	U"#CD5C5C"   },
        {  U"lightcoral"            ,	U"#F08080"   },
        {  U"darksalmon"            ,	U"#E9967A"   },
        {  U"salmon"                ,	U"#FA8072"   },
        {  U"lightsalmon"           ,	U"#FFA07A"   },
        {  U"orangered"             ,	U"#FF4500"   },
        {  U"darkorange"            ,	U"#FF8C00"   },
        {  U"orange"                ,	U"#FFA500"   },
        {  U"gold"                  ,	U"#FFD700"   },
        {  U"darkgoldenrod"         ,	U"#B8860B"   },
        {  U"goldenrod"             ,	U"#DAA520"   },
        {  U"palegoldenrod"         ,	U"#EEE8AA"   },
        {  U"darkkhaki"             ,	U"#BDB76B"   },
        {  U"khaki"                 ,	U"#F0E68C"   },
        {  U"olive"                 ,	U"#808000"   },
        {  U"yellow"                ,	U"#FFFF00"   },
        {  U"yellowgreen"           ,	U"#9ACD32"   },
        {  U"darkolivegreen"        ,	U"#556B2F"   },
        {  U"olivedrab"             ,	U"#6B8E23"   },
        {  U"lawngreen"             ,	U"#7CFC00"   },
        {  U"chartreuse"            ,	U"#7FFF00"   },
        {  U"greenyellow"           ,	U"#ADFF2F"   },
        {  U"darkgreen"             ,	U"#006400"   },
        {  U"green"                 ,	U"#008000"   },
        {  U"forestgreen"           ,	U"#228B22"   },
        {  U"lime"                  ,	U"#00FF00"   },
        {  U"limegreen"             ,	U"#32CD32"   },
        {  U"lightgreen"            ,	U"#90EE90"   },
        {  U"palegreen"             ,	U"#98FB98"   },
        {  U"darkseagreen"          ,	U"#8FBC8F"   },
        {  U"mediumspringgreen"     ,	U"#00FA9A"   },
        {  U"springgreen"           ,	U"#00FF7F"   },
        {  U"seagreen"              ,	U"#2E8B57"   },
        {  U"mediumaquamarine"      ,	U"#66CDAA"   },
        {  U"mediumseagreen"        ,	U"#3CB371"   },
        {  U"lightseagreen"         ,	U"#20B2AA"   },
        {  U"darkslategray"         ,	U"#2F4F4F"   },
        {  U"teal"                  ,	U"#008080"   },
        {  U"darkcyan"              ,	U"#008B8B"   },
        {  U"aqua"                  ,	U"#00FFFF"   },
        {  U"cyan"                  ,	U"#00FFFF"   },
        {  U"lightcyan"             ,	U"#E0FFFF"   },
        {  U"darkturquoise"         ,	U"#00CED1"   },
        {  U"turquoise"             ,	U"#40E0D0"   },
        {  U"mediumturquoise"       ,	U"#48D1CC"   },
        {  U"paleturquoise"         ,	U"#AFEEEE"   },
        {  U"aquamarine"            ,	U"#7FFFD4"   },
        {  U"powderblue"            ,	U"#B0E0E6"   },
        {  U"cadetblue"             ,	U"#5F9EA0"   },
        {  U"steelblue"             ,	U"#4682B4"   },
        {  U"cornflowerblue"        ,	U"#6495ED"   },
        {  U"deepskyblue"           ,	U"#00BFFF"   },
        {  U"dodgerblue"            ,	U"#1E90FF"   },
        {  U"lightblue"             ,	U"#ADD8E6"   },
        {  U"skyblue"               ,	U"#87CEEB"   },
        {  U"lightskyblue"          ,	U"#87CEFA"   },
        {  U"midnightblue"          ,	U"#191970"   },
        {  U"navy"                  ,	U"#000080"   },
        {  U"darkblue"              ,	U"#00008B"   },
        {  U"mediumblue"            ,	U"#0000CD"   },
        {  U"blue"                  ,	U"#0000FF"   },
        {  U"royalblue"             ,	U"#4169E1"   },
        {  U"blueviolet"            ,	U"#8A2BE2"   },
        {  U"indigo"                ,	U"#4B0082"   },
        {  U"darkslateblue"         ,	U"#483D8B"   },
        {  U"slateblue"             ,	U"#6A5ACD"   },
        {  U"mediumslateblue"       ,	U"#7B68EE"   },
        {  U"mediumpurple"          ,	U"#9370DB"   },
        {  U"darkmagenta"           ,	U"#8B008B"   },
        {  U"darkviolet"            ,	U"#9400D3"   },
        {  U"darkorchid"            ,	U"#9932CC"   },
        {  U"mediumorchid"          ,	U"#BA55D3"   },
        {  U"purple"                ,	U"#800080"   },
        {  U"thistle"               ,	U"#D8BFD8"   },
        {  U"plum"                  ,	U"#DDA0DD"   },
        {  U"violet"                ,	U"#EE82EE"   },
        {  U"fuchsia"               ,	U"#FF00FF"   },
        {  U"orchid"                ,	U"#DA70D6"   },
        {  U"mediumvioletred"       ,	U"#C71585"   },
        {  U"palevioletred"         ,	U"#DB7093"   },
        {  U"deeppink"              ,	U"#FF1493"   },
        {  U"hotpink"               ,	U"#FF69B4"   },
        {  U"lightpink"             ,	U"#FFB6C1"   },
        {  U"pink"                  ,	U"#FFC0CB"   },
        {  U"antiquewhite"          ,	U"#FAEBD7"   },
        {  U"beige"                 ,	U"#F5F5DC"   },
        {  U"bisque"                ,	U"#FFE4C4"   },
        {  U"blanchedalmond"        ,	U"#FFEBCD"   },
        {  U"wheat"                 ,	U"#F5DEB3"   },
        {  U"cornsilk"              ,	U"#FFF8DC"   },
        {  U"lemonchiffon"          ,	U"#FFFACD"   },
        {  U"lightgoldenrodyellow"  ,	U"#FAFAD2"   },
        {  U"lightyellow"           ,	U"#FFFFE0"   },
        {  U"saddlebrown"           ,	U"#8B4513"   },
        {  U"sienna"                ,	U"#A0522D"   },
        {  U"chocolate"             ,	U"#D2691E"   },
        {  U"peru"                  ,	U"#CD853F"   },
        {  U"sandybrown"            ,	U"#F4A460"   },
        {  U"burlywood"             ,	U"#DEB887"   },
        {  U"tan"                   ,	U"#D2B48C"   },
        {  U"rosybrown"             ,	U"#BC8F8F"   },
        {  U"moccasin"              ,	U"#FFE4B5"   },
        {  U"navajowhite"           ,	U"#FFDEAD"   },
        {  U"peachpuff"             ,	U"#FFDAB9"   },
        {  U"mistyrose"             ,	U"#FFE4E1"   },
        {  U"lavenderblush"         ,	U"#FFF0F5"   },
        {  U"linen"                 ,	U"#FAF0E6"   },
        {  U"oldlace"               ,	U"#FDF5E6"   },
        {  U"papayawhip"            ,	U"#FFEFD5"   },
        {  U"seashell"              ,	U"#FFF5EE"   },
        {  U"mintcream"             ,	U"#F5FFFA"   },
        {  U"slategray"             ,	U"#708090"   },
        {  U"lightslategray"        ,	U"#778899"   },
        {  U"lightsteelblue"        ,	U"#B0C4DE"   },
        {  U"lavender"              ,	U"#E6E6FA"   },
        {  U"floralwhite"           ,	U"#FFFAF0"   },
        {  U"aliceblue"             ,	U"#F0F8FF"   },
        {  U"ghostwhite"            ,	U"#F8F8FF"   },
        {  U"honeydew"              ,	U"#F0FFF0"   },
        {  U"ivory"                 ,	U"#FFFFF0"   },
        {  U"azure"                 ,	U"#F0FFFF"   },
        {  U"snow"                  ,	U"#FFFAFA"   },
        {  U"black"                 ,	U"#000000"   },
        {  U"dimgray"               ,	U"#696969"   },
        {  U"gray"                  ,	U"#808080"   },
        {  U"darkgray"              ,	U"#A9A9A9"   },
        {  U"silver"                ,	U"#C0C0C0"   },
        {  U"lightgray"             ,	U"#D3D3D3"   },
        {  U"gainsboro"             ,	U"#DCDCDC"   },
        {  U"whitesmoke"            ,	U"#F5F5F5"   },
        {  U"white"                 ,	U"#FFFFFF"   }
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlAttribute::Ptr XhtmlAttribute::createFromStr(std::vector<std::tuple<MessageType, tgui::String>>& messages, const tgui::String& buffer,
        const size_t beginPosition, size_t& processedLength)
    {
        processedLength = 0;

#ifdef LOG_ATTRIBUTE
        // std::wcout << U"  Begin parsing attribute. ";
#endif
        if (buffer.empty())
        {
            tgui::String message(U"XhtmlAttribute::createFromStr() -> Invalid buffer!");
            messages.push_back(std::make_tuple(MessageType::ERROR, message));
            return nullptr;
        }

        XhtmlAttribute::Ptr attribute = nullptr;
        size_t  workPosition = beginPosition;
        size_t  endPosition = beginPosition;
        char32_t workCharacter = 0;

        // skip leading white-space characters
        while (ext::isSpace(buffer[workPosition]))
            workPosition++;

        // name always begins with an alpha character - otherwise we have empty content or we are done with the attributes
        if (!ext::isAlpha(buffer[workPosition]))
            return nullptr;

        endPosition = workPosition;
        do
        {
            // inner-texts and attribute names may contain letters (a-z, A-Z),
            // digits (0-9), underscores '_', hyphen '-', colons ':', and periods '.'
            // otherwise we have to determine whether we have a valid end of the name
            // or an inner text, that does not establish an attribute
            if ((!ext::isAlnum(buffer[endPosition])) &&
                (buffer[endPosition] != U'-') &&
                (buffer[endPosition] != U':') &&
                (buffer[endPosition] != U'_') &&
                (buffer[endPosition] != U'.'))
            {
                if (endPosition == workPosition)
                {
                    tgui::String message(U"XhtmlAttribute::createFromStr() -> Attribute is of length 0, but shouldn't!");
                    messages.push_back(std::make_tuple(MessageType::ERROR, message));

                    // probably no attribute but an inner text
                    return nullptr;
                }

                // only white-space characters, a null-character, an
                // equal-sign, a greater-than symbol, or a forward-slash
                // can act as as a separator between an attribute and
                // its value or  an inner-text delimiter
                if (buffer[endPosition] == 0 ||
                    ext::isSpace(buffer[endPosition]) ||
                    buffer[endPosition] == U'=' ||
                    buffer[endPosition] == U'>' ||
                    buffer[endPosition] == U'/')
                {
                    break;
                }

                // probably no attribute but an inner text
                return nullptr;    // any other character will fail parsing process
            }

            endPosition++;
        } while (true);

        // success
        tgui::String attributeString = buffer.substr(workPosition, endPosition - workPosition);
        if (attributeString.equalIgnoreCase(XhtmlStyleEntry::TypeName))
            attribute = std::make_shared<XhtmlStyleEntry>();
        else
            attribute = std::make_shared<XhtmlAttribute>(attributeString);

        // process inner-text or attribute without value
        if (buffer[endPosition] != U'=')
        {
#ifdef LOG_ATTRIBUTE
            std::wcout << U"  Set name '" << attribute->getName() << U"' to attribute without value." << std::endl;
#endif
            processedLength = endPosition - beginPosition;
            return attribute;
        }
        // process key/value pair
        else
        {
            // skip white-space characters after equal-sign
            // and the equal-sign itself
            do
            {
                endPosition++;
            } while (ext::isSpace(buffer[endPosition]));

            workPosition = endPosition;
            workCharacter = buffer[endPosition];

            // is attribute value wrapped in quotes?
            if (workCharacter == U'\'' || workCharacter == U'\"')
            {
                workPosition++;    // skip quote symbol
                do
                {
                    endPosition++;
                }
                // Loop until we find the same quote character that
                // was used at the starting of the attribute value.
                // Anything within these quotes is considered valid!
                // NOTE that the entity references are resolved later.
                while (buffer[endPosition] != 0 && buffer[endPosition] != workCharacter);
            }
            // open attribute value i.e. not wrapped in quotes?
            else
            {
                do
                {
                    endPosition++;
                }
                // Loop until we find a tag ending delimiter or any
                // white-space character, or until we reach at the
                // end of the string buffer.
                while (buffer[endPosition] != 0 &&
                    !ext::isSpace(buffer[endPosition]) &&
                    buffer[endPosition] != U'/' &&
                    buffer[endPosition] != U'>');
            }

#ifdef LOG_ATTRIBUTE
            std::wcout << U"  Set name '" << attribute->getName() << U"' to attribute." << std::endl;
#endif
            if (endPosition != workPosition)
                // normalize data and resolve references
                attribute->putValue(messages, buffer.substr(workPosition, endPosition - workPosition));

#ifdef LOG_ATTRIBUTE
            std::wcout << U"  End parsing attribute successfully with value '" << attribute->getValue() << U"'." << std::endl;
#endif
            processedLength = ((endPosition - beginPosition) +
                (workCharacter == U'\'' || workCharacter == U'\"' ? 1 : 0));
            return attribute;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void XhtmlAttribute::putValue(std::vector<std::tuple<tgui::MessageType, String>>& messages, const tgui::String& rawValue)
    {
        if (rawValue.empty())
        {
            tgui::String message(U"XhtmlAttribute::putValue() -> Invalid rawValue!");
            messages.push_back(std::make_tuple(MessageType::ERROR, message));
            return;
        }

        m_value = rawValue;

        // clean the value
        m_value = m_value.trim();
        m_value.remove(U'\n');
        m_value.replace(U'\r', U' ');
        m_value.replace(U'\t', U' ');
        m_value.replace(U'\v', U' ');

        /** resolve entity reference(s) */
        size_t    workPosition = -1;
        int       substitutionLength = 0;
        char32_t  substituteCharacter = 0;
        do
        {
            if ((workPosition = m_value.find(U'&', ++workPosition)) == (size_t)-1)
                break;

            substitutionLength = XhtmlEntityResolver::resolveEntity(messages, (char32_t*)m_value.substr(workPosition).c_str(), substituteCharacter);
            if (substitutionLength)
            {
                tgui::String strSubst; strSubst += substituteCharacter;
                m_value.replace(m_value.substr(workPosition, substitutionLength), strSubst);
            }
        } while (true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const tgui::String XhtmlStyleEntry::TypeName = U"style";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlStyleEntry& XhtmlStyleEntry::operator=(XhtmlStyleEntry&& styleEntry) noexcept
    {
        if (this != &styleEntry)
        {
            m_color           = std::move(styleEntry.m_color);
            m_backgroundColor = std::move(styleEntry.m_backgroundColor);
            m_borderColor     = std::move(styleEntry.m_borderColor);
            m_opacity         = std::move(styleEntry.m_opacity);
            m_fontFamily      = std::move(styleEntry.m_fontFamily);
            m_fontSize        = std::move(styleEntry.m_fontSize);
            m_fontStyle       = std::move(styleEntry.m_fontStyle);
            m_margin          = std::move(styleEntry.m_margin);
            m_border          = std::move(styleEntry.m_border);
            m_padding         = std::move(styleEntry.m_padding);
            m_styleEntryFlags = std::move(styleEntry.m_styleEntryFlags);
        }
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlStyleEntry& XhtmlStyleEntry::operator=(const XhtmlStyleEntry& styleEntry)
    {
        if (this != &styleEntry)
        {
            XhtmlStyleEntry temp(styleEntry);

            std::swap(m_color,           temp.m_color);
            std::swap(m_backgroundColor, temp.m_backgroundColor);
            std::swap(m_borderColor,     temp.m_borderColor);
            std::swap(m_opacity,         temp.m_opacity),
            std::swap(m_fontFamily,      temp.m_fontFamily);
            std::swap(m_fontSize,        temp.m_fontSize);
            std::swap(m_fontStyle,       temp.m_fontStyle);
            std::swap(m_margin,          temp.m_margin),
            std::swap(m_border,          temp.m_border);
            std::swap(m_padding,         temp.m_padding);
            std::swap(m_styleEntryFlags, temp.m_styleEntryFlags);
        }
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void XhtmlStyleEntry::putValue(std::vector<std::tuple<tgui::MessageType, String>>& messages, const tgui::String& rawValue)
    {
        XhtmlAttribute::putValue(messages, rawValue);
        auto styleEntryValues = ext::String::split(m_value, U';', true);
        for (auto styleEntryValue : styleEntryValues)
        {
            styleEntryValue.remove(U';');
            styleEntryValue = styleEntryValue.trim();

            if (styleEntryValue.size() == 0)
                continue;
            if (styleEntryValue.size() <= 3)
            {
                tgui::String message(U"XhtmlAttribute::putValue() -> Unable to recognize style value from '" + styleEntryValue + U"'!");
                messages.push_back(std::make_tuple(MessageType::ERROR, message));
                continue;
            }

            auto styleEntryParts = ext::String::split(styleEntryValue, U':', true);
            if (styleEntryParts.size() != 2)
            {
                tgui::String message(U"XhtmlAttribute::putValue() -> Unable to split style value '" + styleEntryValue + U"' into key and value(s)!");
                messages.push_back(std::make_tuple(MessageType::ERROR, message));
                continue;
            }

            styleEntryParts[0] = styleEntryParts[0].trim();
            styleEntryParts[1] = styleEntryParts[1].trim();
            std::transform(styleEntryParts[0].begin(), styleEntryParts[0].end(), styleEntryParts[0].begin(), ::tolower);
            std::transform(styleEntryParts[1].begin(), styleEntryParts[1].end(), styleEntryParts[1].begin(), ::tolower);

            if (styleEntryParts[0].size() <= 1 || styleEntryParts[1].size() <= 1)
            {
                tgui::String message(U"XhtmlAttribute::putValue() -> Unable to recognize  key and value from style value '" + styleEntryValue + U"'!");
                messages.push_back(std::make_tuple(MessageType::ERROR, message));
                continue;
            }

            if      (styleEntryParts[0].equalIgnoreCase(U"opacity"))
            {
                auto opacity = styleEntryParts[1].toFloat();
                setOpacity(opacity);
                continue;
            }
            else if (styleEntryParts[0].equalIgnoreCase(U"background") ||
                     styleEntryParts[0].equalIgnoreCase(U"background-color"))
            {
                try
                {
                    auto colorValue = WebColorMap.find(styleEntryParts[1]) != WebColorMap.end() ? WebColorMap[styleEntryParts[1]] : styleEntryParts[1];
                    if (colorValue[0] != U'#')
                        colorValue = U"#" + colorValue;
                    Color color(colorValue);
                    setBackgroundColor(color);
                }
                catch(const Exception&)
                {
                    tgui::String message(U"XhtmlAttribute::putValue() -> Unable to recognize  value from style value '" + styleEntryValue + U"'!");
                    messages.push_back(std::make_tuple(MessageType::ERROR, message));
                }
                continue;
            }
            else if (styleEntryParts[0].equalIgnoreCase(U"color"))
            {
                try
                {
                    auto colorValue = WebColorMap.find(styleEntryParts[1]) != WebColorMap.end() ? WebColorMap[styleEntryParts[1]] : styleEntryParts[1];
                    if (colorValue[0] != U'#')
                        colorValue = U"#" + colorValue;
                    Color color(colorValue);
                    setColor(color);
                }
                catch(const Exception&)
                {
                    tgui::String message(U"XhtmlAttribute::putValue() -> Unable to recognize  value from style value '" + styleEntryValue + U"'!");
                    messages.push_back(std::make_tuple(MessageType::ERROR, message));
                }
                continue;
            }
            else if (styleEntryParts[0].equalIgnoreCase(U"border-color"))
            {
                try
                {
                    auto colorValue = WebColorMap.find(styleEntryParts[1]) != WebColorMap.end() ? WebColorMap[styleEntryParts[1]] : styleEntryParts[1];
                    if (colorValue[0] != U'#')
                        colorValue = U"#" + colorValue;
                    Color color(colorValue);
                    setBorderColor(color);
                }
                catch(const Exception&)
                {
                    tgui::String message(U"XhtmlAttribute::putValue() -> Unable to recognize  value from style value '" + styleEntryValue + U"'!");
                    messages.push_back(std::make_tuple(MessageType::ERROR, message));
                }
                continue;
            }
            else if (styleEntryParts[0].equalIgnoreCase(U"font-style"))
            {
                if (styleEntryParts[1].find(U"italic") != SIZE_MAX || styleEntryParts[1].find(U"oblique") != SIZE_MAX)
                    setItalic(true);
                else
                {
                    tgui::String message(U"XhtmlAttribute::putValue() -> Unable to recognize  value from style value '" + styleEntryValue + U"'!");
                    messages.push_back(std::make_tuple(MessageType::ERROR, message));
                }
                continue;
            }
            else if (styleEntryParts[0].equalIgnoreCase(U"font-weight"))
            {
                if (styleEntryParts[1].find(U"bold") != SIZE_MAX)
                    setBold(true);
                else
                {
                    tgui::String message(U"XhtmlAttribute::putValue() -> Unable to recognize  value from style value '" + styleEntryValue + U"'!");
                    messages.push_back(std::make_tuple(MessageType::ERROR, message));
                }
                continue;
            }
            else if (styleEntryParts[0].equalIgnoreCase(U"border-width"))
            {
                styleEntryParts[1].replace(U"thin", U"1px");
                styleEntryParts[1].replace(U"medium", U"3px");
                styleEntryParts[1].replace(U"thick", U"5px");
                FourDimSize borderWidth = getBorderWidth();
                borderWidth.parse(ext::String::split(styleEntryParts[1], U' '));
                setBorderWidth(borderWidth);
                continue;
            }
            else if (XhtmlStyleEntry::containsAnyIgnoreCase(styleEntryParts[0],
                        {U"margin", U"margin-top", U"margin-left", U"margin-bottom", U"margin-right"}))
            {
                FourDimSize margin = getMargin();
                if (styleEntryParts[0].equalIgnoreCase(U"margin"))
                    margin.parse(ext::String::split(styleEntryParts[1], U' ', true));
                else
                {
                    if (styleEntryParts[0].equalIgnoreCase(U"margin-top"))
                        margin.top = styleEntryParts[1].toFloat();
                    else if (styleEntryParts[0].equalIgnoreCase(U"margin-left"))
                        margin.left = styleEntryParts[1].toFloat();
                    else if (styleEntryParts[0].equalIgnoreCase(U"margin-bottom"))
                        margin.bottom = styleEntryParts[1].toFloat();
                    else if (styleEntryParts[0].equalIgnoreCase(U"margin-right"))
                        margin.right = styleEntryParts[1].toFloat();

                    margin.sizeType = FourDimSize::determineSizeType(styleEntryParts[1]);
                }
                setMargin(margin);
                continue;
            }
            else if (XhtmlStyleEntry::containsAnyIgnoreCase(styleEntryParts[0],
                        {U"padding", U"padding-top", U"padding-left", U"padding-bottom", U"padding-right"}))
            {
                FourDimSize padding = getPadding();
                if (styleEntryParts[0].equalIgnoreCase(U"padding"))
                    padding.parse(ext::String::split(styleEntryParts[1], U' ', true));
                else
                {
                    if (styleEntryParts[0].equalIgnoreCase(U"padding-top") == 0)
                        padding.top = styleEntryParts[1].toFloat();
                    else if (styleEntryParts[0].equalIgnoreCase(U"padding-left") == 0)
                        padding.left = styleEntryParts[1].toFloat();
                    else if (styleEntryParts[0].equalIgnoreCase(U"padding-bottom") == 0)
                        padding.bottom = styleEntryParts[1].toFloat();
                    else if (styleEntryParts[0].equalIgnoreCase(U"padding-right") == 0)
                        padding.right = styleEntryParts[1].toFloat();

                    padding.sizeType = FourDimSize::determineSizeType(styleEntryParts[1]);
                }
                setPadding(padding);
                continue;
            }
            tgui::String message(U"XhtmlAttribute::putValue() -> Unable to recognize  value from style value '" + styleEntryValue + U"'!");
            messages.push_back(std::make_tuple(MessageType::ERROR, message));
            continue;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool XhtmlStyleEntry::containsAnyIgnoreCase(const tgui::String& nocaseLeft, const std::vector<tgui::String>& patterns)
    {
        auto left = nocaseLeft.toLower();
        for (auto pattern : patterns)
            if (left.contains(pattern) == 0)
                return true;

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
