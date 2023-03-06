/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Formatted text view (zlib license) provided for TGUI - Texus' Graphical User Interface
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


#ifndef TGUI_FORMATTED_TEXT_VIEW_HPP
#define TGUI_FORMATTED_TEXT_VIEW_HPP

namespace tgui
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The formatted text view widget
    ///
    /// A formatted text view widget is a multi-line formatted text output field which supports word-wrap and scrollbars.
    /// If you are looking for a multi-line text input field then check out the TextArea class (that doesn't support format).
    ///
    /// It displays the content of a FormattedTextDocument
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API FormattedTextView : public ClickableWidget
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        using Ptr = std::shared_ptr<FormattedTextView>;            //!< Shared widget pointer
        using ConstPtr = std::shared_ptr<const FormattedTextView>; //!< Shared constant widget pointer

        static constexpr const char StaticWidgetType[] = "FormattedTextView";

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Initializing constructor
        ///
        /// @param typeName     Type of the widget
        /// @param initRenderer Should the renderer be initialized? Should be true unless a derived class initializes it.
        ///
        /// @see create
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        FormattedTextView(const char* typeName = StaticWidgetType, bool initRenderer = true);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new XHTML static widget
        ///
        /// @param size  Size of the clickable widget
        ///
        /// @return The new XHTML static
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static inline FormattedTextView::Ptr create(const Layout2d& size = {"100%", "100%"})
        {   auto widget = std::make_shared<FormattedTextView>(StaticWidgetType, true);   widget->setSize(size);   return widget;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of the widget if you don't know its exact type
        ///
        /// This function should only be used when you don't know the type of the widget.
        /// If you know what kind of widget you are copying, you should use the copy function.
        ///
        /// @return Copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD inline Widget::Ptr clone() const override
        {   return std::make_shared<FormattedTextView>(*this);   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another XHTML ststic
        ///
        /// @param formattedTextView  The other XHTML ststic
        ///
        /// @return The new XHTML static
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static FormattedTextView::Ptr copy(FormattedTextView::ConstPtr formattedTextView);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the renderer, which gives access to functions that determine how the widget is displayed
        ///
        /// @return Temporary pointer to the renderer that may be shared with other widgets using the same renderer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline FormattedTextRenderer* getSharedRenderer()
        {   return aurora::downcast<FormattedTextRenderer*>(Widget::getSharedRenderer());   }

        inline const FormattedTextRenderer* getSharedRenderer() const
        {   return aurora::downcast<const FormattedTextRenderer*>(Widget::getSharedRenderer());   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the renderer, which gives access to functions that determine how the widget is displayed
        ///
        /// @return Temporary pointer to the renderer
        ///
        /// @warning After calling this function, the widget has its own copy of the renderer and it will no longer be shared.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline FormattedTextRenderer* getRenderer()
        {   return aurora::downcast<FormattedTextRenderer*>(Widget::getRenderer());   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the text view
        ///
        /// This size does not include the borders.
        ///
        /// @param size  The new size of the text view
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(const Layout2d& size) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the widget
        ///
        /// @param width   Width of the widget
        /// @param height  Height of the widget
        ///
        /// @warning This setSize overload must never be used from internal TGUI code or by custom widget implementations
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setSize(Layout width, Layout height)
        {   setSize({std::move(width), std::move(height)});   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the underlying XHTML document
        ///
        /// @param document  The underlying XHTML document to set
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setDocument(FormattedTextXhtmlDocument::Ptr document)
        {   m_document = document;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the underlying XHTML document
        ///
        /// @return The underlying XHTML document
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline FormattedTextDocument::Ptr getDocument()
        {   return m_document;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the font collection
        ///
        /// @param height  The font collection to set
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setFontCollection(const FormattedTextDocument::FontCollection& fontCollection)
        {
            if (fontCollection.assertValid())
                m_fontCollection = fontCollection;
            else
                throw "Font collection invalid!";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the font collection
        ///
        /// @return The font collection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline const FormattedTextDocument::FontCollection& getFontCollection() const
        {   return m_fontCollection;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void recalculateVisiblePart()
        {}

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool mouseWheelScrolled(float delta, Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes whether the horizontal scroll-bar should be displayed
        ///
        /// @param policy  The policy for displaying the horizontal scroll-bar
        ///
        /// The default policy is Automatic, which means word-wrap will be used to keep the text within the FormattedTextView.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setHorizontalScrollbarPolicy(Scrollbar::Policy policy);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether the horizontal scroll- bar should be displayed
        ///
        /// @return The policy for displaying the horizontal scroll-bar
        ///
        /// The default policy is Automatic, which means word-wrap will be used to keep the text within the FormattedTextView.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline Scrollbar::Policy getHorizontalScrollbarPolicy() const
        {   return m_horizontalScrollbarPolicy;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes whether the vertical scroll-bar should be displayed
        ///
        /// @param policy  The policy for displaying the vertical scroll-bar
        ///
        /// The default policy is Always, which means word-wrap will be used to keep the text within the FormattedTextView.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setVerticalScrollbarPolicy(Scrollbar::Policy policy);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether the vertical scroll- bar should be displayed
        ///
        /// @return The policy for displaying the vertical scroll-bar
        ///
        /// The default policy is Always, which means word-wrap will be used to keep the text within the FormattedTextView.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline Scrollbar::Policy getVerticalScrollbarPolicy() const
        {   return m_verticalScrollbarPolicy;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the thumb position of the horizontal scroll-bar
        ///
        /// @param value  New value of the horizontal scroll-bar
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setHorizontalScrollbarValue(unsigned int value)
        {   m_horizontalScrollbar->setValue(value);   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the thumb position of the horizontal scroll-bar
        ///
        /// @return Value of the horizontal scroll-bar
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline unsigned int getHorizontalScrollbarValue() const
        {   return m_horizontalScrollbar->getValue();   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the thumb position of the vertical scroll-bar
        ///
        /// @param value  New value of the vertical scroll-bar
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline void setVerticalScrollbarValue(unsigned int value)
        {   m_verticalScrollbar->setValue(value);   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the thumb position of the vertical scroll-bar
        ///
        /// @return Value of the vertical scroll-bar
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline unsigned int getVerticalScrollbarValue() const
        {   return m_verticalScrollbar->getValue();   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Updates the zoom (limited between 10% and 1000%)
        ///
        /// @param delta  The wheel scroll direction
        ///
        /// @return The flag whether zomm update was successful (true) or not (false)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool updateZoom(float delta);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieves a signal based on its name
        ///
        /// @param signalName  Name of the signal
        ///
        /// @return Signal that corresponds to the name
        ///
        /// @throw Exception when the name does not match any signal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Signal& getSignal(String signalName) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the zoom (limited between 10% and 1000%)
        ///
        /// @param newZoom  The new zoom
        ///
        /// @return The flag whether zomm set was successful (true) or not (false)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setZoom(float newZoom);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the zoom (limited between 10% and 1000%)
        ///
        /// @return The current zoom
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        inline float getZoom() const
        {   return m_zoom;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void leftMousePressed(Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void leftMouseReleased(Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mouseMoved(Vector2f pos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mouseNoLongerOnWidget() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void leftMouseButtonNoLongerDown() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Focus or unfocus the widget
        /// @param focused  Is the widget focused?
        ///
        /// When a widget is focused, the previously focused widget will be unfocused.
        ///
        /// @warning This function only works properly when the widget was already added to its parent (e.g. the Gui).
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setFocused(bool focused) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Updates the physical size of the scroll-bars, as well as the view-port size.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateScrollbars();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Rearrange the complete visible content
        ///
        /// @param keepSelection   Determine whether to keep the selection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void rearrangeText(bool keepSelection);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draw the widget to a render target
        ///
        /// @param target Render target to draw to
        /// @param states Current render states
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void draw(BackendRenderTarget& target, RenderStates states) const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Gets the size without the borders
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2f getInnerSize() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Gets the size without the borders, padding and scrollbars
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2f getRenderSize() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        SignalFloat   onZoomChange = {"ZoomChanged"}; //!< The zoom was changed. Parameter: new zoom

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        FormattedTextDocument::Ptr               m_document;              //!< The formatted document to display
        float                                    m_zoom = 1.0f;           //!< The zoom

        Borders                                  m_bordersCached;
        Padding                                  m_paddingCached;
        Color                                    m_borderColorCached;
        Color                                    m_backgroundColorCached;

        CopiedSharedPtr<ScrollbarChildWidget>    m_horizontalScrollbar;   //!< The horizontal scrollbar
        CopiedSharedPtr<ScrollbarChildWidget>    m_verticalScrollbar;     //!< The vertical scrollbar
        Scrollbar::Policy                        m_verticalScrollbarPolicy = Scrollbar::Policy::Automatic;
        Scrollbar::Policy                        m_horizontalScrollbarPolicy = Scrollbar::Policy::Never;

        // Is there a possibility that the user is going to double click?
        bool                                     m_possibleDoubleClick = false;

        // The fonts
        FormattedTextDocument::FontCollection    m_fontCollection;
        std::vector<FormattedLink::Ptr>          m_anchorSources;         //!< The list of formatted links
        std::map<String, FormattedElement::Ptr>  m_anchorTargets;         //!< The list of link targets
    };

}

#endif // TGUI_FORMATTED_TEXT_VIEW_HPP
