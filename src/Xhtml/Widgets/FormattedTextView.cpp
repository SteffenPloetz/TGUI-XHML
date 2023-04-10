#include <TGUI/Config.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Keyboard.hpp>

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
#include "TGUI/Xhtml/XhtmlElements.hpp"
#include "TGUI/Widgets/FormattedElements.hpp"
#include "TGUI/Widgets/FormattedTextXhtmlDocument.hpp"

#include "TGUI/Renderers/FormattedTextRenderer.hpp"
#include "TGUI/Widgets/FormattedTextView.hpp"

namespace tgui
{
    // ToDo: Discuss in forum.
#if TGUI_COMPILED_WITH_CPP_VER < 17 && !defined( _MSC_VER)
    constexpr const char FormattedTextView::StaticWidgetType[];
#endif

    FormattedTextView::FormattedTextView(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, false}, m_document(), m_zoom(1.0f), m_bordersCached(0), m_paddingCached(2),
        m_borderColorCached(Color::Transparent), m_backgroundColorCached(Color::Transparent),
        m_horizontalScrollbar(), m_verticalScrollbar(), m_fontCollection(), m_anchorSources(), m_anchorTargets()
    {
        m_horizontalScrollbar->setSize(/*Will be updated anyway*/ 18, 18);
        m_horizontalScrollbar->setVisible(false);
        m_verticalScrollbar->setSize(18, /*Will be updated anyway*/ 18);
        m_verticalScrollbar->setVisible(true);
        setHorizontalScrollbarPolicy(Scrollbar::Policy::Automatic);
        setVerticalScrollbarPolicy(Scrollbar::Policy::Always);

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<FormattedTextRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            m_textSizeCached = 16; // getGlobalTextSize();
            setTextSize(m_textSizeCached);
            setSize({Text::getLineHeight(m_fontCached, m_textSizeCached) * 18,
                     10 * m_fontCached.getLineSpacing(m_textSizeCached)
                     + std::max(m_fontCached.getFontHeight(m_textSizeCached), m_fontCached.getLineSpacing(m_textSizeCached)) - m_fontCached.getLineSpacing(m_textSizeCached)
                     + Text::getExtraVerticalPadding(m_textSizeCached)
                     + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
        }

        m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FormattedTextView::Ptr FormattedTextView::copy(FormattedTextView::ConstPtr formattedTextView)
    {
        if (formattedTextView)
            return std::static_pointer_cast<FormattedTextView>(formattedTextView->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::setSize(const Layout2d& size)
    {
        ClickableWidget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        updateScrollbars();

        // The size of the XHTML static area has changed, update the content layout
        rearrangeText(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FormattedTextView::mouseWheelScrolled(float delta, Vector2f pos)
    {
        if(keyboard::isMultiselectModifierPressed())
            return updateZoom(delta);

        bool scrollbarMoved = false;
        if (m_horizontalScrollbar->isShown() &&
            (!m_verticalScrollbar->isShown() ||
             m_horizontalScrollbar->isMouseOnWidget(pos - getPosition()) ||
             keyboard::isShiftPressed()))
        {
            scrollbarMoved = m_horizontalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
        }
        else if (m_verticalScrollbar->isShown())
        {
            scrollbarMoved = m_verticalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
        }

        if (scrollbarMoved)
            recalculateVisiblePart();

        return scrollbarMoved;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::setHorizontalScrollbarPolicy(Scrollbar::Policy policy)
    {
        m_horizontalScrollbarPolicy = policy;

        if (policy == Scrollbar::Policy::Always)
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(false);
        }
        else if (policy == Scrollbar::Policy::Never)
        {
            m_horizontalScrollbar->setVisible(false);
        }
        else // Scrollbar::Policy::Automatic
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(true);
        }

        rearrangeText(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::setVerticalScrollbarPolicy(Scrollbar::Policy policy)
    {
        m_verticalScrollbarPolicy = policy;

        if (policy == Scrollbar::Policy::Always)
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(false);
        }
        else if (policy == Scrollbar::Policy::Never)
        {
            m_verticalScrollbar->setVisible(false);
        }
        else // Scrollbar::Policy::Automatic
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(true);
        }

        rearrangeText(false);
    }

    bool FormattedTextView::updateZoom(float delta)
    {
        float newZoom = m_zoom;

        if (delta < 0.0f && m_zoom > 5.0f)            newZoom -= 0.5f;
        else if (delta < 0.0f && m_zoom > 2.0f)       newZoom -= 0.2f;
        else if (delta < 0.0f && m_zoom > 0.1f)       newZoom -= 0.1f;

        else if (delta > 0.0f && m_zoom < 2.0f)       newZoom += 0.1f;
        else if (delta > 0.0f && m_zoom < 5.0f)       newZoom += 0.2f;
        else if (delta > 0.0f && m_zoom < 10.0f)      newZoom += 0.5f;

        return setZoom(newZoom);
    }

    bool FormattedTextView::setZoom(float newZoom)
    {
        if (newZoom < 0.1f)
            newZoom = 0.1f;
        if (newZoom > 10.0f)
            newZoom = 10.0f;

        if (newZoom == m_zoom)
            return false;

        m_zoom = newZoom;
        rearrangeText(true);
        onZoomChange.emit(this, m_zoom);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& FormattedTextView::getSignal(String signalName)
    {
        if (signalName == onZoomChange.getName())
            return onZoomChange;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::leftMousePressed(Vector2f pos)
    {
        ClickableWidget::leftMousePressed(pos);

        pos -= getPosition();

        // If there is a scroll-bar then pass the event
        if ((m_verticalScrollbar->isShown()) && (m_verticalScrollbar->isMouseOnWidget(pos)))
        {
            m_verticalScrollbar->leftMousePressed(pos);
            recalculateVisiblePart();
        }
        else if (m_horizontalScrollbar->isShown() && m_horizontalScrollbar->isMouseOnWidget(pos))
        {
            m_horizontalScrollbar->leftMousePressed(pos);
        }
        else // The click occurred on the formatted text view
        {
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::leftMouseReleased(Vector2f pos)
    {
        // If there is a scroll-bar then pass it the event
        if (m_verticalScrollbar->isShown())
        {
            // Only pass the event when the scroll-bar still thinks the mouse is down
            if (m_verticalScrollbar->isMouseDown())
            {
                m_verticalScrollbar->leftMouseReleased(pos - getPosition());
                recalculateVisiblePart();
            }
        }

        if (m_horizontalScrollbar->isShown())
        {
            if (m_horizontalScrollbar->isMouseDown())
                m_horizontalScrollbar->leftMouseReleased(pos - getPosition());
        }

        ClickableWidget::leftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::mouseMoved(Vector2f pos)
    {
        ClickableWidget::rightMousePressed(pos);

        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        // The mouse has moved so a double click is no longer possible
        m_possibleDoubleClick = false;

        // Check if the mouse event should go to the vertical scroll-bar
        if (m_verticalScrollbar->isShown() && (m_verticalScrollbar->isMouseOnWidget(pos) ||
            (m_verticalScrollbar->isMouseDown() && m_verticalScrollbar->isMouseDownOnThumb()) ))
        {
            m_verticalScrollbar->mouseMoved(pos);
            recalculateVisiblePart();
        }
        // Check if the mouse event should go to the horizontal scroll-bar
        else if (m_horizontalScrollbar->isShown() && (m_horizontalScrollbar->isMouseOnWidget(pos) ||
                 (m_horizontalScrollbar->isMouseDown() && m_horizontalScrollbar->isMouseDownOnThumb()) ))
        {
            m_horizontalScrollbar->mouseMoved(pos);
        }
        // If the mouse is held down then you are dragging or selecting text - no child widget should change the state
        else if (m_mouseDown)
        {
        }
        // Inform the scrollbars that the mouse is not on them
        else
        {
            m_verticalScrollbar->mouseNoLongerOnWidget();
            m_horizontalScrollbar->mouseNoLongerOnWidget();
        }

        Vector2f adoptedPos = pos + Vector2f(m_paddingCached.getLeft(), m_paddingCached.getTop()) +
            Vector2f(static_cast<float>(m_horizontalScrollbar->getValue()), static_cast<float>(m_verticalScrollbar->getValue()));
        for (FormattedLink::Ptr anchorSource : m_anchorSources)
        {
            if (anchorSource->getLayoutArea().contains(adoptedPos))
            {
                if (anchorSource->getActive())
                    continue;
                anchorSource->setActive(true);
            }
            else
            {
                if (!anchorSource->getActive())
                    continue;
                anchorSource->setActive(false);
            }
        }

        ClickableWidget::rightMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        if (m_verticalScrollbar->isShown())
            m_verticalScrollbar->mouseNoLongerOnWidget();

        if (m_horizontalScrollbar->isShown())
            m_horizontalScrollbar->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::leftMouseButtonNoLongerDown()
    {
        ClickableWidget::leftMouseButtonNoLongerDown();

        if (m_verticalScrollbar->isShown())
            m_verticalScrollbar->leftMouseButtonNoLongerDown();

        if (m_horizontalScrollbar->isShown())
            m_horizontalScrollbar->leftMouseButtonNoLongerDown();

        for (FormattedLink::Ptr anchorSource : m_anchorSources)
        {
            if (anchorSource->getActive())
            {
                String href = anchorSource->getHref();
                if (href.starts_with(U'#'))
                {
                    href = href.substr(1);
                    auto pormattedElement = m_anchorTargets[href];
                    if (pormattedElement != nullptr)
                    {
                        auto innerSize = getInnerSize();
                        auto layoutSize = m_document->getOccupiedLayoutSize();
                        auto elementLayoutPosition = pormattedElement->getLayoutLeftTop();

                        if (innerSize.y >= layoutSize.y)
                            break;
                        auto scrollPosY = static_cast<int>(elementLayoutPosition.y / (layoutSize.y - innerSize.y) * m_verticalScrollbar->getMaximum() + 0.49F);

                        m_verticalScrollbar->setValue(scrollPosY);
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::setFocused(bool focused)
    {
#if defined (TGUI_SYSTEM_ANDROID) || defined (TGUI_SYSTEM_IOS)
        if (focused)
            keyboard::openVirtualKeyboard({getAbsolutePosition(), getFullSize()});
        else
            keyboard::closeVirtualKeyboard();
#endif

        ClickableWidget::setFocused(focused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::updateScrollbars()
    {
        auto innerSize  = getInnerSize();
        auto layoutSize = Vector2f(0.0f, 0.0f);
        if (m_document)
            layoutSize = m_document->getOccupiedLayoutSize();

        if (m_horizontalScrollbarPolicy != Scrollbar::Policy::Never)
            m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(layoutSize.x + 0.49f));
        if (m_verticalScrollbarPolicy != Scrollbar::Policy::Never)
            m_verticalScrollbar->setMaximum(static_cast<unsigned int>(layoutSize.y + 0.49f));

        if (m_horizontalScrollbar->isShown())
        {
            auto horizontalScrollBarY = m_horizontalScrollbar->getSize().y;
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, innerSize.y - horizontalScrollBarY});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(innerSize.y - horizontalScrollBarY - m_paddingCached.getTop() - m_paddingCached.getBottom()));
        }
        else
        {
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, innerSize.y});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(innerSize.y - m_paddingCached.getTop() - m_paddingCached.getBottom()));
        }

        if (m_verticalScrollbar->isShown())
        {
            auto verticalScrollBarX = m_verticalScrollbar->getSize().x;
            m_horizontalScrollbar->setSize({innerSize.x - verticalScrollBarX, m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(innerSize.x - verticalScrollBarX - m_paddingCached.getLeft() - m_paddingCached.getRight()));
        }
        else
        {
            m_horizontalScrollbar->setSize({innerSize.x, m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(innerSize.x - m_paddingCached.getLeft() - m_paddingCached.getRight()));
        }

        if (m_horizontalScrollbar->isShown())
        {
            m_horizontalScrollbar->setPosition(m_bordersCached.getLeft(), getSize().y - m_bordersCached.getBottom() - m_horizontalScrollbar->getSize().y);
        }

        if (m_verticalScrollbar->isShown())
        {
            m_verticalScrollbar->setPosition({getSize().x - m_bordersCached.getRight() - m_verticalScrollbar->getSize().x, m_bordersCached.getTop()});
        }
        m_verticalScrollbar->setScrollAmount(static_cast<unsigned int>(m_textSizeCached));
        m_horizontalScrollbar->setScrollAmount(m_textSizeCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::rearrangeText(bool keepSelection)
    {
        auto renderSize = getRenderSize();

        if (m_document)
        {
            m_document->layout(renderSize, m_textSizeCached * m_zoom, getSharedRenderer()->getDefaultTextColor(), m_opacityCached,
                               m_fontCollection, keepSelection);
        }

        m_anchorSources.clear();
        m_anchorTargets.clear();
        if (m_document != nullptr)
        {
            for (auto formattedElement : m_document->getContent())
            {
                FormattedLink::Ptr formattedLink = nullptr;
                if (formattedElement->getContentOrigin()->getTypeName() == MarkupLanguageElement::Anchor)
                    formattedLink = std::dynamic_pointer_cast<FormattedLink>(formattedElement);
                if (formattedLink != nullptr)
                    m_anchorSources.push_back(formattedLink);

                auto originName = formattedElement->getOriginName();
                auto originId   = formattedElement->getOriginId();
                if (!originName.empty())
                    m_anchorTargets[originName] = formattedElement;
                if (!originId.empty())
                    m_anchorTargets[originId] = formattedElement;
            }
        }

        updateScrollbars();
     }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const RenderStates statesForScrollbar = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
            states.transform.translate(m_bordersCached.getOffset());
        }
        auto innerSize = getInnerSize();

        // Draw the background
        target.drawFilledRect(states, innerSize, Color::applyOpacity(m_backgroundColorCached, m_opacityCached));

        // Draw the contents of the text view
        states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});

        float clipWidth = innerSize.x - m_paddingCached.getLeft() - m_paddingCached.getRight();
        if (m_verticalScrollbar->isShown())
            clipWidth -= m_verticalScrollbar->getSize().x;

        float clipHeight = innerSize.y - m_paddingCached.getTop() - m_paddingCached.getBottom();
        if (m_horizontalScrollbar->isShown())
            clipHeight -= m_horizontalScrollbar->getSize().y;

        target.addClippingLayer(states, {{}, {clipWidth, clipHeight}});

        // Move the text according to the scrollars
        states.transform.translate({-static_cast<float>(m_horizontalScrollbar->getValue()), -static_cast<float>(m_verticalScrollbar->getValue())});

        FormattedLink::Ptr lastSourceAnchor = nullptr;
        for (auto formattedElement : m_document->getContent())
        {
            auto formattedTextSection = std::dynamic_pointer_cast<FormattedTextSection>(formattedElement);
            auto formattedImage = std::dynamic_pointer_cast<FormattedImage>(formattedElement);
            auto formattedRectangle = std::dynamic_pointer_cast<FormattedRectangle>(formattedElement);
            if (formattedTextSection)
            {
                bool match = false;
                if (lastSourceAnchor)
                {
                    auto textRenderArea = formattedTextSection->getLayoutArea();

                    if (lastSourceAnchor->getLayoutArea().contains(textRenderArea.getPosition() + (textRenderArea.getSize() * 0.5F)))
                        match = true;
                }

                Text text;
                text.setString(formattedTextSection->getString());
                text.setFont(formattedTextSection->getFont());
                text.setCharacterSize(formattedTextSection->getCharacterSizeAsInt());
                text.setPosition({formattedTextSection->getLayoutLeft(),  formattedTextSection->getLayoutTop()});
                text.setOpacity(formattedTextSection->getOpacity());
                if (match)
                {
                    text.setColor(lastSourceAnchor->getActive() ? lastSourceAnchor->getActiveColor() : lastSourceAnchor->getLinkColor());
                    text.setStyle(lastSourceAnchor->getActive() && lastSourceAnchor->getUnderlined() ? formattedTextSection->getStyle() | TextStyle::Underlined : formattedTextSection->getStyle());
                }
                else
                {
                    text.setColor(formattedTextSection->getColor());
                    text.setStyle(formattedTextSection->getStyle());
                }

                target.drawText(states, text);
            }
            else if (formattedImage)
            {
                auto topleft = formattedImage->getLayoutLeftTop();
                if (formattedImage->getTexture().getData() != nullptr)
                {
                    Sprite sprite(formattedImage->getTexture());
                    sprite.setPosition(topleft);
                    sprite.setSize(Vector2f(formattedImage->getLogicaSize()));
                    target.drawSprite(states, sprite);
                }
            }
            else if (formattedRectangle)
            {
                if (lastSourceAnchor != nullptr)
                {
                    if (lastSourceAnchor->getLayoutBottom() < formattedRectangle->getLayoutTop() ||
                        lastSourceAnchor->getLayoutTop() > formattedRectangle->getLayoutBottom())
                        lastSourceAnchor = nullptr;
                }
                if (formattedRectangle->getContentOrigin() != nullptr && formattedRectangle->getContentOrigin()->getTypeName() == MarkupLanguageElement::Anchor)
                    lastSourceAnchor = std::dynamic_pointer_cast<FormattedLink>(formattedElement);

                if (formattedRectangle->getOpacity() > 0.0f)
                {
                    ///////////////
                    // 0-------1 //
                    // |       | //
                    // |       | //
                    // 3-------2 //
                    ///////////////

                    auto color = Vertex::Color(formattedRectangle->getBackgroundColor().getRed(),
                                               formattedRectangle->getBackgroundColor().getGreen(),
                                               formattedRectangle->getBackgroundColor().getBlue(),
                                               std::max(std::min((int)(formattedRectangle->getOpacity() * formattedRectangle->getBackgroundColor().getAlpha()), 255), 0));
                    const std::array<Vertex, 4> vertices = {{
                        {{formattedRectangle->getLayoutLeft(),  formattedRectangle->getLayoutTop()   }, color},
                        {{formattedRectangle->getLayoutLeft(),  formattedRectangle->getLayoutBottom()}, color},
                        {{formattedRectangle->getLayoutRight(), formattedRectangle->getLayoutTop()   }, color},
                        {{formattedRectangle->getLayoutRight(), formattedRectangle->getLayoutBottom()}, color}
                    }};
                    const std::array<unsigned int, 2*3> indices = {{
                        0, 1, 2,
                        1, 3, 2
                    }};
                    target.drawVertexArray(states, vertices.data(), vertices.size(), indices.data(), indices.size(), nullptr);
                    //target.drawTriangle(states, vertices[0], vertices[1], vertices[2]);
                    //target.drawTriangle(states, vertices[1], vertices[3], vertices[2]);
                }
                if (!formattedRectangle->getBoderWidth().isEmpty(innerSize))
                {
                    //////////////////////
                    // 0---1----------6 //
                    // |              | //
                    // |   8------7   | //
                    // |   |      |   | //
                    // |   |      |   | //
                    // |   3------5   | //
                    // |              | //
                    // 2--------------4 //
                    //////////////////////
                    Outline borderWidth(std::max(1.0f, formattedRectangle->getBoderWidth().left),  std::max(1.0f, formattedRectangle->getBoderWidth().top),
                                        std::max(1.0f, formattedRectangle->getBoderWidth().right), std::max(1.0f, formattedRectangle->getBoderWidth().bottom));
                    auto color = Vertex::Color(formattedRectangle->getBorderColor().getRed(),
                                               formattedRectangle->getBorderColor().getGreen(),
                                               formattedRectangle->getBorderColor().getBlue(),
                                               std::max(std::min((int)(formattedRectangle->getOpacity() * formattedRectangle->getBorderColor().getAlpha()), 255), 0));
                    Outline borderArea{ formattedRectangle->getLayoutLeft()   + formattedRectangle->getBoderMargin().left,
                                        formattedRectangle->getLayoutTop()    + formattedRectangle->getBoderMargin().top,
                                        formattedRectangle->getLayoutRight()  - formattedRectangle->getBoderMargin().right,
                                        formattedRectangle->getLayoutBottom() - formattedRectangle->getBoderMargin().bottom };
                    const std::array<Vertex, 9> vertices = {{
                        {{borderArea.getLeft(),                           borderArea.getTop()                             }, color},
                        {{borderArea.getLeft()  + borderWidth.getLeft(),  borderArea.getTop()                             }, color},
                        {{borderArea.getLeft(),                           borderArea.getBottom()                          }, color},
                        {{borderArea.getLeft()  + borderWidth.getLeft(),  borderArea.getBottom() - borderWidth.getBottom()}, color},
                        {{borderArea.getRight(),                          borderArea.getBottom()                          }, color},
                        {{borderArea.getRight() - borderWidth.getRight(), borderArea.getBottom() - borderWidth.getBottom()}, color},
                        {{borderArea.getRight(),                          borderArea.getTop()                             }, color},
                        {{borderArea.getRight() - borderWidth.getRight(), borderArea.getTop()    + borderWidth.getTop()   }, color},
                        {{borderArea.getLeft()  + borderWidth.getLeft(),  borderArea.getTop()    + borderWidth.getTop()   }, color}
                    }};
                    const std::array<unsigned int, 8*3> indices = {{
                        0, 2, 1,
                        1, 2, 3,
                        2, 4, 3,
                        3, 4, 5,
                        4, 6, 5,
                        5, 6, 7,
                        6, 1, 7,
                        7, 1, 8
                    }};
                    target.drawVertexArray(states, vertices.data(), vertices.size(), indices.data(), indices.size(), nullptr);
                }
            }
        }

        target.removeClippingLayer();

        // Draw the scrollbars if needed
        if (m_verticalScrollbar->isShown())
            m_verticalScrollbar->draw(target, statesForScrollbar);

        if (m_horizontalScrollbar->isShown())
            m_horizontalScrollbar->draw(target, statesForScrollbar);
   }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f FormattedTextView::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    Vector2f FormattedTextView::getRenderSize() const
    {
        auto innerSize = getInnerSize();
        return {std::max(0.f, innerSize.x - m_paddingCached.getLeft() - m_paddingCached.getRight() - (m_verticalScrollbar->isShown() ? m_verticalScrollbar->getSize().x : 0.0f)),
                std::max(0.f, innerSize.y - m_paddingCached.getTop() - m_paddingCached.getBottom() - (m_horizontalScrollbar->isShown() ? m_horizontalScrollbar->getSize().y : 0.0f))};
    }

}
