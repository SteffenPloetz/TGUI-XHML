/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Formatted text XHTML document (zlib license) provided for TGUI - Texus' Graphical User Interface
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


#ifndef TGUI_FORMATTED_TEXT_XHTML_DOCUMENT_HPP
#define TGUI_FORMATTED_TEXT_XHTML_DOCUMENT_HPP

#include "FormattedTextDocument.hpp"

namespace tgui
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The enumeration of style category flags, used to filter the style properties to apply
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    enum class StyleCategoryFlags
    {
        ForeColor        =  1,  //!< Apply the fore color property
        BackColor        =  2,  //!< Apply the back color property
        Opacity          =  4,  //!< Apply the opacity property

        ColorsAndOpacity =  7,  //!< Apply the color and opacity properties

        Fonts            =  8,  //!< Apply the font properties

        BorderWidth      = 16,  //!< Apply the border width property properties
        BorderColor      = 32   //!< Apply the border color property properties
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Combines two style category flags with binary OR operator
    ///
    /// @param left  The first style category flag to combine with binary OR operator
    /// @param right The second style category flag to combine with binary OR operator
    ///
    /// @return The combination of the two indicated style flags
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    inline StyleCategoryFlags operator|(StyleCategoryFlags left, StyleCategoryFlags right)
    {
        return static_cast<StyleCategoryFlags>(static_cast<int>(left) | static_cast<int>(right));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Combines two style category flags with binary AND operator
    ///
    /// @param left  The first style category flag to combine with binary AND operator
    /// @param right The second style category flag to combine with binary AND operator
    ///
    /// @return The combination of the two indicated style flags
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    inline StyleCategoryFlags operator&(StyleCategoryFlags left, StyleCategoryFlags right)
    {
        return static_cast<StyleCategoryFlags>(static_cast<int>(left) & static_cast<int>(right));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The formatted text XHTML document
    ///
    /// A (specialized) XHTML formatted text data holder with XHTML element support.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API FormattedTextXhtmlDocument : public FormattedTextDocument
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        using Ptr = std::shared_ptr<FormattedTextXhtmlDocument>;            //!< Shared formatted text XHTML document pointer
        using ConstPtr = std::shared_ptr<const FormattedTextXhtmlDocument>; //!< Shared constant formatted text XHTML document pointer


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the right coordinate of the indicated rectangle
        ///
        /// @param rect  The rectangle to calculate the right coordinate for
        ///
        /// @return The right coordinate of the indicated rectangle
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline float right(const FloatRect& rect) const
        { return rect.left + rect.width; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the bottom coordinate of the indicated rectangle
        ///
        /// @param rect  The rectangle to calculate the bottom coordinate for
        ///
        /// @return The bottom coordinate of the indicated rectangle
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline float bottom(const FloatRect& rect) const
        { return rect.top + rect.height; } //!< Convenient access to rectangle's bottom

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inflates the indicated rectangle
        ///
        /// @param rect   The rectangle to inflate
        /// @param value  The inflate value to apply at left, top, right and bottom
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void inflate(FloatRect& rect, float value)
        { rect.top -= value; rect.left -= value; rect.height += value + value; rect.width += value + value; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inflates the indicated rectangle
        ///
        /// @param rect        The rectangle to inflate
        /// @param horizontal  The inflate value to apply at left and right
        /// @param vertical    The inflate value to apply at top and bottom
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void inflate(FloatRect& rect, float horizontal, float vertical)
        { rect.top -= vertical; rect.left -= horizontal; rect.height += vertical + vertical; rect.width += horizontal + horizontal; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inflates the indicated rectangle
        ///
        /// @param rect   The rectangle to inflate
        /// @param left   The inflate value to apply at left
        /// @param top    The inflate value to apply at top
        /// @param right  The inflate value to apply at right
        /// @param bottom The inflate value to apply at bottom
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void inflate(FloatRect& rect, float left, float top, float right, float bottom)
        { rect.top -= top; rect.left -= left; rect.height += top + bottom; rect.width += left + right; }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FormattedTextXhtmlDocument();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The virtual default destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~FormattedTextXhtmlDocument()
        { ; }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new formatted text XHTML document
        ///
        /// Prefer the factory method in cases where the formatted text section needs to be shared, otherwise prefer the constructor.
        ///
        /// @return The new formatted text XHTML document
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static inline FormattedTextXhtmlDocument::Ptr create() { return std::make_shared<FormattedTextXhtmlDocument>(); }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the formatted text content of this document
        ///
        /// @return The formatted text content of this document
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline const std::vector<std::shared_ptr<FormattedElement>>& getContent() const
        {   return m_content;   }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the root element of the XHTML element tree
        ///
        /// @return The root element of the XHTML element tree
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setRootElement(XhtmlContainerElement::Ptr rootElement)
        {   m_rootElement = rootElement;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the root element of the registered XHTML element tree
        ///
        /// @return The root element of the registered XHTML element tree
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline XhtmlContainerElement::Ptr getRootElement() const
        {   return m_rootElement;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the first head element of the registered XHTML element tree
        ///
        /// @return The first head element of the registered XHTML element tree. Can be nullptr
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlContainerElement::Ptr getHeadElement() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the first body element of the registered XHTML element tree
        ///
        /// @return The first body element of the registered XHTML element tree. Can be nullptr
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlContainerElement::Ptr getBodyElement() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the first head element' style element child of the registered XHTML element tree
        ///
        /// @return The first head element' style element child of the registered XHTML element tree. Can be nullptr
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        XhtmlStyle::Ptr getStyleElement() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the collection of applicable style elements, global ones prior local
        ///
        /// @param xhtmlElement  The XHTML element to get local style entry from
        ///
        /// @return The collection of applicable style elements, global ones prior local. Can be empty collection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<XhtmlStyleEntry::Ptr> getApplicableStyleElements(XhtmlElement::Ptr xhtmlElement);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Applies a list of XHTML style entries to the formatting state
        ///
        /// @param styleEntries                             The XHTML style entries to apply
        /// @param styleCategories                          The style categories to apply
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void applyStyleEntriesToFormattingState(const std::vector<tgui::XhtmlStyleEntry::Ptr> styleEntries,
            const FormattedTextDocument::FontCollection& fontCollection,
            StyleCategoryFlags categories = StyleCategoryFlags::ColorsAndOpacity | StyleCategoryFlags::Fonts);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Applies a list of XHTML style entries to the formatted element
        ///
        /// @param formattedElement                         The formatted element to apply the style entries to
        /// @param styleEntries                             The XHTML style entries to apply
        /// @param styleCategories                          The style categories to apply
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void applyStyleEntriesToFormattedElement(FormattedElement::Ptr formattedElement, const std::vector<tgui::XhtmlStyleEntry::Ptr> styleEntries,
            const FormattedTextDocument::FontCollection& fontCollection,
            StyleCategoryFlags categories = StyleCategoryFlags::ColorsAndOpacity | StyleCategoryFlags::Fonts);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates and basically initializes a FormattedRectangle
        ///
        /// @param xhtmlElement                             The XHTML element, represented by this formatted text section
        /// @param applyLineRunLength                       Determine wheter to start at current line run length (true) or very left
        ///
        /// @return The newly created FormattedRectangle
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FormattedRectangle::Ptr createFormattedRectangleWithPosition(XhtmlElement::Ptr xhtmlElement, bool applyLineRunLength = false);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates and basically initializes a FormattedLink
        ///
        /// @param xhtmlElement                             The XHTML element, represented by this formatted text section
        /// @param applyLineRunLength                       Determine wheter to start at current line run length (true) or very left
        ///
        /// @return The newly created FormattedLink
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FormattedLink::Ptr createFormattedLinkWithPosition(XhtmlElement::Ptr xhtmlElement, bool applyLineRunLength = false);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates and basically initializes a FormattedImage
        ///
        /// @param xhtmlElement                             The XHTML element, represented by this formatted text section
        ///
        /// @return The newly created FormattedImage
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FormattedImage::Ptr createFormattedImageWithPosition(XhtmlElement::Ptr xhtmlElement);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates and basically initializes a FormattedTextSection
        ///
        /// @param xhtmlElement                             The XHTML element, represented by this formatted text section
        /// @param font                                     The font to apply
        /// @param indentOffset                             The horizontal offset (indent) to apply to the current layout position
        /// @param superscriptOrSubsciptTextHeightReduction The text height reduction applied for superscript / subscript
        ///
        /// @return The newly created FormattedTextSection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FormattedTextSection::Ptr createFormattedTextSectionWithFontAndPosition(XhtmlElement::Ptr xhtmlElement, Font font,
            float indentOffset = 0.0f, float superscriptOrSubsciptTextHeightReduction = 0.0f);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Rearrange the complete visible content
        ///
        /// @param clientSize       The available size of the area to render to
        /// @param defaultTextSize  The default text size
        /// @param defaultForeColor The default foreground/text color
        /// @param defaultOpacity   The default foreground/text and background opacity
        /// @param fontCollection   The collection of supported fonts
        /// @param keepSelection    Determine whether to keep the selection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void layout(Vector2f clientSize, float defaultTextSize, Color defaultForeColor, float defaultOpacity,
                    const FormattedTextDocument::FontCollection& fontCollection, bool keepSelection);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the layout size
        ///
        /// @return The layout size
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline Vector2f getOccupiedLayoutSize() const
        { return Vector2f(m_occupiedLayoutSize.x, m_occupiedLayoutSize.y); }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Rearrange the visible content, defined by the indicated XHTML Element
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void layout(bool& predecessorWasBlockNode, bool parentIsTextBlockElement, bool& PedecessorWasTextBlockElement,
                    XhtmlElement::Ptr xhtmlElement, const FormattedTextDocument::FontCollection& fontCollection, bool keepSelection);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        std::map<size_t, Texture>                 m_textures;                 //!< The collection of already created textures
        std::vector<FormattedElement::Ptr>        m_content;                  //!< The collection of formatted content elements
        XhtmlContainerElement::Ptr                m_rootElement;              //!< The root element of the raw data elements

        float                                     m_defaultTextSize;          //!< The default text size (for HTML typically 16)
        Color                                     m_defaultForeColor;         //!< The default fore color
        float                                     m_defaultOpacity;           //!< The default text opacity
        Font                                      m_defaultFont;              //!< The default font
        Vector2f                                  m_availableClientSize;      //!< The size, that can be occupied by the layout
        Vector2f                                  m_occupiedLayoutSize;       //!< The size, that has been occupied by the layout
        FloatRect                                 m_evolvingLayoutArea;       //!< The area, that is available for the next layout element
        float                                     m_evolvingLineExtraHeight;  //!< The extra height of lines, that contain enlarged elements
        float                                     m_evolvingLineRunLength;    //!< The (text, image, ...) run length aggregated on the current line
        int                                       m_preformattedText;         //!< The flag, determining whether text is preformatted (keep '\n')

        FormattingState                           m_formattingState;          //!< The current state of all formatting attributes
        float                                     m_listPadding;              //!< The indent per list level
        float                                     m_backPadding;              //!< Back indent of the list item bullet, relative to the list item indent
    };
}

#endif // TGUI_FORMATTED_TEXT_XHTML_DOCUMENT_HPP
