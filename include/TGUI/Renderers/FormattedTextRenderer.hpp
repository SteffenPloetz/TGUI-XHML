/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Formatted text renerer (zlib license) provided for TGUI - Texus' Graphical User Interface
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


#ifndef TGUI_XHTML_FORMATTED_TEXT_RENDERER_HPP
#define TGUI_XHTML_FORMATTED_TEXT_RENDERER_HPP


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class TGUI_API FormattedTextRenderer : public WidgetRenderer
    {
    public:

        using WidgetRenderer::WidgetRenderer;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the borders
        ///
        /// @param borders  Size of the borders
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setBorders(const Borders& borders);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the size of the borders
        ///
        /// @return border size
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Borders getBorders() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the padding of the list box
        ///
        /// @param padding  The padding width and height
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setPadding(const Padding& padding);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the size of the padding
        ///
        /// @return padding size
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Padding getPadding() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the background color of the text widget
        ///
        /// @param backgroundColor  The new background color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setBackgroundColor(Color backgroundColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the background color
        ///
        /// @return Background color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Color getBackgroundColor() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the text
        ///
        /// @param textColor  The new text color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTextColor(Color textColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the color of the text
        ///
        /// @return Text color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Color getTextColor() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the color of the default text that can optionally be displayed when the text widget is empty
        ///
        /// @param defaultTextColor  The new default text color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setDefaultTextColor(Color defaultTextColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the color of the default text that can optionally be displayed when the text widget is empty
        ///
        /// @return Default text color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Color getDefaultTextColor() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the selected text
        ///
        /// @param textColor  The new selected text color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSelectedTextColor(Color textColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the color of the selected text
        ///
        /// @return Selected text color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Color getSelectedTextColor() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the background of the selected text
        ///
        /// @param textColor  The new selected text background color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSelectedTextBackgroundColor(Color textColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the color of the background of the selected text
        ///
        /// @return Selected text background color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Color getSelectedTextBackgroundColor() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the borders
        ///
        /// @param borderColor  The new border color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setBorderColor(Color borderColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the color of the borders
        ///
        /// @return Border color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Color getBorderColor() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the color of the blinking caret
        ///
        /// @param caretColor  The new caret color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setCaretColor(Color caretColor);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the color of the blinking caret
        ///
        /// @return Caret color
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Color getCaretColor() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the width of the blinking caret
        ///
        /// @param width  New width of the caret
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setCaretWidth(float width);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the width of the blinking caret
        ///
        /// @return caret width
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float getCaretWidth() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the renderer data of the scrollbar
        ///
        /// @param scrollbarRendererData  Data about how the scrollbar should look
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setScrollbar(std::shared_ptr<RendererData> scrollbarRendererData);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the renderer data of the scrollbar
        ///
        /// @return Data about how the scrollbar looks
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::shared_ptr<RendererData> getScrollbar() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the wanted width scrollbar
        /// @param scrollbarWidth  Requested scrollbar width or 0 to use the default width (texture size if using textures)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setScrollbarWidth(float scrollbarWidth);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the wanted width scrollbar
        /// @return Requested scrollbar width or 0 if no width was set (texture width or default value will be used)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float getScrollbarWidth() const;
    };
}

#endif // TGUI_XHTML_FORMATTED_TEXT_RENDERER_HPP
