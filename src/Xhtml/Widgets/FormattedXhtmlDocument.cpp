#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <cstdint>

#include <TGUI/Config.hpp>
#include <TGUI/TGUI.hpp>

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
#include "TGUI/Xhtml/Dom/XhtmlParser.hpp"
#include "TGUI/Xhtml/UtfHelper.hpp"

namespace tgui  { namespace xhtml
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FormattedXhtmlDocument::FormattedXhtmlDocument()
        : FormattedDocument(), m_textures(), m_content(), m_rootElement(), m_errorNotifyDlgParent(),
          m_defaultTextSize(14.0f), m_defaultForeColor(Color(0, 0, 0)), m_defaultOpacity(1), m_defaultFont(nullptr),
          m_availableClientSize(0.0f, 0.0f), m_occupiedLayoutSize(0.0f, 0.0f), m_evolvingLayoutArea(0.0f, 0.0f, 0.0f, 0.0f),
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
            for (String& className : classNames)
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
        const std::vector<XhtmlStyleEntry::Ptr> styleEntries, Vector2f parentSize,
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
                    formattedRect->setBoderWidth(styleEntry->getBorderWidth().toPixel(parentSize));
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

        m_availableClientSize = clientSize;
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

    int FormattedXhtmlDocument::loadDocument(const std::string filePath, bool trace)
    {
        int state = 0;

        std::string  errorMessage;
        String fileEncoding;
        String hypertextString = FormattedXhtmlDocument::readXhtmlDocument(filePath, fileEncoding, errorMessage);
        if (hypertextString.size() == 0)
        {
            if (trace)
                std::cerr << "ERROR: " << errorMessage << "\n";
            state = -1;
        }
        else
        {
            auto elements = FormattedXhtmlDocument::parseXhtmlDocument(hypertextString, true, trace);
            if (elements.size() == 0)
                state = -2;
            else
            {
                auto rootHtml = XhtmlElement::getFirstElement(elements, "html");
                auto html = std::dynamic_pointer_cast<XhtmlContainerElement>(rootHtml);
                if (html == nullptr)
                    state = -3;
                else
                    setRootElement(html);
            }
        }

        if (m_errorNotifyDlgParent && state != 0)
        {
            static MessageBox::Ptr errorMessageBox;
            tgui::String message;
            if (state == -1)
                message = "File can not be red:\n";
            else if (state == -2)
                message = "File can not be parsed (has syntax errors or is to complex):\n";
            else
                message = "File doesn't contain an XHTML root element:\n";
            message += '\"' + filePath + '\"';
            if (errorMessageBox)
                errorMessageBox->setText(message);
            else
                errorMessageBox = MessageBox::create("Error loading XML file", message, {"OK"});
            auto parentSize = m_errorNotifyDlgParent->getSize();
            errorMessageBox->setPosition(parentSize.x / 2 - 150, parentSize.y / 2 - 50);
            m_errorNotifyDlgParent->getParentGui()->add(errorMessageBox, U"FormattedXhtmlDocument_ErrorMessageBox");
            auto children = errorMessageBox->getWidgets(); // First child is label, second child id button.
            auto button = std::dynamic_pointer_cast<tgui::Button>(children[children.size() - 1]);
            if (button)
            {
                button->onPress([&] { auto gui = m_errorNotifyDlgParent->getParentGui(); if (gui) { gui->remove(errorMessageBox); } });
            }
        }

        return state;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedXhtmlDocument::calculateTableColumnRequestedSizes(XhtmlElement::Ptr tableElement, FormattedDocument::TableMetric::Ptr tableMetric)
    {
        bool forceNewFreeCellRow = false;
        for (XhtmlElement::Ptr childElement : *(tableElement->getChildren()))
        {
            auto childTypeName = childElement->getTypeName();
            if (childTypeName == XhtmlElementType::TableHead || childTypeName == XhtmlElementType::TableBody || childTypeName == XhtmlElementType::TableFoot)
            {
                if (childTypeName == XhtmlElementType::TableHead)
                    tableMetric->setCurrentRowRange(TableRowRange::Head);
                else if (childTypeName == XhtmlElementType::TableFoot)
                    tableMetric->setCurrentRowRange(TableRowRange::Foot);
                else
                    tableMetric->setCurrentRowRange(TableRowRange::Body);

                forceNewFreeCellRow = true;
                for (XhtmlElement::Ptr grandchildElement : *(childElement->getChildren()))
                {
                    calculateRowRangeColumnRequestedSizes(grandchildElement, tableMetric, forceNewFreeCellRow);
                    forceNewFreeCellRow = false;
                }
                forceNewFreeCellRow = true;
            }
            else
            {
                tableMetric->setCurrentRowRange(TableRowRange::Body);
                calculateRowRangeColumnRequestedSizes(childElement, tableMetric, forceNewFreeCellRow);
                forceNewFreeCellRow = false;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedXhtmlDocument::calculateRowRangeColumnRequestedSizes(XhtmlElement::Ptr xhtmlElement,
        FormattedDocument::TableMetric::Ptr tableMetric, bool forceNewFreeCellRow)
    {
        auto elementTypeName = xhtmlElement->getTypeName();
        if (elementTypeName == XhtmlElementType::TableRow)
        {
            auto tableRowMetric = std::make_shared<FormattedDocument::TableRowMetric>();
            tableMetric->addRowRangeRowMetric(tableMetric->getCurrentRowRange(), tableRowMetric);
            tableRowMetric->setContentOrigin(xhtmlElement);

            for (XhtmlElement::Ptr childElement : *(xhtmlElement->getChildren()))
            {
                auto childTypeName = childElement->getTypeName();
                if (childTypeName == XhtmlElementType::TableHeaderCell || childTypeName == XhtmlElementType::TableDataCell)
                    calculateCellColumnRequestedSize(childElement, tableMetric, tableRowMetric);
            }
        }
        else if (elementTypeName == XhtmlElementType::TableHeaderCell || elementTypeName == XhtmlElementType::TableDataCell)
        {
            // 'Free' cells on table level are added to the last 'free' row of the table's BODY part, if any, or create a new 'free' row in the table's BODY part.
            // 'Free' cells on thead/tbody/tfoot level are added to the last 'free' row of the HED/BODY/FOOT part, if any, or create a new 'free' row in the table's BODY part.
            auto tableRowMetric = tableMetric->getRowRangeTailRowMetric(tableMetric->getCurrentRowRange());
            if ((tableRowMetric && tableRowMetric->getContentOrigin()) || forceNewFreeCellRow)
                tableRowMetric = nullptr;
            if (!tableRowMetric)
            {
                tableRowMetric = std::make_shared<FormattedDocument::TableRowMetric>();
                tableMetric->addRowRangeRowMetric(tableMetric->getCurrentRowRange(), tableRowMetric);
            }

            calculateCellColumnRequestedSize(xhtmlElement, tableMetric, tableRowMetric);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedXhtmlDocument::calculateCellColumnRequestedSize(XhtmlElement::Ptr xhtmlElement, FormattedDocument::TableMetric::Ptr tableMetric,
        FormattedDocument::TableRowMetric::Ptr tableRowMetric)
    {
        auto tableCellMetric = std::make_shared<FormattedDocument::TableCellMetric>();
        tableRowMetric->addCellMetric(tableCellMetric);
        tableCellMetric->setContentOrigin(xhtmlElement);

        auto styleableElement = std::dynamic_pointer_cast<XhtmlStyleableInterface>(xhtmlElement);
        auto styleEntry = styleableElement->getStyleEntry();
        // SizeType::ViewportWidth with 0.0f acts as a default.
        if (styleEntry != nullptr && (styleEntry->getStyleEntryFlags() & StyleEntryFlags::Width) == StyleEntryFlags::Width)
            tableMetric->updateRequestedColumnSize(tableRowMetric->getCellMetricCount() - 1, styleEntry->getWidth());
        else
            tableMetric->updateRequestedColumnSize(tableRowMetric->getCellMetricCount() - 1, OneDimSize(SizeType::ViewportWidth, 0.0f));

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedXhtmlDocument::calculateTableColumnPreferredSizes(float availableDimension, FormattedDocument::TableMetric::Ptr tableMetric)
    {
        size_t freeColumnCount = 0;
        float  freePercentageTotal = 0.0f;
        size_t relativeColumnCount = 0;
        float  relativePercentageTotal = 0.0f;
        size_t fixedColumnCount = 0;
        float  fixedWidthTotal = 0.0f;

        for (size_t index = 0; index < tableMetric->getRequestedColumnSizesCount(); index++)
        {
            auto preferredColumnSize = tableMetric->getRequestedColumnSize(index);
            if (preferredColumnSize.sizeType == SizeType::ViewportWidth && preferredColumnSize.value == 0.0f)
            {
                freeColumnCount++;
                freePercentageTotal += availableDimension != 0.0f ? 100.0f / availableDimension : 0.1f;
            }
            else if (preferredColumnSize.sizeType == SizeType::ViewportWidth)
            {
                relativeColumnCount++;
                relativePercentageTotal += preferredColumnSize.value;
            }
            else if (preferredColumnSize.sizeType == SizeType::Relative)
            {
                relativeColumnCount++;
                relativePercentageTotal += preferredColumnSize.value;
            }
            else
            {
                auto fixedWidth = preferredColumnSize.toPixel(availableDimension);
                fixedColumnCount++;
                fixedWidthTotal += fixedWidth.value;

                tableMetric->updatePreferredColumnSize(index, {true, fixedWidth.value});
            }
        }

        float relativePercentageRequired = relativePercentageTotal + freePercentageTotal;
        float freeWidthAvailable = relativePercentageRequired != 0.0f ? (availableDimension - fixedWidthTotal) * freePercentageTotal / relativePercentageRequired : availableDimension - fixedWidthTotal;
        float relativeWidthAvailable = relativePercentageRequired != 0.0f ? (availableDimension - fixedWidthTotal) * relativePercentageTotal / relativePercentageRequired : availableDimension - fixedWidthTotal;

        for (size_t index = 0; index < tableMetric->getRequestedColumnSizesCount(); index++)
        {
            auto preferredColumnSize = tableMetric->getRequestedColumnSize(index);
            if (preferredColumnSize.sizeType == SizeType::ViewportWidth && preferredColumnSize.value == 0.0f)
                tableMetric->updatePreferredColumnSize(index, {false, freeWidthAvailable / freeColumnCount});
            else if (preferredColumnSize.sizeType == SizeType::ViewportWidth)
                tableMetric->updatePreferredColumnSize(index, {false, relativeWidthAvailable * preferredColumnSize.value});
            else if (preferredColumnSize.sizeType == SizeType::Relative)
                tableMetric->updatePreferredColumnSize(index, {false, relativeWidthAvailable * preferredColumnSize.value});
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
        if (typeName == XhtmlElementType::Body            ||
            typeName == XhtmlElementType::H1              || typeName == XhtmlElementType::H2              || typeName == XhtmlElementType::H3 ||
            typeName == XhtmlElementType::H4              || typeName == XhtmlElementType::H5              || typeName == XhtmlElementType::H6 ||
            typeName == XhtmlElementType::Emphasized      || typeName == XhtmlElementType::Italic          ||
            typeName == XhtmlElementType::Strong          || typeName == XhtmlElementType::Bold            ||
            typeName == XhtmlElementType::Underline       ||
            typeName == XhtmlElementType::Superscript     || typeName == XhtmlElementType::Subscript       ||
            typeName == XhtmlElementType::UnorderedList   || typeName == XhtmlElementType::OrderedList     ||
            typeName == XhtmlElementType::ListItem        ||
            typeName == XhtmlElementType::Table           ||
            typeName == XhtmlElementType::TableHead       || typeName == XhtmlElementType::TableBody       || typeName == XhtmlElementType::TableFoot     ||
            typeName == XhtmlElementType::TableRow        || typeName == XhtmlElementType::TableHeaderCell || typeName == XhtmlElementType::TableDataCell ||
            typeName == XhtmlElementType::Span            || typeName == XhtmlElementType::Anchor          ||
            typeName == XhtmlElementType::Division        || typeName == XhtmlElementType::Preformatted    || typeName == XhtmlElementType::Code ||
            typeName == XhtmlElementType::Paragraph       || typeName == XhtmlElementType::Image)
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
                if (predecessorElementProvidesExtraSpace || parentElementSuppressesInitialExtraSpace)
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
                auto predec = XhtmlElement::getPreviousSibling(xhtmlElement);
                auto parentTypeName = (parent != nullptr ? parent->getTypeName() : XhtmlElementType::Body);
                auto predecTypeName = (predec != nullptr ? predec->getTypeName() : XhtmlElementType::Break);

                // -- Prepare inner layout
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

                auto newListMetric = std::make_shared<FormattedDocument::ListData>();
                newListMetric->Ordered = (typeName == XhtmlElementType::OrderedList);
                newListMetric->ActualItemIndex = 0;
                newListMetric->ItemType = MarkupListItemType::InheritOrDefault;
                m_formattingState.ListMetrics.push_back(newListMetric);

                // -- Prepare X

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

                // -- Prepare Bullet-Counting and Indent
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
            else if (typeName == XhtmlElementType::Table)
            {
                // Can be an anchor or styled, but doesn't represent text/image ==> FormattedRectangle

                // -- Prepare Y
                m_evolvingLayoutArea.top += m_evolvingLineExtraHeight + m_defaultTextSize;
                m_evolvingLineExtraHeight = 0;

                // -- Prepare inner layout
                auto tableMetric = std::make_shared<FormattedDocument::TableMetric>();
                m_formattingState.TableMetrics.push_back(tableMetric);

                // -- Prepare X
                tableMetric->setCachedLayoutArea(m_evolvingLayoutArea);

                // -- Create element
                currentFormattedElement = createFormattedRectangleWithPosition(xhtmlElement);
                m_content.push_back(currentFormattedElement);

                // -- Update inner layout
                tableMetric->setFormattedElement(currentFormattedElement);
                tableMetric->setContentOrigin(xhtmlElement);

                // -- Act like a "\r"
                m_evolvingLineRunLength = 0;

                // -- Set flags
            }
            else if (typeName == XhtmlElementType::TableHead || typeName == XhtmlElementType::TableBody || typeName == XhtmlElementType::TableFoot)
            {
                // Everything is done by table's call to calculateTableColumnRequestedSizes().
            }
            else if (typeName == XhtmlElementType::TableRow || typeName == XhtmlElementType::TableHeaderCell || typeName == XhtmlElementType::TableDataCell)
            {
                // Can be an anchor or styled, but doesn't represent text/image ==> FormattedRectangle

                // -- Prepare Y
                m_evolvingLineExtraHeight = 0;

                // -- Prepare inner layout
                FormattedDocument::TableMetric::Ptr tableMetric = m_formattingState.TableMetrics.back();

                // -- Prepare X, remember Y and update Y & X
                if (tableMetric)
                {
                    // Prepare X
                    auto cachedLayoutArea = tableMetric->getCachedLayoutArea();
                    auto tableToCellSpacing = tableMetric->calculateTableLayoutAreaToCellSpacing();
                    m_evolvingLayoutArea.left  = cachedLayoutArea.left + tableToCellSpacing.getLeft();
                    m_evolvingLayoutArea.width = cachedLayoutArea.width - tableToCellSpacing.getLeft() - tableToCellSpacing.getRight();

                    // Remember Y
                    if (typeName == XhtmlElementType::TableRow)
                    {
                        // Must be dispensable, since there are also free cells without row.
                        size_t rowIndex = 0;
                        auto tableRowMetric = tableMetric->firstOrDefaultRowMetricByContentOrigin(xhtmlElement, rowIndex);
                        if (tableRowMetric)
                        {
                            if (tableRowMetric->getStartCoordinate() < 0.0f)
                            {
                                auto tableCellSpacing = tableMetric->calculateTableRenderAreaToCellSpacing();
                                m_evolvingLayoutArea.top += tableCellSpacing.getTop();
                            }
                            else
                                m_evolvingLayoutArea.top += 2.0f;

                            tableRowMetric->setStartCoordinate(m_evolvingLayoutArea.top);
                        }
                        m_evolvingLayoutArea.top = tableRowMetric->getStartCoordinate();
                    }

                    // Remember Y and update Y & X
                    if (typeName == XhtmlElementType::TableHeaderCell || typeName == XhtmlElementType::TableDataCell)
                    {
                        size_t rowIndex = 0;
                        auto tableRowMetric = tableMetric->firstOrDefaultRowMetricByAnyCellContentOrigin(xhtmlElement, rowIndex);
                        if (tableRowMetric)
                        {
                            // Fallback, if there is no row.
                            if (tableRowMetric->getStartCoordinate() < 0.0f)
                            {
                                if (rowIndex == 0)
                                {
                                    auto tableCellSpacing = tableMetric->calculateTableRenderAreaToCellSpacing();
                                    m_evolvingLayoutArea.top += tableCellSpacing.getTop();
                                }
                                else
                                    m_evolvingLayoutArea.top += 2.0f;

                                tableRowMetric->setStartCoordinate(m_evolvingLayoutArea.top);
                            }
                            m_evolvingLayoutArea.top = tableRowMetric->getStartCoordinate();

                            auto columnIndex = tableRowMetric ? tableRowMetric->firstOrDefaultCellIndexByContentOrigin(xhtmlElement) : (size_t)0;
                            auto cellLayoutArea = tableMetric->calculateCellLayoutArea(columnIndex);
                            m_evolvingLayoutArea.left += cellLayoutArea.left;
                            m_evolvingLayoutArea.width = cellLayoutArea.width;

                            auto tableCellMetric = tableRowMetric->firstOrDefaultCellMetricByContentOrigin(xhtmlElement);
                            if (tableCellMetric)
                                tableCellMetric->setStartCoordinate(m_evolvingLayoutArea.left);
                        }
                    }
                }

                // -- Create element (but only if <table> tag exists)
                if (tableMetric)
                {
                    currentFormattedElement = createFormattedRectangleWithPosition(xhtmlElement);
                    m_content.push_back(currentFormattedElement);
                }

                // -- Register element
                if (tableMetric)
                {
                    if (typeName == XhtmlElementType::TableRow)
                    {
                        auto tableRowMetric = tableMetric->firstOrDefaultRowMetricByContentOrigin(xhtmlElement);
                        if (tableRowMetric)
                            tableRowMetric->setFormattedElement(currentFormattedElement);
                    }

                    // Header cells (<th>...</th>) aren't automatically assigned to the head row range!
                    // The only defferences are alignment center and bold font.
                    if (typeName == XhtmlElementType::TableHeaderCell || typeName == XhtmlElementType::TableDataCell)
                    {
                        auto tableRowMetric = tableMetric->firstOrDefaultRowMetricByAnyCellContentOrigin(xhtmlElement);
                        auto tableCellMetric = tableRowMetric ? tableRowMetric->firstOrDefaultCellMetricByContentOrigin(xhtmlElement) : nullptr;
                        if (tableCellMetric)
                            tableCellMetric->setFormattedElement(currentFormattedElement);
                    }
                }

                // -- Act like a "\r"
                if (tableMetric)
                {
                    m_evolvingLineRunLength = 0;
                }

                // -- Set flags
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
            if (typeName == XhtmlElementType::Body            ||
                typeName == XhtmlElementType::H1              || typeName == XhtmlElementType::H2              || typeName == XhtmlElementType::H3 ||
                typeName == XhtmlElementType::H4              || typeName == XhtmlElementType::H5              || typeName == XhtmlElementType::H6 ||
                typeName == XhtmlElementType::UnorderedList   || typeName == XhtmlElementType::OrderedList     ||
                typeName == XhtmlElementType::Table           ||
                typeName == XhtmlElementType::TableHead       || typeName == XhtmlElementType::TableBody       || typeName == XhtmlElementType::TableFoot     ||
                typeName == XhtmlElementType::TableRow        || typeName == XhtmlElementType::TableHeaderCell || typeName == XhtmlElementType::TableDataCell ||
                typeName == XhtmlElementType::Span            || typeName == XhtmlElementType::Anchor          ||
                typeName == XhtmlElementType::Division        || typeName == XhtmlElementType::Preformatted    || typeName == XhtmlElementType::Code ||
                typeName == XhtmlElementType::Paragraph       || typeName == XhtmlElementType::Image)
            {
                FormattedRectangle::Ptr formattedRectSection = std::dynamic_pointer_cast<FormattedRectangle>(currentFormattedElement);
                if (formattedRectSection)

                {
                    applyStyleEntriesToFormattedElement(formattedRectSection, styleEntries, { m_availableClientSize.x, /* yes, X */ m_availableClientSize.x }, fontCollection,
                        StyleCategoryFlags::BackColor | StyleCategoryFlags::Opacity |
                        StyleCategoryFlags::BorderStyle | StyleCategoryFlags::BorderWidth | StyleCategoryFlags::BorderColor);
                }
            }

            // -------------------------------
            // Apply leading margin
            // -------------------------------
            if (xhtmlStyleableElement &&
                typeName != XhtmlElementType::TableHead &&
                typeName != XhtmlElementType::TableBody &&
                typeName != XhtmlElementType::TableFoot &&
                typeName != XhtmlElementType::TableRow &&
                typeName != XhtmlElementType::TableHeaderCell &&
                typeName != XhtmlElementType::TableDataCell)
            {
                FormattedRectangle::Ptr formattedRectSection = std::dynamic_pointer_cast<FormattedRectangle>(currentFormattedElement);
                if (formattedRectSection)
                {
                    for (auto styleEntry : styleEntries)
                    {
                        if ((styleEntry->getStyleEntryFlags() & StyleEntryFlags::Margin) == StyleEntryFlags::Margin)
                        {
                            auto margin = styleEntry->getMargin().toPixel({ m_availableClientSize.x, /* yes, X */ m_availableClientSize.x });
                            inflate(m_evolvingLayoutArea, -margin.left, -margin.top, -margin.right, 0);
                            formattedRectSection->setMargin(margin);
                        }
                    }
                }
            }

            // -----------------------------
            // Apply leading padding
            // -----------------------------
            if (xhtmlStyleableElement &&
                typeName != XhtmlElementType::Table &&
                typeName != XhtmlElementType::TableHead &&
                typeName != XhtmlElementType::TableBody &&
                typeName != XhtmlElementType::TableFoot &&
                typeName != XhtmlElementType::TableRow)
            {
                if (currentFormattedElement)
                {
                    for (auto styleEntry : styleEntries)
                    {
                        if ((styleEntry->getStyleEntryFlags() & StyleEntryFlags::Padding) == StyleEntryFlags::Padding)
                        {
                            auto padding = styleEntry->getPadding().toPixel({ m_availableClientSize.x, /* yes, X */ m_availableClientSize.x });
                            inflate(m_evolvingLayoutArea, -padding.left, -padding.top, -padding.right, 0);
                        }
                    }
                }
            }

            // -------------------------------
            // Special table treatment
            // -------------------------------
            if (typeName == XhtmlElementType::Table)
            {
                // First stage precalculation of rows and cells.
                FormattedDocument::TableMetric::Ptr tableMetric = m_formattingState.TableMetrics.back();
                if (tableMetric)
                {
                    calculateTableColumnRequestedSizes(xhtmlElement, tableMetric);

                    auto tableToCellSpacing = tableMetric->calculateTableLayoutAreaToCellSpacing();
                    auto cellToCellSpacing  = std::max((size_t)0, tableMetric->getRequestedColumnSizesCount() - 1) * 2.0f;
                    calculateTableColumnPreferredSizes(m_evolvingLayoutArea.width - tableToCellSpacing.getLeft() - tableToCellSpacing.getRight() - cellToCellSpacing, tableMetric);
                }
            }

            // -------------------------------
            // Process children
            // -------------------------------
            bool loopinternalPredecessorElementProvidesExtraSpace = false;
            bool lastchildAcceptsRunLengtExpansion = false;
            if (typeName == XhtmlElementType::Table     ||
                typeName == XhtmlElementType::TableHead || typeName == XhtmlElementType::TableBody || typeName == XhtmlElementType::TableFoot)
            {
                // For a table and table row-ranges we can't strictly follow the XHTML element hierarchy.
                // Because free rows always belog to the <tbody>...</tbody> row-range (even if they appear before or after the <tbody>...</tbody> row-range).
                // And free cells always create a separate row and this row belongs to the  <tbody>...</tbody> row-range.

                FormattedDocument::TableMetric::Ptr tableMetric = m_formattingState.TableMetrics.back();
                if (tableMetric && typeName == XhtmlElementType::Table)
                {
                    auto tableRowRanges = { TableRowRange::Head , TableRowRange::Body, TableRowRange::Foot };
                    for (auto tableRowRange : tableRowRanges)
                    {
                        for (size_t rowIndex = 0; rowIndex < tableMetric->getRowRangeRowMetricCount(tableRowRange); rowIndex++)
                        {
                            auto rowMetric = tableMetric->getRowRangeRowMetric(tableRowRange, rowIndex);
                            auto rowMarkupElement = rowMetric ? rowMetric->getContentOrigin() : nullptr;
                            auto rowXhtmlElement = std::dynamic_pointer_cast<XhtmlElement>(rowMarkupElement);
                            if (rowXhtmlElement)
                            {
                                layout(loopinternalPredecessorElementProvidesExtraSpace, false,
                                    lastchildAcceptsRunLengtExpansion, rowXhtmlElement, fontCollection, keepSelection);
                            }
                            else if (rowMetric)
                            {
                                for (size_t cellIndex = 0; cellIndex < rowMetric->getCellMetricCount(); cellIndex++)
                                {
                                    auto tableCellMetric = rowMetric->getCellMetric(cellIndex);
                                    auto cellMarkupElement = tableCellMetric ? tableCellMetric->getContentOrigin() : nullptr;
                                    auto cellXhtmlElement = std::dynamic_pointer_cast<XhtmlElement>(cellMarkupElement);
                                    if (cellXhtmlElement)
                                    {
                                        layout(loopinternalPredecessorElementProvidesExtraSpace, false,
                                            lastchildAcceptsRunLengtExpansion, cellXhtmlElement, fontCollection, keepSelection);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                // For everything else except table and table row-ranges we can strictly follow the XHTML element hierarchy.
                // Because free rows are already assigned to a row-range and free cells are already assigned to a row.

                for (size_t index = 0; xhtmlElement->isContainer() && index < xhtmlElement->countChildren(); index++)
                    layout(loopinternalPredecessorElementProvidesExtraSpace, index == 0 && currentIsInitialExtraSpaceSuppressingElement,
                        lastchildAcceptsRunLengtExpansion, xhtmlElement->getChild(index), fontCollection, keepSelection);
            }
            float bottomExtraSpace = 0.0f;

            // -----------------------------
            // Apply tailing padding
            // -----------------------------
            if (xhtmlStyleableElement &&
                typeName != XhtmlElementType::Table &&
                typeName != XhtmlElementType::TableHead &&
                typeName != XhtmlElementType::TableBody &&
                typeName != XhtmlElementType::TableFoot &&
                typeName != XhtmlElementType::TableRow)
            {
                if (currentFormattedElement)
                {
                    for (auto styleEntry : styleEntries)
                    {
                        if ((styleEntry->getStyleEntryFlags() & StyleEntryFlags::Padding) == StyleEntryFlags::Padding)
                        {
                            auto padding = styleEntry->getPadding().toPixel({ m_availableClientSize.x, /* yes, X */ m_availableClientSize.x });
                            inflate(m_evolvingLayoutArea, padding.left, -padding.bottom, padding.right, 0);
                            bottomExtraSpace += padding.bottom;
                        }
                    }
                }
            }

            // -------------------------------
            // Apply tailing margin
            // -------------------------------
            if (xhtmlStyleableElement &&
                typeName != XhtmlElementType::TableHead &&
                typeName != XhtmlElementType::TableBody &&
                typeName != XhtmlElementType::TableFoot &&
                typeName != XhtmlElementType::TableRow &&
                typeName != XhtmlElementType::TableHeaderCell &&
                typeName != XhtmlElementType::TableDataCell)
            {
                if (currentFormattedElement)
                {
                    for (auto styleEntry : styleEntries)
                    {
                        if ((styleEntry->getStyleEntryFlags() & StyleEntryFlags::Margin) == StyleEntryFlags::Margin)
                        {
                            auto margin = styleEntry->getMargin().toPixel({ m_availableClientSize.x, /* yes, X */ m_availableClientSize.x });
                            inflate(m_evolvingLayoutArea, margin.left, -margin.bottom, margin.right, 0);
                            bottomExtraSpace += margin.bottom;
                        }
                    }
                }
            }

            // -------------------------------
            // Finalize background
            // -------------------------------
            if (typeName == XhtmlElementType::Body            || typeName == XhtmlElementType::UnorderedList   || typeName == XhtmlElementType::OrderedList ||
                typeName == XhtmlElementType::H1              || typeName == XhtmlElementType::H2              || typeName == XhtmlElementType::H3 ||
                typeName == XhtmlElementType::H4              || typeName == XhtmlElementType::H5              || typeName == XhtmlElementType::H6 ||
                typeName == XhtmlElementType::UnorderedList   || typeName == XhtmlElementType::OrderedList     ||
                typeName == XhtmlElementType::Table           ||
                typeName == XhtmlElementType::TableHead       || typeName == XhtmlElementType::TableBody       || typeName == XhtmlElementType::TableFoot ||
                typeName == XhtmlElementType::TableRow        || typeName == XhtmlElementType::TableHeaderCell || typeName == XhtmlElementType::TableDataCell ||
                typeName == XhtmlElementType::Span            || typeName == XhtmlElementType::Anchor          ||
                typeName == XhtmlElementType::Division        || typeName == XhtmlElementType::Preformatted    || typeName == XhtmlElementType::Code ||
                typeName == XhtmlElementType::Paragraph)      // typeName == XhtmlElementType::Image)
            {
                if (currentFormattedElement && typeName == XhtmlElementType::Table)
                {
                    FormattedDocument::TableMetric::Ptr tableMetric = m_formattingState.TableMetrics.back();
                    if (tableMetric)
                    {
                        auto tableToCellSpacing = tableMetric->calculateTableLayoutAreaToCellSpacing();
                        auto rowMetric = tableMetric->getTailRowMetric();
                        auto rightBottom = Vector2f(tableMetric->getCachedLayoutArea().left + tableMetric->getTotalPreferredColumnSizes() +
                                                    std::max((size_t)0, tableMetric->getRequestedColumnSizesCount() - 1) * 2.0f +
                                                    tableToCellSpacing.getLeft() + tableToCellSpacing.getRight(),
                                                    (rowMetric != nullptr ? rowMetric->getEndCoordinate() : m_evolvingLayoutArea.top) +
                                                    tableToCellSpacing.getBottom());
                        currentFormattedElement->setLayoutRightBottom(rightBottom);
                    }
                }
                else if (currentFormattedElement)
                {
                    // Recover the last child (if any) or itself. If there is a child - if child is text section a special treatment is required.
                    auto lastText = std::dynamic_pointer_cast<FormattedTextSection>(m_content.back());
                    auto lastImage = std::dynamic_pointer_cast<FormattedImage>(m_content.back());
                    auto lastFlowContentSection = lastText ? std::dynamic_pointer_cast<FormattedElement>(lastText) : std::dynamic_pointer_cast<FormattedElement>(lastImage);
                    if (lastFlowContentSection)
                    {
                        // typically the the text sections are always open to add new charachters (in other words: not finalized with line break / carriage return)
                        // which implies, that m_evolvingLayoutArea.top points still to the top of the  text sections, while they might have a heigth
                        if (typeName == XhtmlElementType::Span || typeName == XhtmlElementType::Anchor)
                            currentFormattedElement->setLayoutRightBottom(Vector2f(m_evolvingLayoutArea.left + m_evolvingLineRunLength, lastFlowContentSection->getLayoutRefLine() + m_formattingState.TextHeight / 4 + bottomExtraSpace));
                        else if (typeName == XhtmlElementType::TableDataCell || typeName == XhtmlElementType::TableHeaderCell)
                            currentFormattedElement->setLayoutRightBottom(Vector2f(right(m_evolvingLayoutArea), lastFlowContentSection->getLayoutRefLine() + bottomExtraSpace));
                        else
                            currentFormattedElement->setLayoutRightBottom(Vector2f(right(m_evolvingLayoutArea), lastFlowContentSection->getLayoutRefLine() + m_formattingState.TextHeight / 4 + bottomExtraSpace));
                    }
                    else
                        currentFormattedElement->setLayoutRightBottom(Vector2f(right(m_evolvingLayoutArea), m_evolvingLayoutArea.top));
                }
            }

            // -------------------------------
            // Finalize formatted elements
            // Apply tailing offsets
            // -------------------------------
            if (typeName == XhtmlElementType::H1       || typeName == XhtmlElementType::H2              || typeName == XhtmlElementType::H3 ||
                typeName == XhtmlElementType::H4       || typeName == XhtmlElementType::H5              || typeName == XhtmlElementType::H6)
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
            else if (typeName == XhtmlElementType::Table)
            {
                FormattedDocument::TableMetric::Ptr tableMetric = m_formattingState.TableMetrics.back();
                if (typeName == XhtmlElementType::Table)
                    m_formattingState.TableMetrics.pop_back();

                if (tableMetric)
                {
                    auto cachedLayoutArea = tableMetric->getCachedLayoutArea();
                    m_evolvingLayoutArea.left  = cachedLayoutArea.left;
                    m_evolvingLayoutArea.width = cachedLayoutArea.width;
                }
            }
            else if (typeName == XhtmlElementType::TableRow)
            {
                FormattedDocument::TableMetric::Ptr tableMetric = m_formattingState.TableMetrics.back();
                if (tableMetric)
                {
                    auto tableRowMetric = tableMetric->firstOrDefaultRowMetricByContentOrigin(xhtmlElement);
                    if (currentFormattedElement)
                        currentFormattedElement->setLayoutBottom(currentFormattedElement->getLayoutBottom());
                        // ToDo: Make this calculation meaningful!

                    m_evolvingLayoutArea.left = tableMetric->getCachedLayoutArea().left;
                }

                // act like a "\r\n"
                m_evolvingLineRunLength = 0;
                m_evolvingLineExtraHeight = 0;
            }
            else if (typeName == XhtmlElementType::TableHeaderCell || typeName == XhtmlElementType::TableDataCell)
            {
                m_evolvingLayoutArea.top = currentFormattedElement->getLayoutBottom();

                // Row end coordinate is equal to the max. cell end coordinate.
                FormattedDocument::TableMetric::Ptr tableMetric = m_formattingState.TableMetrics.back();
                if (tableMetric)
                {
                    auto tableRowMetric = tableMetric->firstOrDefaultRowMetricByAnyCellContentOrigin(xhtmlElement);
                    if (tableRowMetric)
                    {
                        tableRowMetric->setEndCoordinate(std::max(m_evolvingLayoutArea.top, tableRowMetric->getEndCoordinate()));
                        m_evolvingLayoutArea.top = tableRowMetric->getEndCoordinate();

                        for (size_t columnIndex = 0; columnIndex < tableRowMetric->getCellMetricCount(); columnIndex++)
                        {
                            auto tableCellMetric = tableRowMetric->getCellMetric(columnIndex);
                            if (tableCellMetric)
                            {
                                auto cellFormattedElement = tableCellMetric->getFormattedElement();
                                if (cellFormattedElement)
                                    cellFormattedElement->setLayoutBottom(m_evolvingLayoutArea.top);
                                    // ToDo: Make this calculation meaningful!
                            }
                        }
                    }

                    m_evolvingLayoutArea.left = tableMetric->getCachedLayoutArea().left;
                }

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

            if (typeName == XhtmlElementType::H1              || typeName == XhtmlElementType::H2            || typeName == XhtmlElementType::H3 ||
                typeName == XhtmlElementType::H4              || typeName == XhtmlElementType::H5            || typeName == XhtmlElementType::H6)
            {
                predecessorElementProvidesExtraSpace = true;
                lastchildAcceptsRunLengtExpansion = false;
            }
            else if (typeName == XhtmlElementType::UnorderedList || typeName == XhtmlElementType::OrderedList)
            {
                predecessorElementProvidesExtraSpace = true;
                lastchildAcceptsRunLengtExpansion = false;
            }
            else if (typeName == XhtmlElementType::Table     ||
                     typeName == XhtmlElementType::TableHead || typeName == XhtmlElementType::TableBody       || typeName == XhtmlElementType::TableFoot ||
                     typeName == XhtmlElementType::TableRow  || typeName == XhtmlElementType::TableHeaderCell || typeName == XhtmlElementType::TableDataCell)
            {
                predecessorElementProvidesExtraSpace = false;
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    tgui::String FormattedXhtmlDocument::readXhtmlDocument(const std::string filePath, tgui::String& fileEncoding, std::string& errorMessage)
    {
        tgui::String  hypertextString;
        std::ifstream hypertextFile(filePath, std::ios::in | std::ios::binary);
        if (hypertextFile.is_open())
        {
            hypertextFile.seekg (0, std::ios::end);
            size_t fileSizeInByte = hypertextFile.tellg();
            hypertextFile.seekg (0, std::ios::beg);
            std::vector<char> data;
            data.resize(fileSizeInByte);
            hypertextFile.read(&data[0], fileSizeInByte);

            if (fileSizeInByte <= 2)
            {
                errorMessage = "File too small or empty.";
            }
            else if ((unsigned char)data[0] == 254 && (unsigned char)data[1] == 255)
            {
                if ((unsigned char)data[0] == 0)
                    fileEncoding = U"UTF-16 BE";
                else
                    fileEncoding = U"UTF-16 LE";
            }
            else if ((unsigned char)data[0] == 254 && (unsigned char)data[1] == 239)
            {
                fileEncoding = U"UTF-8";
            }
            else
            {
                if (tgui::xhtml::UtfHelper::checkValid(data))
                    fileEncoding = U"UTF-8";
                else
                    fileEncoding = U"ASCII";
            }

            if (fileEncoding == U"UTF-8" || fileEncoding == U"ASCII")
            {
                data.push_back(0);
                hypertextString = tgui::String(data.data());
            }
            hypertextFile.close();
        }
        else
            errorMessage = "Unable to open file.";

        return hypertextString;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<XhtmlElement::Ptr> FormattedXhtmlDocument::parseXhtmlDocument(const tgui::String hypertextString,
                                                                                  bool resolveEntities, bool trace)
    {
        XhtmlParser xhtmlParser(hypertextString);
        xhtmlParser.parseDocument(resolveEntities, trace);

        if (trace)
        {
            auto errorMessages = xhtmlParser.getErrorMessages();
            for (size_t index = 0; index < errorMessages->size(); index++)
                std::wcerr << errorMessages->at(index) << std::endl;

            auto warningMessages = xhtmlParser.getWarningMessages();
            for (size_t index = 0; index < warningMessages->size(); index++)
                std::wcout << warningMessages->at(index) << std::endl;
        }

        return xhtmlParser.getRootElements();
    }

} }
