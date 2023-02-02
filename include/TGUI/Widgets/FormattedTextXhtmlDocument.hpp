#ifndef TGUI_FORMATTED_TEXT_XHTML_DOCUMENT_HPP
#define TGUI_FORMATTED_TEXT_XHTML_DOCUMENT_HPP

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Helper to determine the best matching font pack from the most common font family names
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class DefaultFontFamilies
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether the font family name can be recognized as the name of a Serif font family
        ///
        /// @param left  The font family name
        ///
        /// @return The flag determining whether the font family name is recognized as the name of a Serif font family
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool isSerif(const String& familyName)
        {
            // This line will execute only on the first execution of this function:
            static std::vector<String> m_serif = { U"Times New Roman", U"Georgia", U"Garamond", U"Serif" };
            for (size_t index = 0; index < m_serif.size(); index++)
                if (familyName.contains(m_serif[index].c_str()))
                return true;
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether the font family name can be recognized as the name of a Sans font family
        ///
        /// @param left  The font family name
        ///
        /// @return The flag determining whether the font family name is recognized as the name of a Sans font family
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool isSans(const String& familyName)
        {
            static std::vector<String> m_sans = { U"Arial" U"Verdana" U"Helvetica" U"Sans-serif" };
            for (size_t index = 0; index < m_sans.size(); index++)
                if (familyName.contains(m_sans[index].c_str()))
                return true;
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether the font family name can be recognized as the name of a Mono font family
        ///
        /// @param left  The font family name
        ///
        /// @return The flag determining whether the font family name is recognized as the name of a Mono font family
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool isMono(const String& familyName)
        {
            static std::vector<String> m_mono = { U"Courier New", U"Lucida Console", U"Monaco", U"Monospace" };
            for (size_t index = 0; index < m_mono.size(); index++)
                if (familyName.contains(m_mono[index].c_str()))
                return true;
            return false;
        }
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The formatted text document
    ///
    /// A formatted text data holder generic base class.
    /// Specialized formatted text data holder are derived from this class.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API FormattedTextDocument
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The font-pack supports the minimum variety of font-style (normal, italic/oblique) and font-weight (normal, bold)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API FontPack
        {
        public:
            std::shared_ptr<Font> Regular;    //!< The regular font of this font pack
            std::shared_ptr<Font> Bold;       //!< The bold font of this font pack. A dedicated bold font provides better display quality than an widened regular font
            std::shared_ptr<Font> Italic;     //!< The italic font of this font pack. A dedicated italic font provides better display quality than an slanted regular font
            std::shared_ptr<Font> BoldItalic; //!< The bold italic font of this font pack. A dedicated bold italic font provides better display quality than an widened and slanted regular font

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief The default constructor
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FontPack()
                : Regular(), Bold(), Italic(), BoldItalic()
            { ; }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Asserts the validity of this font pack
            ///
            /// @return Either true on a valid font pack, or false otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            bool inline assertValid() const
            {
                return Regular != nullptr &&
                       Bold != nullptr &&
                       Italic != nullptr &&
                       BoldItalic != nullptr;
            }
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The font-collection supports the minimum variety of font-faces (sans, serif, mono)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API FontCollection
        {
        public:
            std::shared_ptr<FontPack> Sans;  //!< The sand font pack of this font collection
            std::shared_ptr<FontPack> Serif; //!< The serif font pack of this font collection
            std::shared_ptr<FontPack> Mono;  //!< The monospace font pack of this font collection

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief The default constructor
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FontCollection()
                : Sans(), Serif(), Mono()
            { ; }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Asserts the validity of this font collection
            ///
            /// @return Either true on a valid font collection, or false otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            bool inline assertValid() const
            {
                return Sans != nullptr && Sans->assertValid() &&
                       Serif != nullptr && Serif->assertValid() &&
                       Mono != nullptr && Mono->assertValid();
            }
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The list-data supports the individual specifications of a list
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API ListData
        {
        public:
            typedef std::shared_ptr<ListData> Ptr; //!< Shared list data pointer

            bool         Ordered;          //!< Determine whether the list is ordered or unordered
            int          ActualItemIndex;  //!< The 1-based index of the actual list item
            ListItemType ItemType;         //!< Determine the bullet to apply

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Calculates the bullet of a list item
            ///
            /// @param nestingDepth     The nesting depth of the current list
            ///
            /// @return The bullet of a list item
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            String calculateBullet(size_t nestingDepth)
            {
                String bullet = L"";
                if (!Ordered)
                {
                    if     (nestingDepth % 3 == 1)
                    {
                        if (ItemType == ListItemType::InheritOrDefault || ItemType == ListItemType::Disc)
#if _WIN32
                            bullet.push_back(static_cast<wchar_t>(8226));
#else
                            bullet = L"•";
#endif
                        else if (ItemType == ListItemType::Circle)
#if _WIN32
                            bullet.push_back(static_cast<wchar_t>(9702));
#else
                            bullet = L"∘";
#endif
                        else if (ItemType == ListItemType::Square)
#if _WIN32
                            bullet.push_back(static_cast<wchar_t>(9642));
#else
                            bullet = L"▪";
#endif
                    }
                    else if (nestingDepth % 3 == 2)
                    {
                        if (ItemType == ListItemType::InheritOrDefault || ItemType == ListItemType::Circle)
#if _WIN32
                            bullet.push_back(static_cast<wchar_t>(9702));
#else
                            bullet = L"∘";
#endif
                        else if (ItemType == ListItemType::Disc)
#if _WIN32
                            bullet.push_back(static_cast<wchar_t>(8226));
#else
                            bullet = L"•";
#endif
                        else if (ItemType == ListItemType::Square)
#if _WIN32
                            bullet.push_back(static_cast<wchar_t>(9642));
#else
                            bullet = L"▪";
#endif
                    }
                    else
                    {
                        if (ItemType == ListItemType::InheritOrDefault || ItemType == ListItemType::Square)
#if _WIN32
                            bullet.push_back(static_cast<wchar_t>(9642));
#else
                            bullet = L"▪";
#endif
                        else if (ItemType == ListItemType::Disc)
#if _WIN32
                            bullet.push_back(static_cast<wchar_t>(8226));
#else
                            bullet = L"•";
#endif
                        else if (ItemType == ListItemType::Circle)
#if _WIN32
                            bullet.push_back(static_cast<wchar_t>(9702));
#else
                            bullet = L"∘";
#endif
                    }
                }
                else
                {
                    if (nestingDepth % 2 == 1)
                    {
                        if(ItemType == ListItemType::InheritOrDefault || ItemType == ListItemType::Arabic)
                            bullet = std::to_wstring(ActualItemIndex);
                        else if(ItemType == ListItemType::UpperAlpha)
                            bullet = (wchar_t)((wchar_t)'@' + (wchar_t)ActualItemIndex);
                        else if(ItemType == ListItemType::LowerAlpha)
                            bullet = (wchar_t)((wchar_t)'`' + (wchar_t)ActualItemIndex);
                    }
                    else
                    {
                        if(ItemType == ListItemType::InheritOrDefault || ItemType == ListItemType::LowerAlpha)
                            bullet = (wchar_t)((wchar_t)'`' + (wchar_t)ActualItemIndex);
                        else if(ItemType == ListItemType::UpperAlpha)
                            bullet = (wchar_t)((wchar_t)'@' + (wchar_t)ActualItemIndex);
                        else if(ItemType == ListItemType::Arabic)
                            bullet = std::to_wstring(ActualItemIndex);
                    }
                    bullet += L".";
                }
                return bullet;
            }
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The formatting-state holds the current formatting
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API FormattingState
        {
        public:
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief The default constructor
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FormattingState(const Color& foreColor)
                : ForeColor(foreColor), BackColor(Color(255, 255, 255)), Opacity(1), TextHeight(14.0f),
                  TextFont(nullptr), Style(TextStyle::Regular), Superscript(false), Subscript(false), ListMetrics(),
                  BorderWidth(SizeType::Pixel, 0.0f), BlockPadding(SizeType::Pixel, 0.0f)
            { ; }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief The copy constructor
            ///
            /// @param formattingState  The new formatting state
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FormattingState(const FormattingState& formattingState)
                : ForeColor(formattingState.ForeColor), BackColor(formattingState.BackColor), Opacity(formattingState.Opacity),
                  TextHeight(formattingState.TextHeight), TextFont(formattingState.TextFont),
                  Style(formattingState.Style), Superscript(formattingState.Superscript), Subscript(formattingState.Subscript),
                  ListMetrics(formattingState.ListMetrics), BorderWidth(formattingState.BorderWidth), BlockPadding(formattingState.BlockPadding)
            { ; }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief The assignment operator
            ///
            /// @param formattingState  The new formatting state
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FormattingState& operator =(FormattingState& formattingState)
            {
                ForeColor       = formattingState.ForeColor;
                BackColor       = formattingState.BackColor;
                Opacity         = formattingState.Opacity;
                TextHeight        = formattingState.TextHeight;
                TextFont        = formattingState.TextFont;
                Style           = formattingState.Style;
                Superscript     = formattingState.Superscript;
                Subscript       = formattingState.Subscript;
                ListMetrics     = formattingState.ListMetrics;
                BorderWidth     = formattingState.BorderWidth;
                BlockPadding    = formattingState.BlockPadding;

                return *this;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current text size to the requested text size
            ///
            /// @param textSize  The text size to set
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setTextSize(OneDimSize textSize)
            {
                if (textSize.sizeType == SizeType::Pixel)
                    TextHeight = textSize.value;
                else if (textSize.sizeType == SizeType::EquivalentOfM)
                    TextHeight = textSize.value * 16;
                else if (textSize.sizeType == SizeType::Point)
                    TextHeight = textSize.value * 1.3333333333f;
                else // (textSize.sizeType == SizeType::Relative)
                    TextHeight = TextHeight * textSize.value;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching font of the sans font family
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the sans regular font weight
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToSansFamily(const FormattedTextDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == *fontCollection.Sans->Regular    || TextFont == *fontCollection.Serif->Regular    || TextFont == *fontCollection.Mono->Regular)
                    TextFont = *fontCollection.Sans->Regular;
                else if (TextFont == *fontCollection.Sans->Italic     || TextFont == *fontCollection.Serif->Italic     || TextFont == *fontCollection.Mono->Italic)
                    TextFont = *fontCollection.Sans->Italic;
                else if (TextFont == *fontCollection.Sans->BoldItalic || TextFont == *fontCollection.Serif->BoldItalic || TextFont == *fontCollection.Mono->BoldItalic)
                    TextFont = *fontCollection.Sans->BoldItalic;
                else if (forceOnNoMatch)
                    TextFont = *fontCollection.Sans->Regular;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching font of the serif font family
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the serif regular font weight
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToSerifFamily(const FormattedTextDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == *fontCollection.Sans->Regular    || TextFont == *fontCollection.Serif->Regular    || TextFont == *fontCollection.Mono->Regular)
                    TextFont = *fontCollection.Serif->Regular;
                else if (TextFont == *fontCollection.Sans->Italic     || TextFont == *fontCollection.Serif->Italic     || TextFont == *fontCollection.Mono->Italic)
                    TextFont = *fontCollection.Serif->Italic;
                else if (TextFont == *fontCollection.Sans->BoldItalic || TextFont == *fontCollection.Serif->BoldItalic || TextFont == *fontCollection.Mono->BoldItalic)
                    TextFont = *fontCollection.Serif->BoldItalic;
                else if (forceOnNoMatch)
                    TextFont = *fontCollection.Serif->Regular;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching font of the mono font family
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the mono regular font weight
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToMonoFamily(const FormattedTextDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == *fontCollection.Sans->Regular    || TextFont == *fontCollection.Serif->Regular    || TextFont == *fontCollection.Mono->Regular)
                    TextFont = *fontCollection.Mono->Regular;
                else if (TextFont == *fontCollection.Sans->Italic     || TextFont == *fontCollection.Serif->Italic     || TextFont == *fontCollection.Mono->Italic)
                    TextFont = *fontCollection.Mono->Italic;
                else if (TextFont == *fontCollection.Sans->BoldItalic || TextFont == *fontCollection.Serif->BoldItalic || TextFont == *fontCollection.Mono->BoldItalic)
                    TextFont = *fontCollection.Mono->BoldItalic;
                else if (forceOnNoMatch)
                    TextFont = *fontCollection.Mono->Regular;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching font family
            ///
            /// @param fontFamily      The name of the requested font family
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font family is forced to the sans font family
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontFamily(const String& fontFamily, const FormattedTextDocument::FontCollection& fontCollection,
                                      bool forceOnNoMatch = false)
            {
                if      (DefaultFontFamilies::isSans(fontFamily))  setFontToSansFamily(fontCollection);
                else if (DefaultFontFamilies::isSerif(fontFamily)) setFontToSerifFamily(fontCollection);
                else if (DefaultFontFamilies::isMono(fontFamily))  setFontToMonoFamily(fontCollection);
                else if (forceOnNoMatch)                           setFontToSansFamily(fontCollection);
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching regular font weight (if not already regular font weight)
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the sans regular font weight
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToRegularWeight(const FormattedTextDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == *fontCollection.Sans->Regular  || TextFont == *fontCollection.Sans->Bold)
                    TextFont = *fontCollection.Sans->Regular;
                else if (TextFont == *fontCollection.Sans->Italic   || TextFont == *fontCollection.Sans->BoldItalic)
                    TextFont = *fontCollection.Sans->Italic;
                else if (TextFont == *fontCollection.Serif->Regular || TextFont == *fontCollection.Serif->Bold)
                    TextFont = *fontCollection.Serif->Regular;
                else if (TextFont == *fontCollection.Serif->Italic  || TextFont == *fontCollection.Serif->BoldItalic)
                    TextFont = *fontCollection.Serif->Italic;
                else if (TextFont == *fontCollection.Mono->Regular  || TextFont == *fontCollection.Mono->Bold)
                    TextFont = *fontCollection.Mono->Regular;
                else if (TextFont == *fontCollection.Mono->Italic   || TextFont == *fontCollection.Mono->BoldItalic)
                    TextFont = *fontCollection.Mono->Italic;
                else if (forceOnNoMatch)
                    TextFont = *fontCollection.Sans->Regular;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching bold font (if not already bold)
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the sans bold font
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToBold(const FormattedTextDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == *fontCollection.Sans->Regular  || TextFont == *fontCollection.Sans->Bold)
                    TextFont = *fontCollection.Sans->Bold;
                else if (TextFont == *fontCollection.Sans->Italic   || TextFont == *fontCollection.Sans->BoldItalic)
                    TextFont = *fontCollection.Sans->BoldItalic;
                else if (TextFont == *fontCollection.Serif->Regular || TextFont == *fontCollection.Serif->Bold)
                    TextFont = *fontCollection.Serif->Bold;
                else if (TextFont == *fontCollection.Serif->Italic  || TextFont == *fontCollection.Serif->BoldItalic)
                    TextFont = *fontCollection.Serif->BoldItalic;
                else if (TextFont == *fontCollection.Mono->Regular  || TextFont == *fontCollection.Mono->Bold)
                    TextFont = *fontCollection.Mono->Bold;
                else if (TextFont == *fontCollection.Mono->Italic   || TextFont == *fontCollection.Mono->BoldItalic)
                    TextFont = *fontCollection.Mono->BoldItalic;
                else if (forceOnNoMatch)
                    TextFont = *fontCollection.Sans->Bold;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font weight to the requested font weight
            ///
            /// @param bold            The requested font weight (bold or regular)
            /// @param fontCollection  The collection of available fonts
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontWeight(bool bold, const FormattedTextDocument::FontCollection& fontCollection)
            {   if (bold) setFontToBold(fontCollection); else setFontToRegularWeight(fontCollection);   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching regular font slant (if not already regular font slant)
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the sans regular font
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToRegularSlant(const FormattedTextDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == *fontCollection.Sans->Regular  || TextFont == *fontCollection.Sans->Italic)
                    TextFont = *fontCollection.Sans->Regular;
                else if (TextFont == *fontCollection.Sans->Bold     || TextFont == *fontCollection.Sans->BoldItalic)
                    TextFont = *fontCollection.Sans->Bold;
                else if (TextFont == *fontCollection.Serif->Regular || TextFont == *fontCollection.Serif->Italic)
                    TextFont = *fontCollection.Serif->Regular;
                else if (TextFont == *fontCollection.Serif->Bold    || TextFont == *fontCollection.Serif->BoldItalic)
                    TextFont = *fontCollection.Serif->Bold;
                else if (TextFont == *fontCollection.Mono->Regular  || TextFont == *fontCollection.Mono->Italic)
                    TextFont = *fontCollection.Mono->Regular;
                else if (TextFont == *fontCollection.Mono->Bold     || TextFont == *fontCollection.Mono->BoldItalic)
                    TextFont = *fontCollection.Mono->Bold;
                else if (forceOnNoMatch)
                    TextFont = *fontCollection.Sans->Regular;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching italic font (if not already italic)
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the sans italic font
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToItalic(const FormattedTextDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == *fontCollection.Sans->Regular  || TextFont == *fontCollection.Sans->Italic)
                    TextFont = *fontCollection.Sans->Italic;
                else if (TextFont == *fontCollection.Sans->Bold     || TextFont == *fontCollection.Sans->BoldItalic)
                    TextFont = *fontCollection.Sans->BoldItalic;
                else if (TextFont == *fontCollection.Serif->Regular || TextFont == *fontCollection.Serif->Italic)
                    TextFont = *fontCollection.Serif->Italic;
                else if (TextFont == *fontCollection.Serif->Bold    || TextFont == *fontCollection.Serif->BoldItalic)
                    TextFont = *fontCollection.Serif->BoldItalic;
                else if (TextFont == *fontCollection.Mono->Regular  || TextFont == *fontCollection.Mono->Italic)
                    TextFont = *fontCollection.Mono->Italic;
                else if (TextFont == *fontCollection.Mono->Bold     || TextFont == *fontCollection.Mono->BoldItalic)
                    TextFont = *fontCollection.Mono->BoldItalic;
                else if (forceOnNoMatch)
                    TextFont = *fontCollection.Sans->Italic;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font slant to the requested font slant
            ///
            /// @param italic          The requested font slant (italic or regular)
            /// @param fontCollection  The collection of available fonts
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontSlant(bool italic, const FormattedTextDocument::FontCollection& fontCollection)
            {   if (italic) setFontToItalic(fontCollection); else setFontToRegularSlant(fontCollection);   }

        public:
            Color                    ForeColor;
            Color                    BackColor;
            float                    Opacity;
            float                    TextHeight;
            Font                     TextFont;
            TextStyle                Style;
            float                    Superscript;  //!< Superscript offset to normal text
            float                    Subscript;    //!< Subscript offset to normal text
            std::list<ListData::Ptr> ListMetrics;  //!< Specifications of the (nested) lists
            FourDimSize              BorderWidth;  //!< Specifications of the border
            FourDimSize              BlockPadding; //!< Specifications of the padding
        };
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The formatted text XHTML document
    ///
    /// A formatted text data holder with XHTML element support.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API FormattedTextXhtmlDocument : public FormattedTextDocument
    {
    public:
        using Ptr = std::shared_ptr<FormattedTextXhtmlDocument>;            //!< Shared formatted text XHTML document pointer
        using ConstPtr = std::shared_ptr<const FormattedTextXhtmlDocument>; //!< Shared constant formatted text XHTML document pointer


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


    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new formatted text XHTML document
        ///
        /// Prefer the factory method in cases where the formatted text section needs to be shared, otherwise prefer the constructor.
        ///
        /// @return The new formatted text XHTML document
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static inline FormattedTextXhtmlDocument::Ptr create() { return std::make_shared<FormattedTextXhtmlDocument>(); }


    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the formatted text content of this document
        ///
        /// @return The formatted text content of this document
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline const std::vector<std::shared_ptr<FormattedElement>>& getContent() const { return m_content; }


    public:
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
        /// @brief Creates and basically initializes a FormattedTextSection
        ///
        /// @param xhtmlElement                             The XHTML element, represented by this formatted text section
        /// @param font                                     The font to apply
        /// @param indentOffset                             The horizontal offset (indent) to apply to the current layout position
        /// @param superscriptOrSubsciptTextHeightReduction The text height reduction applied for superscript / subscript
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
        inline Vector2f getOccupiedLayoutSize()
        { return Vector2f(m_occupiedLayoutSize.x, m_occupiedLayoutSize.y); }


    protected:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Rearrange the visible content, defined by the indicated XHTML Element
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void layout(bool& predecessorWasBlockNode, bool parentIsTextBlockElement, bool& PedecessorWasTextBlockElement,
                    XhtmlElement::Ptr xhtmlElement, const FormattedTextDocument::FontCollection& fontCollection, bool keepSelection);


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
