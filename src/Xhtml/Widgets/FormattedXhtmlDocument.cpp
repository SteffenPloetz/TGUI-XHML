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

#include "TGUI/Xhtml/StringHelper.hpp"
#include "TGUI/Xhtml/Widgets/FormattedXhtmlDocument.hpp"

namespace tgui  { namespace xhtml
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FormattedXhtmlDocument::FormattedXhtmlDocument()
        : FormattedDocument(), m_textures(), m_content(), m_rootElement(), m_defaultTextSize(14.0f),
          m_defaultForeColor(Color(0, 0, 0)), m_defaultOpacity(1), m_defaultFont(nullptr), m_availableClientSize(0.0f, 0.0f),
          m_occupiedLayoutSize(0.0f, 0.0f), m_evolvingLayoutArea(0.0f, 0.0f, 0.0f, 0.0f),
          m_evolvingLineExtraHeight(0.0f), m_evolvingLineRunLength(0.0f), m_preformattedTextFlagCnt(0), m_formattingState(Color(0, 0, 0)),
          m_listPadding(30), m_backPadding(4)
    {
        m_rootElement = XhtmlElement::createHtml(nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlContainerElement::Ptr FormattedXhtmlDocument::getHeadElement() const
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

    XhtmlContainerElement::Ptr FormattedXhtmlDocument::getBodyElement() const
    {
        for (size_t index = 0; index < m_rootElement->countChildren(); index++)
        {
            auto element = m_rootElement->getChild(index);
            if (strncmp(element->getTypeName(), XhtmlElementType::Body, strlen(XhtmlElementType::Body)) == 0)
                return std::dynamic_pointer_cast<XhtmlContainerElement>(element);
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlStyle::Ptr FormattedXhtmlDocument::getStyleElement() const
    {
        auto headElement = getHeadElement();
        if (headElement != nullptr)
            return std::dynamic_pointer_cast<XhtmlStyle>(headElement->getFirstChild(XhtmlElementType::Style));
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<XhtmlStyleEntry::Ptr> FormattedXhtmlDocument::getApplicableStyleElements(XhtmlElement::Ptr xhtmlElement)
    {
        std::vector<XhtmlStyleEntry::Ptr> styleEntries;
        auto styleElement = getStyleElement();
        if (styleElement)
        {
            auto classNames = StringEx::split(xhtmlElement->getClassNames(), U' ', true);
            for (auto className : classNames)
            {
                auto globalStyleEntry = styleElement->getEntry(xhtmlElement->getTypeName(), className);
                if (globalStyleEntry)
                    styleEntries.push_back(globalStyleEntry);
            }
        }
        auto xhtmlStyleableElement = std::dynamic_pointer_cast<XhtmlStyleableInterface>(xhtmlElement);
        if (xhtmlStyleableElement)
        {
            auto localStyleEntry = xhtmlStyleableElement->getStyleEntry();
            if (localStyleEntry)
                styleEntries.push_back(localStyleEntry);
        }

        return styleEntries;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedXhtmlDocument::applyStyleEntriesToFormattingState(std::vector<XhtmlStyleEntry::Ptr> styleEntries,
        const FormattedDocument::FontCollection& fontCollection, StyleCategoryFlags categories)
    {
        for (auto styleEntry : styleEntries)
        {
            auto styleEntryFlags = styleEntry->getStyleEntryFlags();
            if ((categories & StyleCategoryFlags::ColorsAndOpacity) == StyleCategoryFlags::ColorsAndOpacity)
            {
                if ((styleEntryFlags & StyleEntryFlags::ForeColor) == StyleEntryFlags::ForeColor)
                    m_formattingState.ForeColor = styleEntry->getColor();
                if ((styleEntryFlags & StyleEntryFlags::BackColor) == StyleEntryFlags::BackColor)
                    m_formattingState.BackColor = styleEntry->getBackgroundColor();
                if ((styleEntryFlags & StyleEntryFlags::Opacity) == StyleEntryFlags::Opacity)
                    m_formattingState.Opacity = styleEntry->getOpacity();
            }
            if ((categories & StyleCategoryFlags::ColorsAndOpacity) == StyleCategoryFlags::ColorsAndOpacity)
            {
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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedXhtmlDocument::applyStyleEntriesToFormattedElement(FormattedElement::Ptr formattedElement,
        const std::vector<XhtmlStyleEntry::Ptr> styleEntries,
        const FormattedDocument::FontCollection& fontCollection, StyleCategoryFlags categories)
    {

        for (auto styleEntry : styleEntries)
        {
            auto styleEntryFlags = styleEntry->getStyleEntryFlags();
            if ((categories & StyleCategoryFlags::BackColor) == StyleCategoryFlags::BackColor)
            {
                if ((styleEntryFlags & StyleEntryFlags::BackColor) == StyleEntryFlags::BackColor)
                    formattedElement->setBackgroundColor(styleEntry->getBackgroundColor());
            }
            if ((categories & StyleCategoryFlags::Opacity) == StyleCategoryFlags::Opacity)
            {
                if ((styleEntryFlags & StyleEntryFlags::Opacity) == StyleEntryFlags::Opacity)
                    formattedElement->setOpacity(styleEntry->getOpacity());
            }

            FormattedRectangle::Ptr formattedRect = std::dynamic_pointer_cast<FormattedRectangle>(formattedElement);
            if (formattedRect == nullptr)
                continue;

            if ((categories & StyleCategoryFlags::BorderStyle) == StyleCategoryFlags::BorderStyle &&
                !styleEntry->getBorderStyle().isNoneOrHidden())
            {
                if ((styleEntryFlags & StyleEntryFlags::BorderWidth) == StyleEntryFlags::BorderWidth)
                    formattedRect->setBoderStyle(styleEntry->getBorderStyle());
            }
            if ((categories & StyleCategoryFlags::BorderWidth) == StyleCategoryFlags::BorderWidth &&
                !styleEntry->getBorderWidth().isEmpty(m_availableClientSize))
            {
                if ((styleEntryFlags & StyleEntryFlags::BorderWidth) == StyleEntryFlags::BorderWidth)
                    formattedRect->setBoderWidth(styleEntry->getBorderWidth());
            }
            if ((categories & StyleCategoryFlags::BorderColor) == StyleCategoryFlags::BorderColor)
            {
                if ((styleEntryFlags & StyleEntryFlags::BorderColor) == StyleEntryFlags::BorderColor)
                    formattedRect->setBorderColor(styleEntry->getBorderColor());
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FormattedRectangle::Ptr FormattedXhtmlDocument::createFormattedRectangleWithPosition(XhtmlElement::Ptr xhtmlElement, bool applyLineRunLength)
    {
        auto formattedRectangle = std::make_shared<FormattedRectangle>();
        formattedRectangle->setContentOrigin(xhtmlElement);

        auto leftTop = Vector2f(m_evolvingLayoutArea.left,
                                m_evolvingLineExtraHeight + m_evolvingLayoutArea.top);
        formattedRectangle->setLayoutLeftTop(leftTop, (applyLineRunLength ? m_evolvingLineRunLength : 0));
        auto rightBottom = Vector2f(formattedRectangle->getLayoutLeft(),
                                    m_evolvingLineExtraHeight + m_evolvingLayoutArea.top + m_formattingState.TextHeight);
        formattedRectangle->setLayoutRightBottom(rightBottom);
        formattedRectangle->setBackgroundColor(Color(255, 255, 255, 0));

        return formattedRectangle;
    }

    FormattedLink::Ptr FormattedXhtmlDocument::createFormattedLinkWithPosition(XhtmlElement::Ptr xhtmlElement, bool applyLineRunLength)
    {
        auto formattedRectangle = std::make_shared<FormattedLink>();
        formattedRectangle->setContentOrigin(xhtmlElement);

        auto leftTop = Vector2f(m_evolvingLayoutArea.left,
                                m_evolvingLineExtraHeight + m_evolvingLayoutArea.top);
        formattedRectangle->setLayoutLeftTop(leftTop, (applyLineRunLength ? m_evolvingLineRunLength : 0));
        auto rightBottom = Vector2f(formattedRectangle->getLayoutLeft(),
                                    m_evolvingLineExtraHeight + m_evolvingLayoutArea.top + m_formattingState.TextHeight);
        formattedRectangle->setLayoutRightBottom(rightBottom);
        formattedRectangle->setBackgroundColor(Color(255, 255, 255, 0));

        return formattedRectangle;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FormattedImage::Ptr FormattedXhtmlDocument::createFormattedImageWithPosition(XhtmlElement::Ptr xhtmlElement)
    {
        auto formattedImage = std::make_shared<FormattedImage>();
        formattedImage->setContentOrigin(xhtmlElement);

        auto leftTop = Vector2f(m_evolvingLayoutArea.left,
                                m_evolvingLineExtraHeight + m_evolvingLayoutArea.top);
        formattedImage->setLayoutLeftTop(leftTop);
        auto rightBottom = Vector2f(formattedImage->getLayoutLeft(),
                                    m_evolvingLineExtraHeight + m_evolvingLayoutArea.top + m_formattingState.TextHeight);
        formattedImage->setLayoutRightBottom(rightBottom);
        formattedImage->setBackgroundColor(Color(255, 255, 255, 0));

        return formattedImage;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FormattedTextSection::Ptr FormattedXhtmlDocument::createFormattedTextSectionWithFontAndPosition(XhtmlElement::Ptr xhtmlElement,
        Font font, float indentOffset, float superscriptOrSubsciptTextHeightReduction)
    {
        auto formattedTextSection = std::make_shared<FormattedTextSection>(m_formattingState.ForeColor, m_formattingState.Style);
        formattedTextSection->setContentOrigin(xhtmlElement);
        formattedTextSection->setFont(font);
        formattedTextSection->setOpacity(m_formattingState.Opacity);

        auto leftTop = Vector2f(m_evolvingLayoutArea.left,
                                m_evolvingLineExtraHeight + m_evolvingLayoutArea.top);
        formattedTextSection->setLayoutLeftTop(leftTop, indentOffset, std::max(0.0f, superscriptOrSubsciptTextHeightReduction));
        // for superscript / subscript - the text height is already adjusted
        float unscriptedTextHeight = m_formattingState.TextHeight + std::abs(superscriptOrSubsciptTextHeightReduction);
        auto rightBottom = Vector2f(formattedTextSection->getLayoutLeft(),
                                    m_evolvingLineExtraHeight + m_evolvingLayoutArea.top + unscriptedTextHeight);
        formattedTextSection->setLayoutRightBottom(rightBottom, 0.0f, std::min(0.0f, superscriptOrSubsciptTextHeightReduction));

        return formattedTextSection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    size_t FormattedXhtmlDocument::calculateAutoLineBreak(const String remainingText, float runLength) const
    {
        // Determine SECURE auto-line-break position: For a mono-space font, the runLength is distributed equally to the characters.
        // For a proportional font, the runLength typically is smaller and the SECURE auto-line-break position is a good starting point.
        size_t delimiterPosition = static_cast<size_t>(remainingText.size() * ((1.0f * (m_evolvingLayoutArea.width - m_evolvingLineRunLength)) / (1.0f * runLength)));
        // The line width calculation is not 100% correct. - So some security space is subtracted.
        delimiterPosition -= (delimiterPosition > 333 ? 8 : (delimiterPosition > 99 ? 6 : (delimiterPosition > 33 ? 4 : (delimiterPosition > 9 ? 2 : (delimiterPosition > 3 ? 1 : 0)))));
        size_t linebreakPosition = remainingText.find_last_of(LinebreakDelimitercharacters, delimiterPosition); // Search only includes characters at or before delimiterPosition.

        auto tryAgain = true;
        while (tryAgain)
        {
            size_t expandedLinebreakPosition = remainingText.find_first_of(LinebreakDelimitercharacters, delimiterPosition + 1);
            if (expandedLinebreakPosition < remainingText.size())
            {
                expandedLinebreakPosition += expandedLinebreakPosition < remainingText.size() - 1 && remainingText[expandedLinebreakPosition] == L'-' ? 1 : 0;
                runLength = Text::getLineWidth(remainingText.substr(0, expandedLinebreakPosition), m_formattingState.TextFont, static_cast<unsigned int>(m_formattingState.TextHeight + 0.49f));
                if (m_evolvingLayoutArea.width - m_evolvingLineRunLength > runLength)
                {
                    delimiterPosition = expandedLinebreakPosition;
                    linebreakPosition = expandedLinebreakPosition;
                }
                else
                    tryAgain = false;
            }
            else
                tryAgain = false;
        }

        return linebreakPosition;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedXhtmlDocument::layout(Vector2f clientSize, float defaultTextSize, Color defaultForeColor,  float defaultOpacity,
                                            const FormattedDocument::FontCollection& fontCollection, bool keepSelection)
    {
        if(!fontCollection.assertValid())
            std::cerr << "Invalid font collection!\n";
        if(!m_defaultFont)
            std::cerr << "Invalid default font!\n";

        bool predecessorElementProvidesExtraSpace = false;
        bool parentElementSuppressesInitialExtraSpace = false;
        bool lastchildAcceptsRunLengtExpansion = false;

        m_availableClientSize = {0.0f, 0.0f};
        m_occupiedLayoutSize = {0.0f, 0.0f};
        m_content.clear();
        m_defaultTextSize = defaultTextSize;
        m_defaultOpacity = defaultOpacity;
        m_defaultForeColor = defaultForeColor;
        m_defaultFont = fontCollection.Sans->Regular;

        m_evolvingLayoutArea = FloatRect(0, 0, clientSize.x, clientSize.y);
        m_formattingState.TextHeight = defaultTextSize;
        m_formattingState.Opacity = defaultOpacity;
        m_formattingState.ForeColor = defaultForeColor;
        m_formattingState.TextFont = fontCollection.Sans->Regular;

        for (size_t index = 0; index < m_rootElement->countChildren(); index++)
            layout(predecessorElementProvidesExtraSpace, parentElementSuppressesInitialExtraSpace, lastchildAcceptsRunLengtExpansion,
                   m_rootElement->getChild(index), fontCollection,keepSelection);
        if (m_content.size() > 0)
        {
            FormattedElement::Ptr formattedElement = m_content[m_content.size() - 1];
            m_occupiedLayoutSize.y += formattedElement->getLayoutSize().y + m_defaultTextSize / 2;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedXhtmlDocument::layout(bool& predecessorElementProvidesExtraSpace, bool parentElementSuppressesInitialExtraSpace,
                                            bool& lastchildAcceptsRunLengtExpansion, XhtmlElement::Ptr xhtmlElement,
                                            const FormattedDocument::FontCollection& fontCollection, bool keepSelection)
    {
        auto typeName = xhtmlElement->getTypeName();
        if (typeName == XhtmlElementType::Head)
            return;

        std::shared_ptr<FormattedElement> currentFormattedElement = nullptr;

        // Block elements always need to start a new line and to lock the current line.
        if (typeName == XhtmlElementType::Body          ||
            typeName == XhtmlElementType::H1            || typeName == XhtmlElementType::H2 || typeName == XhtmlElementType::H3 ||
            typeName == XhtmlElementType::H4            || typeName == XhtmlElementType::H5 || typeName == XhtmlElementType::H6 ||
            typeName == XhtmlElementType::Emphasized    || typeName == XhtmlElementType::Italic ||
            typeName == XhtmlElementType::Strong        || typeName == XhtmlElementType::Bold ||
            typeName == XhtmlElementType::Underline     ||
            typeName == XhtmlElementType::Superscript   || typeName == XhtmlElementType::Subscript    ||
            typeName == XhtmlElementType::UnorderedList || typeName == XhtmlElementType::OrderedList  ||
            typeName == XhtmlElementType::ListItem      ||
            typeName == XhtmlElementType::Span          || typeName == XhtmlElementType::Anchor       ||
            typeName == XhtmlElementType::Division      || typeName == XhtmlElementType::Preformatted || typeName == XhtmlElementType::Code ||
            typeName == XhtmlElementType::Paragraph     || typeName == XhtmlElementType::Image)
        {
            // prepare rect section to accommodate the block

            FormattingState cachedState(m_formattingState);
            auto styleEntries = getApplicableStyleElements(xhtmlElement);
            auto xhtmlStyleableElement = std::dynamic_pointer_cast<XhtmlStyleableInterface>(xhtmlElement);

            bool currentIsInitialExtraSpaceSuppressingElement = false;

            // -------------------------------
            // Apply predefined font style
            // -------------------------------
            if      (typeName == XhtmlElementType::H1 || typeName == XhtmlElementType::H2 || typeName == XhtmlElementType::H3)
            {
                m_formattingState.TextFont = fontCollection.Sans->Bold;
                m_formattingState.TextHeight = (typeName == XhtmlElementType::H1 ? 28 * m_defaultTextSize / 14 :
                                               (typeName == XhtmlElementType::H2 ? 20 * m_defaultTextSize / 14 :
                                                                                   16 * m_defaultTextSize / 14));
                m_formattingState.ForeColor = m_defaultForeColor;
            }
            else if (typeName == XhtmlElementType::H4 || typeName == XhtmlElementType::H5 || typeName == XhtmlElementType::H6)
            {
                m_formattingState.TextFont = fontCollection.Sans->Bold;
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
                m_formattingState.TextHeight  = m_formattingState.TextHeight - m_formattingState.Superscript;
            }
            else if (typeName == XhtmlElementType::Subscript)
            {
                m_formattingState.Subscript  = m_formattingState.TextHeight / 3;
                m_formattingState.TextHeight = m_formattingState.TextHeight - m_formattingState.Subscript;
            }

            // -------------------------------
            // Apply styled font style
            // -------------------------------
            if (xhtmlStyleableElement)
            {
                applyStyleEntriesToFormattingState(styleEntries, fontCollection, StyleCategoryFlags::ColorsAndOpacity | StyleCategoryFlags::Fonts);
            }

            // -------------------------------
            // Create formatted elements
            // And apply leading offsets
            // -------------------------------
            if (typeName == XhtmlElementType::Body)
            {
                // Can be an anchor or styled, but doesn't represent text/image ==> FormattedRectangle

                // -- Prepare Y
                m_evolvingLineExtraHeight = 0;

                // -- Prepare X

                // -- Create element
                currentFormattedElement = createFormattedRectangleWithPosition(xhtmlElement);
                m_content.push_back(currentFormattedElement);
                if (xhtmlStyleableElement)
                {
                    auto localStyleEntry = xhtmlStyleableElement->getStyleEntry();
                    if (!localStyleEntry)
                    {
                        localStyleEntry = std::make_shared< XhtmlStyleEntry>();
                        xhtmlElement->addAttribute(localStyleEntry);
                    }
                }

                // -- Act like a "\r"
                m_evolvingLineRunLength = 0;

                // -- Set flags
                currentIsInitialExtraSpaceSuppressingElement = true; // First child element can be placed without preceeding extra space.
            }
            else if (typeName == XhtmlElementType::H1 || typeName == XhtmlElementType::H2 || typeName == XhtmlElementType::H3 ||
                     typeName == XhtmlElementType::H4 || typeName == XhtmlElementType::H5 || typeName == XhtmlElementType::H6)
            {
                // Can be an anchor or styled, but doesn't represent text/image ==> FormattedRectangle

                // -- Prepare Y
                if (predecessorElementProvidesExtraSpace || parentElementSuppressesInitialExtraSpace) // XXX
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight;
                else
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize + m_defaultTextSize + m_defaultTextSize / 2;
                m_evolvingLineExtraHeight = 0;

                // -- Prepare X

                // -- Create element
                currentFormattedElement = createFormattedRectangleWithPosition(xhtmlElement);
                m_content.push_back(currentFormattedElement);

                // -- Act like a "\r"
                m_evolvingLineRunLength = 0;

                // -- Set flags
            }
            else if (typeName == XhtmlElementType::UnorderedList || typeName == XhtmlElementType::OrderedList)
            {
                // Can be an anchor or styled, but doesn't represent text/image ==> FormattedRectangle

                // -- Prepare Y
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
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight;
                /* [4] */ else
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize + m_defaultTextSize / 4;
                m_evolvingLineExtraHeight = 0;

                // -- Prepare X

                auto newListMetric = std::make_shared<FormattedDocument::ListData>();
                newListMetric->Ordered = (typeName == XhtmlElementType::OrderedList);
                newListMetric->ActualItemIndex = 0;
                newListMetric->ItemType = ListItemType::InheritOrDefault;
                m_formattingState.ListMetrics.push_back(newListMetric);

                // -- Create element
                currentFormattedElement = createFormattedRectangleWithPosition(xhtmlElement);
                m_content.push_back(currentFormattedElement);

                // -- Act like a "\r"
                m_evolvingLineRunLength = 0;

                // -- Set flags
                currentIsInitialExtraSpaceSuppressingElement = true; // First child element can be placed without preceeding extra space.
            }
            else if (typeName == XhtmlElementType::ListItem)
            {
                // Can be an anchor or styled and also represents text/image ==> FormattedTextSection

                // -- Prepare Y
                m_evolvingLineExtraHeight = 0;

                auto xhtmlListItem = std::dynamic_pointer_cast<XhtmlListItem>(xhtmlStyleableElement);
                FormattedDocument::ListData::Ptr listMetrics = m_formattingState.ListMetrics.back();
                listMetrics->ActualItemIndex++;
                if (listMetrics->ActualItemIndex == 1)
                {
                    // Attention: Asymmetric! Reverse inflating will not happen in matching XhtmlType::ListItem end, but in parent XhtmlType::List end!
                    inflate(m_evolvingLayoutArea, -m_listPadding, 0, 0, 0);
                }

                // -- Prepare X
                String bullet = listMetrics->calculateBullet(m_formattingState.ListMetrics.size());
                tgui::Font font = (listMetrics->Ordered ? m_formattingState.TextFont : fontCollection.Mono->Regular);
                auto runLengt = Text::getLineWidth(bullet, font, static_cast<unsigned int>(m_formattingState.TextHeight + 0.49f));

                auto formattedTextSection = createFormattedTextSectionWithFontAndPosition(xhtmlElement, font, -(m_backPadding + runLengt),
                                                                                          m_formattingState.Subscript - m_formattingState.Superscript);
                formattedTextSection->setString(bullet);
                formattedTextSection->setRunLength(runLengt + 0.49f);
                formattedTextSection->setColor(m_formattingState.ForeColor);
                formattedTextSection->setStyle(m_formattingState.Style);

                // -- Create element
                currentFormattedElement = std::static_pointer_cast<FormattedElement>(formattedTextSection);
                m_content.push_back(currentFormattedElement);

                // -- Act like a "\r"
                m_evolvingLineRunLength = 0;

                // -- Set flags
                currentIsInitialExtraSpaceSuppressingElement = true; // First child element can be placed without preceeding extra space.
            }
            else if (typeName == XhtmlElementType::Span || typeName == XhtmlElementType::Anchor)
            {
                // Can be an anchor or styled, but doesn't represent text/image ==> FormattedRectangle

                // -- Prepare Y

                // -- Prepare X

                // -- Create element
                FormattedLink::Ptr formattedLink = createFormattedLinkWithPosition(xhtmlElement, true);
                currentFormattedElement = formattedLink;
                m_content.push_back(currentFormattedElement);

                if (typeName == XhtmlElementType::Anchor)
                {
                    auto href = xhtmlElement->getAttribute(U"href");
                    if (href != nullptr && href->getValue().length() > 2)
                        formattedLink->setHref(href->getValue());

                    auto body = getBodyElement();
                    if (body != nullptr)
                    {
                        auto link = body->getAttribute(U"link");
                        if (link != nullptr && link->getValue().length() > 2)
                            formattedLink->setLinkColor(Color(link->getValue()));
                        auto alink = body->getAttribute(U"alink");
                        if (alink != nullptr && alink->getValue().length() > 2)
                            formattedLink->setActiveColor(Color(alink->getValue()));
                        auto vlink = body->getAttribute(U"vlink");
                        if (vlink != nullptr && vlink->getValue().length() > 2)
                            formattedLink->setVisitedColor(Color(vlink->getValue()));
                    }
                }

                // -- Act NOT like a "\r"

                // -- Set flags
            }
            else if (typeName == XhtmlElementType::Division || typeName == XhtmlElementType::Preformatted || typeName == XhtmlElementType::Code)
            {
                // Can be an anchor or styled, but doesn't represent text/image ==> FormattedRectangle

                // -- Prepare Y
                if (predecessorElementProvidesExtraSpace || parentElementSuppressesInitialExtraSpace)
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight;
                else
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_formattingState.TextHeight + m_formattingState.TextHeight / 4;
                m_evolvingLineExtraHeight = 0;

                if (typeName == XhtmlElementType::Preformatted)
                    m_preformattedTextFlagCnt++;
                else if (typeName == XhtmlElementType::Code)
                {
                    m_formattingState.TextFont    = fontCollection.Mono->Regular;
                    m_formattingState.TextHeight -= m_formattingState.TextHeight / 4;
                }

                // reduce one line break, if line break is direct predecessor of '<code>'
                // (typically '<code>' is inside '<pre>' and '<pre>' forces linebrak fom source code to visual tree)
                if (typeName == XhtmlElementType::Code)
                {
                    auto sibling = XhtmlElement::getPreviousSibling(xhtmlElement);
                    if (sibling != nullptr && sibling->getTypeName() == XhtmlElementType::Break)
                        m_evolvingLayoutArea.top -= m_formattingState.TextHeight + m_formattingState.TextHeight / 4;
                }

                // -- Prepare X

                // -- Create element
                currentFormattedElement = createFormattedRectangleWithPosition(xhtmlElement);
                m_content.push_back(currentFormattedElement);

                // -- Act like a "\r"
                m_evolvingLineRunLength = 0;

                // -- Set flags
                currentIsInitialExtraSpaceSuppressingElement = true; // First child element can be placed without preceeding extra space.
            }
            else if (typeName == XhtmlElementType::Paragraph)
            {
                // Can be an anchor or styled, but doesn't represent text/image ==> FormattedRectangle

                // -- Prepare Y
                if (predecessorElementProvidesExtraSpace || parentElementSuppressesInitialExtraSpace)
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize;
                else
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize + m_defaultTextSize / 2 + m_defaultTextSize;
                m_evolvingLineExtraHeight = 0;

                // -- Prepare X

                // -- Create element
                currentFormattedElement = createFormattedRectangleWithPosition(xhtmlElement);
                m_content.push_back(currentFormattedElement);

                // -- Act like a "\r"
                m_evolvingLineRunLength = 0;

                // -- Set flags
                currentIsInitialExtraSpaceSuppressingElement = true; // First child element can be placed without preceeding extra space.
            }
            else if (typeName == XhtmlElementType::Image)
            {
                // Can be an anchor or styled and also represents text/image ==> FormattedTextSection

                // -- Prepare Y

                // -- Prepare X

                // -- Create element
                currentFormattedElement = createFormattedImageWithPosition(xhtmlElement);
                m_content.push_back(currentFormattedElement);

                // -- Act NOT like a "\r"

                // -- Set flags
            }

            // -------------------------------
            // Initialize backgound and border
            // -------------------------------
            if (typeName == XhtmlElementType::Body          ||
                typeName == XhtmlElementType::H1            || typeName == XhtmlElementType::H2            || typeName == XhtmlElementType::H3 ||
                typeName == XhtmlElementType::H4            || typeName == XhtmlElementType::H5            || typeName == XhtmlElementType::H6 ||
                typeName == XhtmlElementType::UnorderedList || typeName == XhtmlElementType::OrderedList   ||
                typeName == XhtmlElementType::Span          || typeName == XhtmlElementType::Anchor        ||
                typeName == XhtmlElementType::Division      || typeName == XhtmlElementType::Preformatted  || typeName == XhtmlElementType::Code ||
                typeName == XhtmlElementType::Paragraph     || typeName == XhtmlElementType::Image)
            {
                FormattedRectangle::Ptr formattedRectSection = std::dynamic_pointer_cast<FormattedRectangle>(currentFormattedElement);
                if (formattedRectSection)
                {
                    applyStyleEntriesToFormattedElement(formattedRectSection, styleEntries, fontCollection,
                        StyleCategoryFlags::BackColor | StyleCategoryFlags::Opacity |
                        StyleCategoryFlags::BorderStyle | StyleCategoryFlags::BorderWidth | StyleCategoryFlags::BorderColor);
                }
            }

            // -------------------------------
            // Apply leading margin
            // -------------------------------
            if (xhtmlStyleableElement)
            {
                FormattedRectangle::Ptr formattedRectSection = std::dynamic_pointer_cast<FormattedRectangle>(currentFormattedElement);
                for (auto styleEntry : styleEntries)
                {
                    if ((styleEntry->getStyleEntryFlags() & StyleEntryFlags::Margin) == StyleEntryFlags::Margin)
                    {
                        auto margin = styleEntry->getMargin().toPixel(m_availableClientSize);
                        inflate(m_evolvingLayoutArea, -margin.left, -margin.top, -margin.right, 0);
                        if (formattedRectSection)
                            formattedRectSection->setMargin(margin);
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
            bool loopinternalPredecessorElementProvidesExtraSpace = false; // XXX (typeName == XhtmlElementType::Body);
            bool lastchildAcceptsRunLengtExpansion = false;
            for (size_t index = 0; xhtmlElement->isContainer() && index < xhtmlElement->countChildren(); index++)
                layout(loopinternalPredecessorElementProvidesExtraSpace, index == 0 && currentIsInitialExtraSpaceSuppressingElement, lastchildAcceptsRunLengtExpansion,
                    xhtmlElement->getChild(index), fontCollection, keepSelection);

            float bottomExtraSpace = 0.0f;

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
                        bottomExtraSpace += padding.bottom;
                    }
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
                        bottomExtraSpace += margin.bottom;
                    }
                }
            }

            // -------------------------------
            // Finalize background
            // -------------------------------
            if (typeName == XhtmlElementType::Body          || typeName == XhtmlElementType::UnorderedList || typeName == XhtmlElementType::OrderedList ||
                typeName == XhtmlElementType::H1            || typeName == XhtmlElementType::H2            || typeName == XhtmlElementType::H3 ||
                typeName == XhtmlElementType::H4            || typeName == XhtmlElementType::H5            || typeName == XhtmlElementType::H6 ||
                typeName == XhtmlElementType::UnorderedList || typeName == XhtmlElementType::OrderedList   ||
                typeName == XhtmlElementType::Span          || typeName == XhtmlElementType::Anchor        ||
                typeName == XhtmlElementType::Division      || typeName == XhtmlElementType::Preformatted  || typeName == XhtmlElementType::Code ||
                typeName == XhtmlElementType::Paragraph)    // typeName == XhtmlElementType::Image)
            {
                if (currentFormattedElement != nullptr)
                {
                    // Recover the last child (if any) or itself. If there is a child - if child is text section a special treatment is required.
                    auto lastTextSection = std::dynamic_pointer_cast<FormattedTextSection>(m_content.back());
                    if (lastTextSection)
                    {
                        // typically the the text sections are always open to add new charachters (in other words: not finalized with line break / carriage return)
                        // which implies, that m_evolvingLayoutArea.top points still to the top of the  text sections, while they might have a heigth
                        if (typeName == XhtmlElementType::Span || typeName == XhtmlElementType::Anchor)
                            currentFormattedElement->setLayoutRightBottom(Vector2f(m_evolvingLayoutArea.left + m_evolvingLineRunLength, lastTextSection->getLayoutRefLine() + m_formattingState.TextHeight / 4 + bottomExtraSpace));
                        else
                            currentFormattedElement->setLayoutRightBottom(Vector2f(right(m_evolvingLayoutArea), lastTextSection->getLayoutRefLine() + m_formattingState.TextHeight / 4 + bottomExtraSpace));
                    }
                    else
                        currentFormattedElement->setLayoutRightBottom(Vector2f(right(m_evolvingLayoutArea), m_evolvingLayoutArea.top));
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
                if (lastchildAcceptsRunLengtExpansion)
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight;
                else
                    m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize;

                if (typeName == XhtmlElementType::Preformatted)
                    m_preformattedTextFlagCnt--;
                if (typeName == XhtmlElementType::Preformatted && m_preformattedTextFlagCnt == 0)
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
            predecessorElementProvidesExtraSpace = false;

            if (typeName == XhtmlElementType::H1 || typeName == XhtmlElementType::H2 || typeName == XhtmlElementType::H3 ||
                typeName == XhtmlElementType::H4 || typeName == XhtmlElementType::H5 || typeName == XhtmlElementType::H6)
            {
                predecessorElementProvidesExtraSpace = true;
                lastchildAcceptsRunLengtExpansion = false;
            }
            else if (typeName == XhtmlElementType::UnorderedList || typeName == XhtmlElementType::OrderedList)
            {
                predecessorElementProvidesExtraSpace = true;
                lastchildAcceptsRunLengtExpansion = false;
            }
            else if (typeName == XhtmlElementType::Division || typeName == XhtmlElementType::Preformatted || typeName == XhtmlElementType::Code)
                lastchildAcceptsRunLengtExpansion = false;
            else if (typeName == XhtmlElementType::Paragraph)
            {
                predecessorElementProvidesExtraSpace = true;
                lastchildAcceptsRunLengtExpansion = false;
            }

            m_formattingState = cachedState;
        }

        if (typeName == XhtmlElementType::Break || typeName == XhtmlElementType::Text)
        {
            // prepare text section to accommodate the text
            auto formattedTextSection = createFormattedTextSectionWithFontAndPosition(xhtmlElement, m_formattingState.TextFont, m_evolvingLineRunLength,
                                                                                      m_formattingState.Subscript - m_formattingState.Superscript);

            if (typeName == XhtmlElementType::Break)
            {
                m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_formattingState.TextHeight + m_formattingState.TextHeight / 4;

                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;

                predecessorElementProvidesExtraSpace = false;
                parentElementSuppressesInitialExtraSpace = false;
            }
            else if (typeName == XhtmlElementType::Text)
            {
                String remainingText = static_cast<XhtmlInnerText*>(xhtmlElement.get())->getText();
                auto   runLength     = Text::getLineWidth(remainingText, m_formattingState.TextFont, static_cast<unsigned int>(m_formattingState.TextHeight + 0.49f));
                size_t formerCharCnt = remainingText.size() + 1;

                // distribute, if text exceeds available width
                while ((m_preformattedTextFlagCnt > 0 && remainingText.contains('\r')) ||   // preformatted text has dedicated line bereak instructions
                       (formerCharCnt > remainingText.size() &&                             // there must be a chance to distribute the text to multiple lines
                        m_evolvingLayoutArea.width - m_evolvingLineRunLength < runLength && // there must be a need to distribute the text to multiple lines
                        remainingText.size() > 0))                                          // there must be remaining text to distribute to multiple lines
                {
                    formerCharCnt = remainingText.size();

                    size_t linebreakPosition = calculateAutoLineBreak(remainingText, runLength);

                    // The preformatted text might force an earlier line-break, that is situated witin the remaining text before auto-line-break 'linebreakPosition'.
                    if (m_preformattedTextFlagCnt > 0)
                    {
                        auto   substring      = remainingText.substr(0, linebreakPosition);
                        size_t forcedPosition = substring.find('\r');
                        if (forcedPosition != static_cast<size_t>(-1))
                            linebreakPosition = forcedPosition;
                    }
                    // There might be no way to add (any small part of) the remaining text to the end of the current run length (no meaningfulo auto-line-break).
                    if (linebreakPosition == SIZE_MAX)
                    {
                        // for superscript / subscript - the text height is already adjusted
                        float unscriptedTextHeight = m_formattingState.TextHeight + m_formattingState.Subscript + m_formattingState.Superscript;
                        m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + unscriptedTextHeight + unscriptedTextHeight / 4;

                        // act like a "\r\n"
                        m_evolvingLineRunLength = 0;
                        m_evolvingLineExtraHeight = 0;

                        // Since the internal coordinates of formattedTextSection are not in initial state and would falsify the result, reset the internal coordinates!
                        formattedTextSection->setLayoutArea({0.0f, 0.0f});
                        formattedTextSection->setLayoutLeftTop(Vector2f(m_evolvingLayoutArea.left, m_evolvingLineExtraHeight + m_evolvingLayoutArea.top), m_evolvingLineRunLength, m_formattingState.Subscript);
                        formattedTextSection->setLayoutRightBottom(Vector2f(formattedTextSection->getLayoutLeft(), m_evolvingLineExtraHeight + m_evolvingLayoutArea.top + unscriptedTextHeight), 0.0f, -m_formattingState.Superscript);
                        break;
                    }

                    formattedTextSection->setString(remainingText.substr(0, linebreakPosition));
                    runLength = Text::getLineWidth(formattedTextSection->getString(), m_formattingState.TextFont, static_cast<unsigned int>(m_formattingState.TextHeight + 0.49f));
                    formattedTextSection->setRunLength(runLength + 0.45f);
                    m_evolvingLineRunLength += runLength;
                    formattedTextSection->setColor(m_formattingState.ForeColor);
                    formattedTextSection->setStyle(m_formattingState.Style);

                    remainingText = remainingText.substr(linebreakPosition + 1);
                    runLength = Text::getLineWidth(remainingText, m_formattingState.TextFont, static_cast<unsigned int>(m_formattingState.TextHeight + 0.49f));

                    if (remainingText.size() > 0)
                    {
                        m_occupiedLayoutSize.x = std::max(m_occupiedLayoutSize.x, formattedTextSection->getLayoutLeft());
                        m_occupiedLayoutSize.y = formattedTextSection->getLayoutTop();
                        auto formattedElement = std::static_pointer_cast<FormattedElement>(formattedTextSection);
                        m_content.push_back(formattedElement);

                        // for superscript / subscript - the text height is already adjusted
                        float unscriptedTextHeight = m_formattingState.TextHeight + m_formattingState.Subscript + m_formattingState.Superscript;
                        m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + unscriptedTextHeight + unscriptedTextHeight / 4;

                        // act like a "\r\n"
                        m_evolvingLineRunLength = 0;
                        m_evolvingLineExtraHeight = 0;

                        // prepare next text section to accommodate the distributed (remaining) text
                        formattedTextSection = createFormattedTextSectionWithFontAndPosition(xhtmlElement, m_formattingState.TextFont, m_evolvingLineRunLength, m_formattingState.Subscript - m_formattingState.Superscript);
                    }
                }

                auto lastTextSection = (m_content.size() > 0 ? std::dynamic_pointer_cast<FormattedTextSection>(m_content.back()) : nullptr);
                if (lastTextSection != nullptr && lastTextSection->getString().length() == 0)
                    m_content.pop_back();

                // process remaining text
                if (remainingText.size() > 0)
                {
                    formattedTextSection->setString(remainingText);
                    runLength = Text::getLineWidth(formattedTextSection->getString(), m_formattingState.TextFont, static_cast<unsigned int>(m_formattingState.TextHeight + 0.49f));
                    formattedTextSection->setRunLength(runLength + 0.49f);
                    m_evolvingLineRunLength += runLength;
                    formattedTextSection->setColor(m_formattingState.ForeColor);
                    formattedTextSection->setStyle(m_formattingState.Style);
                }

                predecessorElementProvidesExtraSpace = false;
                parentElementSuppressesInitialExtraSpace = false;
            }

            m_occupiedLayoutSize.x = std::max(m_occupiedLayoutSize.x, formattedTextSection->getLayoutLeft());
            m_occupiedLayoutSize.y = formattedTextSection->getLayoutTop();
            auto formattedElement = std::static_pointer_cast<FormattedElement>(formattedTextSection);
            m_content.push_back(formattedElement);

            lastchildAcceptsRunLengtExpansion = true;
        }

        if (typeName == XhtmlElementType::Image)
        {
            FormattedImage::Ptr formattedImage = std::dynamic_pointer_cast<FormattedImage>(currentFormattedElement);
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
                        std::cerr << "FormattedXhtmlDocument::layout -> Unable to determine physical size from texture!";
                }
                else
                {
                    try
                    {
                        tgui::String uri(attribute->getValue());
                        if (!uri.toLower().contains(U"data:"))
                        {
                            if (uri.toLower().contains(U"file://"))
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
                            std::cerr << "FormattedXhtmlDocument::layout -> Unable to determine physical size from texture!";
                    }
                    catch (const Exception&)
                    {
                        std::cerr << "FormattedXhtmlDocument::layout -> Unable to load the texture!";
                    }
                }
            }

            if ((attribute = xhtmlElement->getAttribute(U"alt")) != nullptr)
                formattedImage->setAltText(attribute->getValue());
            if ((attribute = xhtmlElement->getAttribute(U"width")) != nullptr)
                logicSize.x = attribute->getValue().toUInt();
            if ((attribute = xhtmlElement->getAttribute(U"height")) != nullptr)
                logicSize.y = attribute->getValue().toUInt();
            formattedImage->setLogicalSize(logicSize);

            m_evolvingLineRunLength += m_formattingState.TextHeight / 8;

            if (m_evolvingLayoutArea.width - m_evolvingLineRunLength < logicSize.x)
            {
                m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_formattingState.TextHeight + m_formattingState.TextHeight / 4;

                // act like a "\r\n"
                m_evolvingLineRunLength = m_formattingState.TextHeight / 8;
                m_evolvingLineExtraHeight = 0;

                if (m_evolvingLineExtraHeight + m_formattingState.TextHeight < logicSize.y)
                {
                    // line hight has to be adjusted
                    float additionalExtraHeight = logicSize.y - (m_formattingState.TextHeight + m_evolvingLineExtraHeight);
                    m_evolvingLineExtraHeight += additionalExtraHeight;
                }
            }
            else
            {
                float additionalExtraHeight = 0.0f;
                float renderReferenceLine = m_evolvingLineExtraHeight + m_evolvingLayoutArea.top + m_formattingState.TextHeight;
                if (m_formattingState.TextHeight + m_evolvingLineExtraHeight < logicSize.y)
                {
                    // line hight has to be adjusted
                    additionalExtraHeight = logicSize.y - (m_formattingState.TextHeight + m_evolvingLineExtraHeight);
                    m_evolvingLineExtraHeight += additionalExtraHeight;
                }

                // fix y position for previous formatted elements in the same line
                for(auto rit = m_content.rbegin(); rit != m_content.rend(); ++rit)
                {
                    // skip self
                    if (rit == m_content.rbegin())
                        continue;
#if _DEBUG
                    auto formattedTextSection = std::dynamic_pointer_cast<FormattedTextSection>(*rit);
#endif
                    float oldReferenceLine = (*rit)->getLayoutRefLine();
                    auto  loopTypeName = (*rit)->getContentOrigin()->getTypeName();
                    if (oldReferenceLine == renderReferenceLine || loopTypeName == XhtmlElementType::Span)
                    {
                        auto rect = rit->get()->getLayoutArea();
                        rect.top += additionalExtraHeight;
                        rit->get()->setLayoutArea(rect.getPosition(), rect.getSize());
                    }
                    else
                        break;
                }
            }

            formattedImage->setLayoutLeftTop(m_evolvingLayoutArea.getPosition(), m_evolvingLineRunLength);
            formattedImage->setLayoutRightBottom(Vector2f(formattedImage->getLayoutLeft() + m_evolvingLineRunLength + logicSize.x, m_evolvingLineExtraHeight + m_evolvingLayoutArea.top + m_formattingState.TextHeight));
            m_evolvingLineRunLength += logicSize.x + m_formattingState.TextHeight / 8;
        }
    }
} }
