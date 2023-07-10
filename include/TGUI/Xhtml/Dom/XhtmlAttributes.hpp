/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// XHTML attribute (zlib license) provided for TGUI - Texus' Graphical User Interface
// Copyright (C) 2023 Steffen Ploetz (Steffen.Ploetz@cityweb.de)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef TGUI_XHTML_ATTRIBUTES_HPP
#define TGUI_XHTML_ATTRIBUTES_HPP

#include <cwctype>
#include <string>
#include <vector>

#include "TGUI/Xhtml/MarkupSizeType.hpp"
#include "TGUI/Xhtml/MarkupBorderStyle.hpp"
#include "TGUI/Xhtml/Dom/XhtmlEntityResolver.hpp"

namespace tgui  { namespace xhtml
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Generic XHTML attribute
    ///
    /// This class holds a key / value pair
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlAttribute
    {
    public:
        using Ptr = std::shared_ptr<XhtmlAttribute>;                 //!< Shared Attribute pointer
        using ConstPtr = std::shared_ptr<const XhtmlAttribute>;      //!< Shared constant Attribute pointer

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlAttribute()
            : m_name(), m_value()
        {   ;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor
        ///
        /// @param name   The attribute name to initialize
        /// @param value  The attribute value to initialize
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlAttribute(const tgui::String& name, const tgui::String& value = U"")
            : m_name(name), m_value(value)
        {   ;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Initializing constructor
        ///
        /// @param attribute    The attribute to initialize from
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlAttribute(const XhtmlAttribute& attribute)
            : m_name(attribute.m_name), m_value(attribute.m_value)
        {   ;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The virtual default destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~XhtmlAttribute()
        {   ;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates an XHTML attribute from raw string (on success)
        ///
        /// @param messages         The collection of error/warning messages, created during the parser run
        /// @param buffer           The raw string to parse for the attribute
        /// @param beginPosition    The position within the raw string to start to parse for the attribute
        /// @param processedLength  The number of characters that have been parsed from the buffer
        ///
        /// @return The attribute on success, or nullptr otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static XhtmlAttribute::Ptr createFromStr(std::vector<std::tuple<MessageType, String>>& messages, const tgui::String& buffer,
            const size_t beginPosition, size_t& processedLength);

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Get the attribute name of this attribute
        ///
        /// @return The attribute name of this attribute
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline const tgui::String& getName() const
        {   return m_name;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Get the attribute value of this attribute
        ///
        /// @return The attribute value of this attribute
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline const tgui::String& getValue() const
        {   return m_value;   }

    protected:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets and normalizes the value and resolves contained entities (if any)
        ///
        /// @param messages  The collection of error messages, created during the parser run
        /// @param rawValue  The raw string of the value to set
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void putValue(std::vector<std::tuple<MessageType, tgui::String>>& messages, const tgui::String& rawValue);

    protected:
        tgui::String m_name;   //!< This attribute's name
        tgui::String m_value;  //!< This attribute's value
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wshadow"
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The enumeration of style entry properties, that are valid for a style entry
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    enum class StyleEntryFlags : unsigned int
    {
        None        = 1 << 0,  //!< This is an empty style entry
        ForeColor   = 1 << 1,  //!< The fore color is captured by this style entry
        BackColor   = 1 << 2,  //!< The back color is captured by this style entry
        BorderColor = 1 << 3,  //!< The border color is captured by this style entry
        Opacity     = 1 << 4,  //!< The opacity is captured by this style entry
        FontFamily  = 1 << 5,  //!< The font family name is captured by this style entry
        FontSize    = 1 << 6,  //!< The font size is captured by this style entry
        FontStyle   = 1 << 7,  //!< The font style (slant, weight, underline, strike-through) is captured by this style entry
        Margin      = 1 << 8,  //!< The margin is captured by this style entry
        BorderStyle = 1 << 9,  //!< The border style is captured by this style entry
        BorderWidth = 1 << 10, //!< The border width is captured by this style entry
        Padding     = 1 << 11, //!< The padding is captured by this style entry
        Height      = 1 << 12, //!< The element height is captured by this style entry
        Width       = 1 << 13  //!< The element width is captured by this style entry
    };

#if defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Combines two style flags with binary OR operator
    ///
    /// @param left  The first style flag to combine with binary OR operator
    /// @param right The second style flag to combine with binary OR operator
    ///
    /// @return The combination of the two indicated style flags
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    inline StyleEntryFlags operator|(StyleEntryFlags left, StyleEntryFlags right)
    {   return static_cast<StyleEntryFlags>(static_cast<int>(left) | static_cast<int>(right));   }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Combines two style flags with binary AND operator
    ///
    /// @param left  The first style flag to combine with binary AND operator
    /// @param right The second style flag to combine with binary AND operator
    ///
    /// @return The combination of the two indicated style flags
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    inline StyleEntryFlags operator&(StyleEntryFlags left, StyleEntryFlags right)
    {   return static_cast<StyleEntryFlags>(static_cast<int>(left) & static_cast<int>(right));   }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The initializer structure for a XHTML style entry, designed to benefit from designated initializers
    ///
    /// To benefit from designated initializers we need GCC, or Clang or MSVC with -std:c++latest (which is at least C++20)
    /// Read: https://pdimov.github.io/blog/2020/09/07/named-parameters-in-c20/ for details
    ///
    /// A generic solution for C++ is to use inititalizers, that return a reference to the this pointer. Like
    /// @example auto style = std::make_shared<XhtmlStyleEntry>(XhtmlStyleEntryInitializer().SetForeColor(tgui::Color::Blue));
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct XhtmlStyleEntryInitializer
    {
        Color        ForeColor  = Color::Transparent;                     //!< The foreground/text color
        tgui::String FontFamily = U"";                                    //!< The font family
        OneDimSize   FontSize   = OneDimSize(SizeType::Relative, 1.0F);   //!< The font size
        TextStyle    FontStyle  = static_cast<TextStyle>(1 << 4);         //!< The font style
        Color        BackColor  = Color::Transparent;                     //!< The background color

        inline XhtmlStyleEntryInitializer& SetForeColor(Color color)              { ForeColor = color; return *this; }
        inline XhtmlStyleEntryInitializer& SetFontFamily(tgui::String fontFamily) { FontFamily = fontFamily; return *this; }
        inline XhtmlStyleEntryInitializer& SetFontSize(OneDimSize fontSize)       { FontSize = fontSize; return *this; }
        inline XhtmlStyleEntryInitializer& SetFontStyle(TextStyle fontStyle)      { FontStyle = fontStyle; return *this; }
        inline XhtmlStyleEntryInitializer& SetBackColor(Color color)              { BackColor = color; return *this; }
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The XHTML Style entry
    ///
    /// This class represents a THML **style="..."** entry
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API XhtmlStyleEntry : public XhtmlAttribute
    {
    public:
        using Ptr = std::shared_ptr<XhtmlStyleEntry>; //!< Shared style entry pointer
        using ConstPtr = std::shared_ptr<const XhtmlStyleEntry>; //!< Shared constant style entry pointer

        static const tgui::String TypeName;   //!< The tag name of an style entry

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleEntry()
            : XhtmlAttribute(XhtmlStyleEntry::TypeName), m_color(Color(0, 0, 0)), m_backgroundColor(Color::Transparent), m_borderColor(Color(0, 0, 0)),
              m_opacity(1.0f), m_fontFamily(U"Sans-serif"), m_fontSize({SizeType::Relative, 1.0f}), m_fontStyle(TextStyle::Regular),
              m_margin({SizeType::Relative, 0.0f}), m_borderStyle(FourDimBorderStyle()), m_borderWidth({SizeType::Relative, 0.0f}),
              m_padding({SizeType::Relative, 0.0f}), m_height({SizeType::ViewportWidth, 0.0f}), m_width({SizeType::ViewportWidth, 0.0f}),
              m_styleEntryFlags(StyleEntryFlags::None)
        {   ;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor with foreground color
        ///
        /// @param color  The new foreground color
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleEntry(Color color)
            : XhtmlAttribute(XhtmlStyleEntry::TypeName), m_color(color), m_backgroundColor(Color::Transparent), m_borderColor(Color(0, 0, 0)),
            m_opacity(1.0f), m_fontFamily(U"Sans-serif"), m_fontSize({ SizeType::Relative, 1.0f }), m_fontStyle(TextStyle::Regular),
            m_margin({ SizeType::Relative, 0.0f }), m_borderStyle(FourDimBorderStyle()), m_borderWidth({ SizeType::Relative, 0.0f }),
            m_padding({ SizeType::Relative, 0.0f }), m_height({SizeType::ViewportWidth, 0.0f}), m_width({SizeType::ViewportWidth, 0.0f}),
            m_styleEntryFlags(StyleEntryFlags::ForeColor)
        {
            ;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The initializing constructor with foreground color and font family
        ///
        /// @param color   The new foreground color
        /// @param family  The new family
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleEntry(const XhtmlStyleEntryInitializer& initializer)
            : XhtmlAttribute(XhtmlStyleEntry::TypeName), m_color(Color(0, 0, 0)), m_backgroundColor(Color::Transparent), m_borderColor(Color(0, 0, 0)),
              m_opacity(1.0f), m_fontFamily(U"Sans-serif"), m_fontSize({SizeType::Relative, 1.0f}), m_fontStyle(TextStyle::Regular),
              m_margin({SizeType::Relative, 0.0f}), m_borderStyle(FourDimBorderStyle()), m_borderWidth({ SizeType::Relative, 0.0f }),
              m_padding({SizeType::Relative, 0.0f}), m_height({SizeType::ViewportWidth, 0.0f}), m_width({SizeType::ViewportWidth, 0.0f}),
              m_styleEntryFlags(StyleEntryFlags::None)
        {
            if (initializer.ForeColor != Color::Transparent)
            {
                m_color = initializer.ForeColor;
                m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::ForeColor;
            }
            if (initializer.FontFamily.length() != 0)
            {
                m_fontFamily = initializer.FontFamily;
                m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::FontFamily;
            }
            if (initializer.FontSize.sizeType != SizeType::Relative && initializer.FontSize.value != 1.0F)
            {
                m_fontSize = initializer.FontSize;
                m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::FontSize;
            }
            if (static_cast<int>(initializer.FontStyle) <= static_cast<int>(TextStyle::StrikeThrough))
            {
                m_fontStyle = initializer.FontStyle;
                m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::FontStyle;
            }
            if (initializer.BackColor != Color::Transparent)
            {
                m_backgroundColor = initializer.BackColor;
                m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::BackColor;
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The copy constructor
        ///
        /// @param styleEntry  The style entry entry to apply
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleEntry(const XhtmlStyleEntry& styleEntry)
            : XhtmlAttribute(XhtmlStyleEntry::TypeName), m_color(styleEntry.m_color), m_backgroundColor(styleEntry.m_backgroundColor),
              m_borderColor(styleEntry.m_borderColor), m_opacity(styleEntry.m_opacity),
              m_fontFamily(styleEntry.m_fontFamily), m_fontSize(styleEntry.m_fontSize),
              m_fontStyle(styleEntry.m_fontStyle),
              m_margin(styleEntry.m_margin), m_borderStyle(styleEntry.m_borderStyle), m_borderWidth(styleEntry.m_borderWidth),
              m_padding(styleEntry.m_padding), m_height(styleEntry.m_height), m_width(styleEntry.m_width),
              m_styleEntryFlags(styleEntry.m_styleEntryFlags)
        {   ;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The move constructor
        ///
        /// @param styleEntry  The style entry to apply
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleEntry(XhtmlStyleEntry&& styleEntry) noexcept
            : XhtmlAttribute(XhtmlStyleEntry::TypeName), m_color(std::move(styleEntry.m_color)), m_backgroundColor(std::move(styleEntry.m_backgroundColor)),
              m_borderColor(std::move(styleEntry.m_borderColor)), m_opacity(std::move(styleEntry.m_opacity)),
              m_fontFamily(std::move(styleEntry.m_fontFamily)), m_fontSize(std::move(styleEntry.m_fontSize)),
              m_fontStyle(std::move(styleEntry.m_fontStyle)),
              m_margin(std::move(styleEntry.m_margin)), m_borderStyle(std::move(styleEntry.m_borderStyle)), m_borderWidth(std::move(styleEntry.m_borderWidth)),
              m_padding(std::move(styleEntry.m_padding)), m_height(styleEntry.m_height), m_width(styleEntry.m_width),
              m_styleEntryFlags(std::move(styleEntry.m_styleEntryFlags))
        {   ;   }

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The virtual destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~XhtmlStyleEntry()
        {   ;   }


    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Overload of copy assignment operator
        ///
        /// @param styleEntry  The style entry to apply
        ///
        /// @return            A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleEntry& operator=(const XhtmlStyleEntry& styleEntry);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Move assignment operator
        ///
        /// @param styleEntry  The style entry to apply
        ///
        /// @return            A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleEntry& operator=(XhtmlStyleEntry&& styleEntry) noexcept;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Merge/override the style values of this stryle entry with the style values of the indicated style enty
        ///
        /// @param styleEntry  The style entry to merge/override with this style entry
        ///
        /// @return            A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyleEntry& mergeWith(const XhtmlStyleEntry::Ptr styleEntry);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new color
        ///
        /// @param color The new color
        ///
        /// @return      A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry& setColor(const Color& color)
        {   m_color = color; m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::ForeColor; return *this;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the color
        ///
        /// @return The color
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline Color getColor() const
        {   return m_color;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new background color
        ///
        /// @param backgroundColor  The new background color
        ///
        /// @return                 A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry& setBackgroundColor(const Color& backgroundColor)
        {   m_backgroundColor = backgroundColor; m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::BackColor; return *this;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the background color
        ///
        /// @return The background color
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline Color getBackgroundColor() const
        {   return m_backgroundColor;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new border color
        ///
        /// @param borderColor  The new border color
        ///
        /// @return             A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry& setBorderColor(const Color& borderColor)
        {   m_borderColor = borderColor; m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::BorderColor; return *this;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the border color
        ///
        /// @return The border color
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline Color getBorderColor() const
        {   return m_borderColor;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new opacity
        ///
        /// @param opacity  The new opacity
        ///
        /// @return         A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry& setOpacity(float opacity)
        {   m_opacity = opacity; m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::Opacity; return *this;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the opacity
        ///
        /// @return The background opacity
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline float getOpacity() const
        {   return m_opacity;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new color
        ///
        /// @param color  The new color
        ///
        /// @return       A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry& setFontFamily(const tgui::String& fontFamily)
        {   m_fontFamily = fontFamily; m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::FontFamily; return *this;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the color
        ///
        /// @return The color
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline tgui::String getFontFamily() const
        {   return m_fontFamily;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new font size
        ///
        /// @param fontSize  The new font size
        ///
        /// @return          A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry& setFontSize(OneDimSize fontSize)
        {   m_fontSize = fontSize; m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::FontSize; return *this;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the font size
        ///
        /// @return The font size
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline OneDimSize getFontSize() const
        {   return m_fontSize; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new font is bold flag
        ///
        /// @param bold  The new font is bold flag
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setBold(bool bold)
        {
            if (bold) m_fontStyle = static_cast<TextStyle>(m_fontStyle | TextStyle::Bold);
            else m_fontStyle      = static_cast<TextStyle>(m_fontStyle & (~TextStyle::Bold));
            m_styleEntryFlags     = m_styleEntryFlags | StyleEntryFlags::FontStyle;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the font is bold flag
        ///
        /// @return The font is bold flag
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline bool getBold() const
        {   return (m_fontStyle == (m_fontStyle | TextStyle::Bold)); }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new font is italic flag
        ///
        /// @param italic  The new font is italic flag
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setItalic(bool italic)
        {
            if (italic) m_fontStyle = static_cast<TextStyle>(m_fontStyle | TextStyle::Italic);
            else m_fontStyle        = static_cast<TextStyle>(m_fontStyle & (~TextStyle::Italic));
            m_styleEntryFlags       = m_styleEntryFlags | StyleEntryFlags::FontStyle;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the font is italic flag
        ///
        /// @return The font is italic flag
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline bool getItalic() const
        {   return (m_fontStyle == (m_fontStyle | TextStyle::Italic)); }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new margin
        ///
        /// @param margin  The new margin
        ///
        /// @return        A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry& setMargin(FourDimSize margin)
        {   m_margin = margin; m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::Margin; return *this;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the margin
        ///
        /// @return The margin
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline FourDimSize getMargin() const
        {   return m_margin;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new border style
        ///
        /// @param borderWidth  The new border style
        ///
        /// @return             A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry& setBorderStyle(FourDimBorderStyle borderStyle)
        {   m_borderStyle = borderStyle; m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::BorderStyle; return *this;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the border style
        ///
        /// @return The border style
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline FourDimBorderStyle getBorderStyle() const
        {   return m_borderStyle;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new border width
        ///
        /// @param borderWidth  The new border width
        ///
        /// @return             A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry& setBorderWidth(FourDimSize borderWidth)
        {   m_borderWidth = borderWidth; m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::BorderWidth; return *this;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the border width
        ///
        /// @return The border width
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline FourDimSize getBorderWidth() const
        {   return m_borderWidth;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new padding
        ///
        /// @param padding  The new padding
        ///
        /// @return         A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry& setPadding(FourDimSize padding)
        {   m_padding = padding; m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::Padding; return *this;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the padding
        ///
        /// @return The padding
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline FourDimSize getPadding() const
        {   return m_padding;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new height
        ///
        /// @param height   The new height
        ///
        /// @return         A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry& setHeight(OneDimSize height)
        {   m_height = height; m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::Height; return *this;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the height
        ///
        /// @return The height
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline OneDimSize getHeight() const
        {   return m_height;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new width
        ///
        /// @param width    The new width
        ///
        /// @return         A reference to this style entry
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlStyleEntry& setWidth(OneDimSize width)
        {   m_width = width; m_styleEntryFlags = m_styleEntryFlags | StyleEntryFlags::Width; return *this;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the width
        ///
        /// @return The width
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline OneDimSize getWidth() const
        {   return m_width;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the new style entry flags
        ///
        /// @param styleEntryFlags  The new style entry flags
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setStyleEntryFlags(StyleEntryFlags styleEntryFlags)
        {   m_styleEntryFlags = styleEntryFlags;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the style entry flags
        ///
        /// @return The style entry flags
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline StyleEntryFlags getStyleEntryFlags() const
        {   return m_styleEntryFlags;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets and normalizes the value and resolves contained entities (if any)
        ///
        /// @param messages  The collection of error messages, created during the parser run
        /// @param rawValue  The raw string of the value to set
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void putValue(std::vector<std::tuple<tgui::xhtml::MessageType, String>>& messages, const tgui::String& rawValue) override;

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether indicated string contains case insensitive one of the predicates
        ///
        /// @param left      The string to be compared with the predicates
        /// @param patterns  The strings to check containment
        ///
        /// @return          The flag indicating whether one of the predicates is contained (true) or not (false)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool containsAnyIgnoreCase(const tgui::String& nocaseLeft, const std::vector<tgui::String>& patterns);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Tries to parse the color
        ///
        /// @param colorString The string that should contain information about the color
        /// @param colorValue  [OUT] The color on success
        ///
        /// @return            The flag indicating whether parse succeded (true) or not (false)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool tryParseColor(const tgui::String& colorString, Color& colorValue);

    private:
        Color              m_color;             //!< The foreground/text color
        Color              m_backgroundColor;   //!< The background color
        Color              m_borderColor;       //!< The border color
        float              m_opacity;           //!< The color opacity
        tgui::String       m_fontFamily;        //!< The font family name
        OneDimSize         m_fontSize;          //!< The font size (in pixels)
        TextStyle          m_fontStyle;         //!< The font style (slant, weight, underline, strike-through)
        FourDimSize        m_margin;            //!< The margin width (around the element and around it's border, if any)
        FourDimBorderStyle m_borderStyle;       //!< The border style (around the element)
        FourDimSize        m_borderWidth;       //!< The border width (around the element)
        FourDimSize        m_padding;           //!< The padding width (inside the element and inside it's border, if any)
        OneDimSize         m_height;            //!< The element height
        OneDimSize         m_width;             //!< The element width
        StyleEntryFlags    m_styleEntryFlags;   //!< The flags of style entry properties, that are actively set for this style entry
    };

} }

#endif // TGUI_XHTML_ATTRIBUTES_HPP
