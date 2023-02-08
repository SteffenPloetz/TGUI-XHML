#include <TGUI/Config.hpp>
#include <TGUI/TGUI.hpp>

#include <list>
#include <map>

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
#include "TGUI/StringHelper.hpp"
#include "TGUI/Xhtml/XhtmlElements.hpp"
#include "TGUI/Widgets/FormattedElements.hpp"

#include "TGUI/Widgets/FormattedTextXhtmlDocument.hpp"

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FormattedTextXhtmlDocument::FormattedTextXhtmlDocument()
        : FormattedTextDocument(), m_textures(), m_content(), m_rootElement(), m_defaultTextSize(14.0f),
          m_defaultForeColor(Color(0, 0, 0)), m_defaultOpacity(1), m_defaultFont(nullptr), m_availableClientSize(0.0f, 0.0f),
          m_occupiedLayoutSize(0.0f, 0.0f), m_evolvingLayoutArea(0.0f, 0.0f, 0.0f, 0.0f),
          m_evolvingLineExtraHeight(0.0f), m_evolvingLineRunLength(0.0f), m_preformattedText(0), m_formattingState(Color(0, 0, 0)),
          m_listPadding(30), m_backPadding(4)
    {
        m_rootElement = XhtmlElement::createHtml(nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlContainerElement::Ptr FormattedTextXhtmlDocument::getHeadElement() const
    {
        for (size_t index = 0; index < m_rootElement->countChildren(); index++)
        {
            auto element = m_rootElement->getChild(index);
            if (strncmp(element->getTypeName(), XhtmlElementType::Head, strlen(XhtmlElementType::Head)) == 0)
                return std::dynamic_pointer_cast<XhtmlContainerElement>(element);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlStyle::Ptr FormattedTextXhtmlDocument::getStyleElement() const
    {
        auto headElement = getHeadElement();
        if (headElement != nullptr)
            return std::dynamic_pointer_cast<XhtmlStyle>(headElement->getFirstChild(XhtmlElementType::Style));
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<XhtmlStyleEntry::Ptr> FormattedTextXhtmlDocument::getApplicableStyleElements(XhtmlElement::Ptr xhtmlElement)
    {
        std::vector<XhtmlStyleEntry::Ptr> styleEntries;
        auto styleElement = getStyleElement();
        if (styleElement != nullptr)
        {
            auto classNames = ext::String::split(xhtmlElement->getClassNames(), U' ', true);
            for (auto className : classNames)
            {
                auto globalStyleEntry = styleElement->getEntry(xhtmlElement->getTypeName(), className);
                if (globalStyleEntry != nullptr)
                    styleEntries.push_back(globalStyleEntry);
            }
        }
        auto xhtmlStyleableElement = std::dynamic_pointer_cast<XhtmlStyleableInterface>(xhtmlElement);
        if (xhtmlStyleableElement != nullptr)
        {
            auto localStyleEntry = xhtmlStyleableElement->getStyleEntry();
            if (localStyleEntry != nullptr)
                styleEntries.push_back(localStyleEntry);
        }

        return styleEntries;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FormattedTextSection::Ptr FormattedTextXhtmlDocument::createFormattedTextSectionWithFontAndPosition(XhtmlElement::Ptr xhtmlElement,
        Font font, float indentOffset, float superscriptOrSubsciptTextHeightReduction)
    {
        auto formattedTextSection = std::make_shared<FormattedTextSection>(m_formattingState.ForeColor, m_formattingState.Style);
        formattedTextSection->setContentOrigin(xhtmlElement);
        formattedTextSection->setFont(font);
        formattedTextSection->setRenderLeftTop(Vector2f(m_evolvingLayoutArea.left, m_evolvingLayoutArea.top + m_evolvingLineExtraHeight), indentOffset, std::max(0.0f, superscriptOrSubsciptTextHeightReduction));
        float unscriptedTextHeight = m_formattingState.TextHeight + std::abs(superscriptOrSubsciptTextHeightReduction); // for superscript / subscript - the text height is already adjusted
        formattedTextSection->setRenderRightBottom(Vector2f(formattedTextSection->getRenderLeft(), m_evolvingLayoutArea.top + unscriptedTextHeight + m_evolvingLineExtraHeight), 0.0f, std::min(0.0f, superscriptOrSubsciptTextHeightReduction));
        formattedTextSection->setOpacity(m_formattingState.Opacity);

        return formattedTextSection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextXhtmlDocument::layout(Vector2f clientSize, float defaultTextSize, Color defaultForeColor,  float defaultOpacity,
                                            const FormattedTextDocument::FontCollection& fontCollection, bool keepSelection)
    {
        if(!fontCollection.assertValid())
            std::cerr << "Invalid font collection!\n";
        if(!m_defaultFont)
            std::cerr << "Invalid default font!\n";

        bool predecessorWasStructuringBlockElement = false;
        bool parentIsBlockElement = false;
        bool lastchildWasRunLengtElement = false;

        m_availableClientSize = {0.0f, 0.0f};
        m_occupiedLayoutSize = {0.0f, 0.0f};
        m_content.clear();
        m_defaultTextSize = defaultTextSize;
        m_defaultOpacity = defaultOpacity;
        m_defaultForeColor = defaultForeColor;
        m_defaultFont = *fontCollection.Sans->Regular;

        m_evolvingLayoutArea = FloatRect(0, 0, clientSize.x, clientSize.y);
        m_formattingState.TextHeight = defaultTextSize;
        m_formattingState.Opacity = defaultOpacity;
        m_formattingState.ForeColor = defaultForeColor;
        m_formattingState.TextFont = *fontCollection.Sans->Regular;

        for (size_t index = 0; index < m_rootElement->countChildren(); index++)
            layout(predecessorWasStructuringBlockElement, parentIsBlockElement, lastchildWasRunLengtElement,
                   m_rootElement->getChild(index), fontCollection,keepSelection);
        if (m_content.size() > 0)
        {
            FormattedElement::Ptr formattedElement = m_content[m_content.size() - 1];
            m_occupiedLayoutSize.y += formattedElement->getRenderSize().y + m_defaultTextSize / 2;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextXhtmlDocument::layout(bool& predecessorWasStructuringBlockElement, bool parentIsTextBlockElement,
                                            bool& lastchildWasRunLengtElement, std::shared_ptr<XhtmlElement> xhtmlElement,
                                            const FormattedTextDocument::FontCollection& fontCollection,
                                            bool keepSelection)
    {
        auto typeName = xhtmlElement->getTypeName();
        if (typeName == XhtmlElementType::Head)
            return;

        // Block elements always need to start a new line and to lock the current line.
        if (typeName == XhtmlElementType::Body ||
            typeName == XhtmlElementType::H1 || typeName == XhtmlElementType::H2 || typeName == XhtmlElementType::H3 || typeName == XhtmlElementType::H4 || typeName == XhtmlElementType::H5 || typeName == XhtmlElementType::H6 ||
            typeName == XhtmlElementType::Emphasized || typeName == XhtmlElementType::Strong || typeName == XhtmlElementType::Italic || typeName == XhtmlElementType::Bold || typeName == XhtmlElementType::Underline ||
            typeName == XhtmlElementType::Superscript || typeName == XhtmlElementType::Subscript || typeName == XhtmlElementType::UnorderedList || typeName == XhtmlElementType::OrderedList || typeName == XhtmlElementType::ListItem ||
            typeName == XhtmlElementType::Span || typeName == XhtmlElementType::Division || typeName == XhtmlElementType::Preformatted || typeName == XhtmlElementType::Code || typeName == XhtmlElementType::Paragraph ||
            typeName == XhtmlElementType::Image)
        {
            // prepare rect section to accommodate the block
            std::shared_ptr<FormattedRectangle> formattedRectSection = nullptr;

            FormattingState cachedState(m_formattingState);
            auto styleEntries = getApplicableStyleElements(xhtmlElement);
            auto xhtmlStyleableElement = std::dynamic_pointer_cast<XhtmlStyleableInterface>(xhtmlElement);

            bool currentIsTextBlockElement = false;

            // -------------------------------
            // Apply predefined font style
            // -------------------------------
            if (typeName == XhtmlElementType::H1 || typeName == XhtmlElementType::H2 || typeName == XhtmlElementType::H3)
            {
                m_formattingState.TextFont = *fontCollection.Sans->Bold;
                m_formattingState.TextHeight = (typeName == XhtmlElementType::H1 ? 28 * m_defaultTextSize / 14 :
                                               (typeName == XhtmlElementType::H2 ? 20 * m_defaultTextSize / 14 :
                                                                                   16 * m_defaultTextSize / 14));
                m_formattingState.ForeColor = m_defaultForeColor;
            }
            else if (typeName == XhtmlElementType::H4 || typeName == XhtmlElementType::H5 || typeName == XhtmlElementType::H6)
            {
                m_formattingState.TextFont = *fontCollection.Sans->Bold;
                m_formattingState.TextHeight = (typeName == XhtmlElementType::H4 ? 14 * m_defaultTextSize / 14 :
                                               (typeName == XhtmlElementType::H5 ? 12 * m_defaultTextSize / 14 :
                                                                                   10 * m_defaultTextSize / 14));
                m_formattingState.ForeColor = m_defaultForeColor;
            }
            else if (typeName == XhtmlElementType::Emphasized || typeName == XhtmlElementType::Italic)
            {
                m_formattingState.setFontToItalic(fontCollection);
            }
            else if (typeName == XhtmlElementType::Strong || typeName == XhtmlElementType::Bold)
            {
                m_formattingState.setFontToBold(fontCollection);
            }
            else if (typeName == XhtmlElementType::Underline)
            {
                m_formattingState.Style = TextStyle::Underlined;
            }
            else if (typeName == XhtmlElementType::Superscript)
            {
                m_formattingState.Superscript = m_formattingState.TextHeight / 3;
                m_formattingState.TextHeight = m_formattingState.TextHeight - m_formattingState.Superscript;
            }
            else if (typeName == XhtmlElementType::Subscript)
            {
                m_formattingState.Subscript = m_formattingState.TextHeight / 3;
                m_formattingState.TextHeight = m_formattingState.TextHeight - m_formattingState.Subscript;
            }

            // -------------------------------
            // Apply styled font style
            // -------------------------------
            if (xhtmlStyleableElement)
            {
                for (auto styleEntry : styleEntries)
                {
                    auto styleEntryFlags = styleEntry->getStyleEntryFlags();
                    if ((styleEntryFlags & StyleEntryFlags::ForeColor) == StyleEntryFlags::ForeColor)
                        m_formattingState.ForeColor = styleEntry->getColor();
                    if ((styleEntryFlags & StyleEntryFlags::BackColor) == StyleEntryFlags::BackColor)
                        m_formattingState.BackColor = styleEntry->getBackgroundColor();
                    if ((styleEntryFlags & StyleEntryFlags::Opacity) == StyleEntryFlags::Opacity)
                        m_formattingState.Opacity = styleEntry->getOpacity();
                    if ((styleEntryFlags & StyleEntryFlags::FontFamily) == StyleEntryFlags::FontFamily)
                        m_formattingState.setFontFamily(styleEntry->getFontFamily(), fontCollection);
                    if ((styleEntryFlags & StyleEntryFlags::FontStyle) == StyleEntryFlags::FontStyle)
                        m_formattingState.setFontWeight(styleEntry->getBold(), fontCollection);
                    if ((styleEntryFlags & StyleEntryFlags::FontStyle) == StyleEntryFlags::FontStyle)
                        m_formattingState.setFontSlant(styleEntry->getItalic(), fontCollection);
                    if ((styleEntryFlags & StyleEntryFlags::FontSize) == StyleEntryFlags::FontSize)
                        m_formattingState.setTextSize(styleEntry->getFontSize());
                }
            }

            // -------------------------------
            // Apply leading offsets
            // -------------------------------
            if (typeName == XhtmlElementType::Body)
            {
                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;
            }
            else if (typeName == XhtmlElementType::H1 || typeName == XhtmlElementType::H2 || typeName == XhtmlElementType::H3 ||
                     typeName == XhtmlElementType::H4 || typeName == XhtmlElementType::H5 || typeName == XhtmlElementType::H6)
            {
                if (predecessorWasStructuringBlockElement)
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight;
                else
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize + m_defaultTextSize + m_defaultTextSize / 2;

                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;
            }
            else if (typeName == XhtmlElementType::UnorderedList || typeName == XhtmlElementType::OrderedList)
            {
                auto parent  = xhtmlElement->getParent();
                auto sibling = XhtmlElement::getPreviousSibling(xhtmlElement);
                auto parentTypeName = (parent  != nullptr ? parent->getTypeName()  : XhtmlElementType::Body);
                auto predecTypeName = (sibling != nullptr ? sibling->getTypeName() : XhtmlElementType::Break);

                /*
                 * The list can be
                 * - outside any list [1] or
                 * - inside any list, where it can be
                 *   - embedded on the same level as the list items of se surrounding list [2] or
                 *   - embedded in a surrounding list's list item either
                 *     - directly at the top [3] or
                 *     - after some other content (text, image, ...) [4].
                */

                /* [1] */ if (m_formattingState.ListMetrics.size() == 0)
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize + m_defaultTextSize / 4 + m_defaultTextSize;
                /* [2] */ else if (parentTypeName != XhtmlElementType::UnorderedList && parentTypeName != XhtmlElementType::OrderedList)
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize + m_defaultTextSize / 4;
                /* [3] */ else if (predecTypeName == XhtmlElementType::ListItem)
                    m_evolvingLayoutArea.top += 0;
                /* [4] */ else
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize + m_defaultTextSize / 4;

                auto newListMetric = std::make_shared<FormattedTextDocument::ListData>();
                newListMetric->Ordered = (typeName == XhtmlElementType::OrderedList);
                newListMetric->ActualItemIndex = 0;
                newListMetric->ItemType = ListItemType::InheritOrDefault;
                m_formattingState.ListMetrics.push_back(newListMetric);

                currentIsTextBlockElement = true;

                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;
            }
            else if (typeName == XhtmlElementType::ListItem)
            {
                auto xhtmlListItem = std::dynamic_pointer_cast<XhtmlListItem>(xhtmlStyleableElement);
                FormattedTextDocument::ListData::Ptr listMetrics = m_formattingState.ListMetrics.back();

                listMetrics->ActualItemIndex++;
                if (listMetrics->ActualItemIndex == 1)
                {
                    // Attention: Asymmetric! Reverse inflating will not happen in matching XhtmlType::ListItem end, but in parent XhtmlType::List end!
                    inflate(m_evolvingLayoutArea, -m_listPadding, 0, 0, 0);
                }

                String bullet = listMetrics->calculateBullet(m_formattingState.ListMetrics.size());
                tgui::Font font = (listMetrics->Ordered ? m_formattingState.TextFont : *fontCollection.Mono->Regular);
                auto runLengt = Text::getLineWidth(bullet, font, static_cast<unsigned int>(m_formattingState.TextHeight + 0.49f));

                auto formattedTextSection = createFormattedTextSectionWithFontAndPosition(xhtmlElement, font, -(m_backPadding + runLengt), m_formattingState.Subscript - m_formattingState.Superscript);
                formattedTextSection->setString(bullet);
                formattedTextSection->setRunLength(runLengt + 0.49f);
                formattedTextSection->setColor(m_formattingState.ForeColor);
                formattedTextSection->setStyle(m_formattingState.Style);

                auto formattedElement = std::static_pointer_cast<FormattedElement>(formattedTextSection);
                m_content.push_back(formattedElement);

                currentIsTextBlockElement = true;

                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;
            }
            else if (typeName == XhtmlElementType::Division || typeName == XhtmlElementType::Preformatted || typeName == XhtmlElementType::Code)
            {
                if (predecessorWasStructuringBlockElement || parentIsTextBlockElement)
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight;
                else
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_formattingState.TextHeight + m_formattingState.TextHeight / 4;

                if (typeName == XhtmlElementType::Preformatted)
                    m_preformattedText++;
                else if (typeName == XhtmlElementType::Code)
                {
                    m_formattingState.TextFont  = *fontCollection.Mono->Regular;
                    m_formattingState.TextHeight -= m_formattingState.TextHeight / 4;
                }

                // reduce one line break, if it is direct predecessor of 'front of'<code>'
                // (typically '<code>' is inside '<pre>' and '<pre>' forces linebrak fom source code to visual tree)
                if (typeName == XhtmlElementType::Code)
                {
                    auto sibling = XhtmlElement::getPreviousSibling(xhtmlElement);
                    if (sibling != nullptr && sibling->getTypeName() == XhtmlElementType::Break)
                        m_evolvingLayoutArea.top -= m_formattingState.TextHeight + m_formattingState.TextHeight / 4;
                }

                currentIsTextBlockElement = true;

                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;
            }
            else if (typeName == XhtmlElementType::Paragraph)
            {
                m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize + m_defaultTextSize / 2 + m_defaultTextSize;

                currentIsTextBlockElement = true;

                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;
            }

            // -------------------------------
            // Apply leading margin
            // -------------------------------
            if (xhtmlStyleableElement)
            {
                for (auto styleEntry : styleEntries)
                {
                    if ((styleEntry->getStyleEntryFlags() & StyleEntryFlags::Margin) == StyleEntryFlags::Margin)
                    {
                        auto margin = styleEntry->getMargin().toPixel(m_availableClientSize);
                        inflate(m_evolvingLayoutArea, -margin.left, -margin.top, -margin.right, 0);
                    }
                }
            }

            // -------------------------------
            // Initialize backgound and border
            // -------------------------------
            if (typeName == XhtmlElementType::Body || typeName == XhtmlElementType::UnorderedList || typeName == XhtmlElementType::OrderedList ||
                typeName == XhtmlElementType::Paragraph || typeName == XhtmlElementType::Preformatted || typeName == XhtmlElementType::Code ||
                typeName == XhtmlElementType::Division || typeName == XhtmlElementType::Image)
            {
                for (auto styleEntry : styleEntries)
                {
                    auto styleEntryFlags = styleEntry->getStyleEntryFlags();
                    if ((styleEntryFlags & StyleEntryFlags::BackColor) == StyleEntryFlags::BackColor ||
                        (styleEntryFlags & StyleEntryFlags::Opacity) == StyleEntryFlags::Opacity ||
                        ((styleEntryFlags & StyleEntryFlags::BorderWidth) == StyleEntryFlags::BorderWidth &&
                            !styleEntry->getBorderWidth().isEmpty(m_availableClientSize)) ||
                        (styleEntryFlags & StyleEntryFlags::BorderColor) == StyleEntryFlags::BorderColor)
                    {
                        formattedRectSection = std::make_shared<FormattedRectangle>();
                        formattedRectSection->setRenderArea(m_evolvingLayoutArea.getPosition());
                        if ((styleEntryFlags & StyleEntryFlags::BackColor) == StyleEntryFlags::BackColor)
                            formattedRectSection->setBackgroundColor(styleEntry->getBackgroundColor());
                        if ((styleEntryFlags & StyleEntryFlags::Opacity) == StyleEntryFlags::Opacity)
                            formattedRectSection->setOpacity(styleEntry->getOpacity());
                        if ((styleEntryFlags & StyleEntryFlags::BorderWidth) == StyleEntryFlags::BorderWidth)
                            formattedRectSection->setBoderWidth(styleEntry->getBorderWidth());
                        if ((styleEntryFlags & StyleEntryFlags::BorderColor) == StyleEntryFlags::BorderColor)
                            formattedRectSection->setBorderColor(styleEntry->getBorderColor());
                        auto formattedElement = std::static_pointer_cast<FormattedElement>(formattedRectSection);
                        m_content.push_back(formattedElement);
                    }
                }
            }

            // -----------------------------
            // Apply leading padding
            // -----------------------------
            if (xhtmlStyleableElement)
            {
                for (auto styleEntry : styleEntries)
                {
                    if ((styleEntry->getStyleEntryFlags() & StyleEntryFlags::Padding) == StyleEntryFlags::Padding)
                    {
                        auto padding = styleEntry->getPadding().toPixel(m_availableClientSize);
                        inflate(m_evolvingLayoutArea, -padding.left, -padding.top, -padding.right, 0);
                    }
                }
            }

            // -------------------------------
            // Process children
            // -------------------------------
            bool loopinternalPedecessorWasStructuringBlockElement = false;
            bool lastchildWasRunLengtElement = false;
            for (size_t index = 0; xhtmlElement->isContainer() && index < xhtmlElement->countChildren(); index++)
                layout(loopinternalPedecessorWasStructuringBlockElement, currentIsTextBlockElement, lastchildWasRunLengtElement,
                    xhtmlElement->getChild(index), fontCollection, keepSelection);


            // -----------------------------
            // Apply tailing padding
            // -----------------------------
            if (xhtmlStyleableElement)
            {
                for (auto styleEntry : styleEntries)
                {
                    if ((styleEntry->getStyleEntryFlags() & StyleEntryFlags::Padding) == StyleEntryFlags::Padding)
                    {
                        auto padding = styleEntry->getPadding().toPixel(m_availableClientSize);
                        inflate(m_evolvingLayoutArea, padding.left, -padding.bottom, padding.right, 0);
                    }
                }
            }

            // -------------------------------
            // Finalize background
            // -------------------------------
            if (typeName == XhtmlElementType::Body      || typeName == XhtmlElementType::UnorderedList || typeName == XhtmlElementType::OrderedList ||
                typeName == XhtmlElementType::Paragraph || typeName == XhtmlElementType::Preformatted  || typeName == XhtmlElementType::Code ||
                typeName == XhtmlElementType::Division  || typeName == XhtmlElementType::Image)
            {
                if (formattedRectSection != nullptr)
                {
                    auto lastTextSection = std::dynamic_pointer_cast<tgui::FormattedTextSection>(m_content.back());
                    if (lastTextSection)
                        // typically the the text sections are always open to add new charachters (in other words: not finalized with line break / carriage return)
                        // which implies, that m_evolvingLayoutArea.top points still to the top of the  text sections, while they might have a heigth
                        formattedRectSection->setRenderRightBottom(Vector2f(right(m_evolvingLayoutArea), lastTextSection->getRenderRefLine() + m_formattingState.TextHeight / 4));
                    else
                        formattedRectSection->setRenderRightBottom(Vector2f(right(m_evolvingLayoutArea), m_evolvingLayoutArea.top));
                }
            }

            // -------------------------------
            // Apply tailing margin
            // -------------------------------
            if (xhtmlStyleableElement)
            {
                for (auto styleEntry : styleEntries)
                {
                    if ((styleEntry->getStyleEntryFlags() & StyleEntryFlags::Margin) == StyleEntryFlags::Margin)
                    {
                        auto margin = styleEntry->getMargin().toPixel(m_availableClientSize);
                        inflate(m_evolvingLayoutArea, margin.left, -margin.bottom, margin.right, 0);
                    }
                }
            }

            // -------------------------------
            // Apply tailing offsets
            // -------------------------------
            if (typeName == XhtmlElementType::H1 || typeName == XhtmlElementType::H2 || typeName == XhtmlElementType::H3 ||
                typeName == XhtmlElementType::H4 || typeName == XhtmlElementType::H5 || typeName == XhtmlElementType::H6)
            {
                m_evolvingLayoutArea.top += m_evolvingLineExtraHeight +
                                            (m_formattingState.TextHeight >= m_defaultTextSize ?
                                             m_formattingState.TextHeight  + m_formattingState.TextHeight  / 2 + m_defaultTextSize :
                                             m_defaultTextSize + m_defaultTextSize / 2 + m_defaultTextSize);

                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;
            }
            else if (typeName == XhtmlElementType::UnorderedList || typeName == XhtmlElementType::OrderedList)
            {
                m_formattingState.ListMetrics.pop_back();

                if (m_formattingState.ListMetrics.size() == 0)
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize;
                else
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight;

                // Attention: Asymmetric! Initial inflating was not happen in matching XhtmlType::List begin, but in parent XhtmlType::ListItem begin!
                inflate(m_evolvingLayoutArea, m_listPadding, 0, 0, 0);

                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;
            }
            else if (typeName == XhtmlElementType::ListItem)
            {
                auto lastTypeName = XhtmlElementType::Break;
                auto childCount = xhtmlElement->countChildren();
                if (childCount > 0)
                {
                    auto lastChild = xhtmlElement->getChild(childCount - 1);
                    lastTypeName = lastChild->getTypeName();
                }
                if (lastTypeName != XhtmlElementType::UnorderedList && lastTypeName != XhtmlElementType::OrderedList)
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize / 4 + m_defaultTextSize;
                else
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize / 4;

                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;
            }
            else if (typeName == XhtmlElementType::Division || typeName == XhtmlElementType::Preformatted || typeName == XhtmlElementType::Code)
            {
                if (lastchildWasRunLengtElement)
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight;
                else
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize;

                if (typeName == XhtmlElementType::Preformatted)
                    m_preformattedText--;
                if (typeName == XhtmlElementType::Preformatted && m_preformattedText == 0)
                    m_evolvingLayoutArea.top += m_formattingState.TextHeight;

                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;
            }
            else if (typeName == XhtmlElementType::Paragraph)
            {
                m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize + m_defaultTextSize / 2 + m_defaultTextSize;

                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;
            }

            // -------------------------------
            // Set flags
            // -------------------------------
            predecessorWasStructuringBlockElement = false;

            if (typeName == XhtmlElementType::H1 || typeName == XhtmlElementType::H2 || typeName == XhtmlElementType::H3 ||
                typeName == XhtmlElementType::H4 || typeName == XhtmlElementType::H5 || typeName == XhtmlElementType::H6)
            {
                predecessorWasStructuringBlockElement = true;
                lastchildWasRunLengtElement = false;
            }
            else if (typeName == XhtmlElementType::UnorderedList || typeName == XhtmlElementType::OrderedList)
            {
                predecessorWasStructuringBlockElement = true;
                lastchildWasRunLengtElement = false;
            }
            else if (typeName == XhtmlElementType::Division || typeName == XhtmlElementType::Preformatted || typeName == XhtmlElementType::Code)
                lastchildWasRunLengtElement = false;
            else if (typeName == XhtmlElementType::Paragraph)
                lastchildWasRunLengtElement = false;

            m_formattingState = cachedState;
        }

        if (typeName == XhtmlElementType::Break || typeName == XhtmlElementType::Text)
        {
            // prepare text section to accommodate the text
            auto formattedTextSection = createFormattedTextSectionWithFontAndPosition(xhtmlElement, m_formattingState.TextFont, m_evolvingLineRunLength, m_formattingState.Subscript - m_formattingState.Superscript);

            if (typeName == XhtmlElementType::Break)
            {
                m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_formattingState.TextHeight + m_formattingState.TextHeight / 4;

                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;

                predecessorWasStructuringBlockElement = false;
                parentIsTextBlockElement = false;
            }
            else if (typeName == XhtmlElementType::Text)
            {
                String remainingText = ((XhtmlInnerText*)&(*xhtmlElement))->getText().toUtf32();
                auto   runLengt      = Text::getLineWidth(remainingText, m_formattingState.TextFont, static_cast<unsigned int>(m_formattingState.TextHeight + 0.49f));
                size_t formerCharCnt = remainingText.size() + 1;

                // distribute, if text exceeds available width
                while ((m_preformattedText > 0 && remainingText.contains('\r')) || // preformatted text has a line bereak
                       (formerCharCnt > remainingText.size() &&                    // there must be a chance to distribute the text to multiple lines
                        m_evolvingLayoutArea.width - m_evolvingLineRunLength < runLengt && // there must be a need to distribute the text to multiple lines
                        remainingText.size() > 0))                                 // there must be remaining text to distribute to multiple lines
                {
                    formerCharCnt = remainingText.size();

                    // Determine auto-line-break: For a mono-space font, the runLength is distributed equally to the characters.
                    size_t delimiterPosition = (int)(remainingText.size() * ((1.0f * (m_evolvingLayoutArea.width - m_evolvingLineRunLength)) / (1.0f * runLengt)));
                    // The line width calculation is not 100% correct. - So some security space is subtracted.
                    delimiterPosition -= (delimiterPosition > 333 ? 8 : (delimiterPosition > 99 ? 6 : (delimiterPosition > 33 ? 4 : (delimiterPosition > 9 ? 2 : (delimiterPosition > 3 ? 1 : 0)))));
                    String delimitercharacters(L"\r\n\t\v -");
                    size_t linebreakPosition = remainingText.find_last_of(delimitercharacters, delimiterPosition);

                    // The preformatted text might force an earlier line-break.
                    if (m_preformattedText > 0)
                    {
                        auto   substring      = remainingText.substr(0, linebreakPosition);
                        size_t forcedPosition = substring.find('\r');
                        if (forcedPosition != static_cast<size_t>(-1))
                            linebreakPosition = forcedPosition;
                    }
                    // There might be no way to add the remaining text (without any possibility to auto-line-break) to the end of the current run length.
                    if (linebreakPosition < 1 || linebreakPosition > delimiterPosition)
                    {
                        m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_formattingState.TextHeight + m_formattingState.TextHeight / 4;

                        // act like a "\r\n"
                        m_evolvingLineRunLength = 0;
                        m_evolvingLineExtraHeight = 0;

                        formattedTextSection->setRenderLeftTop(Vector2f(m_evolvingLayoutArea.left, m_evolvingLayoutArea.top + m_evolvingLineExtraHeight), m_evolvingLineRunLength, m_formattingState.Subscript);
                        formattedTextSection->setRenderRightBottom(Vector2f(formattedTextSection->getRenderLeft(), m_evolvingLayoutArea.top + m_formattingState.TextHeight + m_evolvingLineExtraHeight), 0.0f, -m_formattingState.Superscript);
                        break;
                    }
                    delimiterPosition = linebreakPosition;

                    formattedTextSection->setString(remainingText.substr(0, delimiterPosition + (remainingText[delimiterPosition] == L'-' ? 1 : 0)));
                    runLengt = Text::getLineWidth(formattedTextSection->getString(), m_formattingState.TextFont, static_cast<unsigned int>(m_formattingState.TextHeight + 0.49f));
                    formattedTextSection->setRunLength(runLengt + 0.45f);
                    m_evolvingLineRunLength += runLengt;
                    formattedTextSection->setColor(m_formattingState.ForeColor);
                    formattedTextSection->setStyle(m_formattingState.Style);

                    remainingText = remainingText.substr(delimiterPosition + 1);
                    runLengt = Text::getLineWidth(remainingText, m_formattingState.TextFont, static_cast<unsigned int>(m_formattingState.TextHeight + 0.49f));

                    if (remainingText.size() > 0)
                    {
                        m_occupiedLayoutSize.x = std::max(m_occupiedLayoutSize.x, formattedTextSection->getRenderLeft());
                        m_occupiedLayoutSize.y = formattedTextSection->getRenderTop();
                        auto formattedElement = std::static_pointer_cast<FormattedElement>(formattedTextSection);
                        m_content.push_back(formattedElement);

                        m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_formattingState.TextHeight + m_formattingState.TextHeight / 4;

                        // act like a "\r\n"
                        m_evolvingLineRunLength = 0;
                        m_evolvingLineExtraHeight = 0;

                        // prepare next text section to accommodate the distributed (remaining) text
                        formattedTextSection = createFormattedTextSectionWithFontAndPosition(xhtmlElement, m_formattingState.TextFont, m_evolvingLineRunLength, m_formattingState.Subscript - m_formattingState.Superscript);
                    }
                }

                auto lastTextSection = (m_content.size() > 0 ? std::dynamic_pointer_cast<tgui::FormattedTextSection>(m_content.back()) : nullptr);
                if (lastTextSection != nullptr && lastTextSection->getString().length() == 0)
                    m_content.pop_back();

                // process remaining text
                if (remainingText.size() > 0)
                {
                    formattedTextSection->setString(remainingText);
                    auto runLengt = Text::getLineWidth(formattedTextSection->getString(), m_formattingState.TextFont, static_cast<unsigned int>(m_formattingState.TextHeight + 0.49f));
                    formattedTextSection->setRunLength(runLengt + 0.49f);
                    m_evolvingLineRunLength += runLengt;
                    formattedTextSection->setColor(m_formattingState.ForeColor);
                    formattedTextSection->setStyle(m_formattingState.Style);
                }

                predecessorWasStructuringBlockElement = false;
                parentIsTextBlockElement = false;
            }

            m_occupiedLayoutSize.x = std::max(m_occupiedLayoutSize.x, formattedTextSection->getRenderLeft());
            m_occupiedLayoutSize.y = formattedTextSection->getRenderTop();
            auto formattedElement = std::static_pointer_cast<FormattedElement>(formattedTextSection);
            m_content.push_back(formattedElement);

            lastchildWasRunLengtElement = true;
        }

        if (typeName == XhtmlElementType::Image)
        {
            auto formattedImage = std::make_shared<FormattedImage>();
            formattedImage->setContentOrigin(xhtmlElement);

            Vector2u            availSize(static_cast<unsigned int>(m_formattingState.TextHeight + 0.49f), static_cast<unsigned int>(m_formattingState.TextHeight + 0.49f));
            Vector2u            phsicSize = availSize;
            Vector2u            logicSize = availSize;
            XhtmlAttribute::Ptr attribute;

            if ((attribute = xhtmlElement->getAttribute(U"src")) != nullptr && attribute->getValue().size() > 0)
            {
                std::hash<std::u32string> hashString;
                auto hash = hashString(attribute->getValue().c_str());

                auto iterator = m_textures.find(hash);
                if (iterator != m_textures.end())
                {
                    const Texture& texture = iterator->second;
                    auto  size = texture.getImageSize();
                    if (size != Vector2u{})
                    {
                        phsicSize = size;
                        logicSize = size;
                        formattedImage->setTexture(texture);
                        formattedImage->setPhysicalSize(phsicSize);
                    }
                    else
                        std::cerr << "FormattedTextXhtmlDocument::layout -> Unable to determine physical size from texture!";
                }
                else
                {
                    try
                    {
                        tgui::String uri(attribute->getValue());
                        if (!ext::String::containsIgnoreCase(uri, U"data:"))
                        {
                            if (ext::String::containsIgnoreCase(uri, U"file://"))
                                uri.replace(U"file://", U"");
                        }
                        auto textureWrapper = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Texture, uri);
                        const Texture& texture = textureWrapper.getTexture();
                        auto  size = texture.getImageSize();
                        if (size != Vector2u{})
                        {
                            phsicSize = size;
                            logicSize = size;
                            formattedImage->setTexture(texture);
                            formattedImage->setPhysicalSize(phsicSize);

                            m_textures.insert(std::pair<size_t, Texture>(hash, texture));
                        }
                        else
                            std::cerr << "FormattedTextXhtmlDocument::layout -> Unable to determine physical size from texture!";
                    }
                    catch (const Exception&)
                    {
                        std::cerr << "FormattedTextXhtmlDocument::layout -> Unable to load the texture!";
                    }
                }
            }

            if ((attribute = xhtmlElement->getAttribute(U"alt")) != nullptr)
                formattedImage->setAltText(attribute->getValue());
            if ((attribute = xhtmlElement->getAttribute(U"width")) != nullptr)
                logicSize.x = ext::String::toULong(attribute->getValue().c_str(), 10);
            if ((attribute = xhtmlElement->getAttribute(U"height")) != nullptr)
                logicSize.y = ext::String::toULong(attribute->getValue().c_str(), 10);
            formattedImage->setLogicalSize(logicSize);

            m_evolvingLineRunLength += m_formattingState.TextHeight / 8;

            if (m_evolvingLayoutArea.width - m_evolvingLineRunLength < logicSize.x)
            {
                m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_formattingState.TextHeight + m_formattingState.TextHeight / 4;

                // act like a "\r\n"
                m_evolvingLineRunLength = m_formattingState.TextHeight / 8;
                m_evolvingLineExtraHeight = 0;

                if (m_formattingState.TextHeight + m_evolvingLineExtraHeight < logicSize.y)
                {
                    // line hight has to be adjusted
                    float additionalExtraHeight = logicSize.y - (m_formattingState.TextHeight + m_evolvingLineExtraHeight);
                    m_evolvingLineExtraHeight += additionalExtraHeight;
                }
            }
            else
            {
                float additionalExtraHeight = 0.0f;
                float renderReferenceLine = m_evolvingLayoutArea.top + m_formattingState.TextHeight + m_evolvingLineExtraHeight;
                if (m_formattingState.TextHeight + m_evolvingLineExtraHeight < logicSize.y)
                {
                    // line hight has to be adjusted
                    additionalExtraHeight = logicSize.y - (m_formattingState.TextHeight + m_evolvingLineExtraHeight);
                    m_evolvingLineExtraHeight += additionalExtraHeight;
                }

                // fix y position for previous formatted elements in this line
                for(auto rit = m_content.rbegin(); rit != m_content.rend(); ++rit)
                {
                    float oldReferenceLine = rit->get()->getRenderRefLine();
                    if (oldReferenceLine == renderReferenceLine)
                        rit->get()->setRenderTop(rit->get()->getRenderTop() + additionalExtraHeight);
                    else
                        break;
                }
            }

            formattedImage->setRenderLeftTop(m_evolvingLayoutArea.getPosition(), m_evolvingLineRunLength);
            formattedImage->setRenderRightBottom(Vector2f(formattedImage->getRenderLeft() + m_evolvingLineRunLength + logicSize.x, m_evolvingLayoutArea.top + m_formattingState.TextHeight + m_evolvingLineExtraHeight));
            m_content.push_back(formattedImage);
            m_evolvingLineRunLength += logicSize.x + m_formattingState.TextHeight / 8;
        }
    }
}
