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
        Widget{typeName, false}, m_document(), m_bordersCached(0), m_paddingCached(2),
        m_borderColorCached(Color::Transparent), m_backgroundColorCached(Color::Transparent),
        m_horizontalScrollbar(), m_verticalScrollbar(), m_fontCollection()
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
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        updateScrollbars();

        // The size of the XHTML static area has changed, update the content layout
        rearrangeText(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FormattedTextView::mouseWheelScrolled(float delta, Vector2f pos)
    {
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool FormattedTextView::isMouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::leftMousePressed(Vector2f pos)
    {
        Widget::leftMousePressed(pos);

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
        else // The click occurred on the text view
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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedTextView::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        // The mouse has moved so a double click is no longer possible
        m_possibleDoubleClick = false;

        // Check if the mouse event should go to the vertical scroll-bar
        if (m_verticalScrollbar->isShown() &&
            ((m_verticalScrollbar->isMouseDown() && m_verticalScrollbar->isMouseDownOnThumb()) || m_verticalScrollbar->isMouseOnWidget(pos)))
        {
            m_verticalScrollbar->mouseMoved(pos);
            recalculateVisiblePart();
        }
        // Check if the mouse event should go to the horizontal scroll-bar
        else if (m_horizontalScrollbar->isShown() &&
                 ((m_horizontalScrollbar->isMouseDown() && m_horizontalScrollbar->isMouseDownOnThumb()) || m_horizontalScrollbar->isMouseOnWidget(pos)))
        {
            m_horizontalScrollbar->mouseMoved(pos);
        }
        // If the mouse is held down then you are selecting text
        else if (m_mouseDown)
        {
        }
        // Inform the scrollbars that the mouse is not on them
        else
        {
            m_verticalScrollbar->mouseNoLongerOnWidget();
            m_horizontalScrollbar->mouseNoLongerOnWidget();
        }
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
        Widget::leftMouseButtonNoLongerDown();

        if (m_verticalScrollbar->isShown())
            m_verticalScrollbar->leftMouseButtonNoLongerDown();

        if (m_horizontalScrollbar->isShown())
            m_horizontalScrollbar->leftMouseButtonNoLongerDown();
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

        Widget::setFocused(focused);
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
        float zoom = 1.0f; // 0.6f;

        if (m_document)
        {
            m_document->layout(renderSize, m_textSizeCached * zoom, getSharedRenderer()->getDefaultTextColor(), m_opacityCached,
                               m_fontCollection, keepSelection);
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
        {
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

            for (auto formattedElement : m_document->getContent())
            {
                // FormattedTextSection* pFormattedTextSection = dynamic_cast<FormattedTextSection*> &(formattedElement->get());
                auto pFormattedTextSection = std::dynamic_pointer_cast<FormattedTextSection>(formattedElement);
                auto pFormattedImage = std::dynamic_pointer_cast<FormattedImage>(formattedElement);
                auto pFormattedRectangle = std::dynamic_pointer_cast<FormattedRectangle>(formattedElement);
                if (pFormattedTextSection)
                {
                    Text text;
                    text.setString(pFormattedTextSection->getString());
                    text.setFont(pFormattedTextSection->getFont());
                    text.setCharacterSize(pFormattedTextSection->getCharacterSizeAsInt());
                    text.setPosition({pFormattedTextSection->getRenderLeft(),  pFormattedTextSection->getRenderTop()});
                    text.setOpacity(pFormattedTextSection->getOpacity());
                    text.setColor(pFormattedTextSection->getColor());
                    text.setStyle(pFormattedTextSection->getStyle());

                    target.drawText(states, text);
                }
                else if (pFormattedImage)
                {
                    auto topleft = pFormattedImage->getRenderLeftTop();
                    if (pFormattedImage->getTexture().getData() != nullptr)
                    {
                        Sprite sprite(pFormattedImage->getTexture());
                        sprite.setPosition(topleft);
                        sprite.setSize(Vector2f(pFormattedImage->getLogicaSize()));
                        target.drawSprite(states, sprite);
                    }
                }
                else if (pFormattedRectangle)
                {
                    if (pFormattedRectangle->getOpacity() > 0.0f)
                    {
                        ///////////////
                        // 0-------1 //
                        // |       | //
                        // |       | //
                        // 3-------2 //
                        ///////////////

                        auto color = Vertex::Color(pFormattedRectangle->getBackgroundColor().getRed(),
                                                   pFormattedRectangle->getBackgroundColor().getGreen(),
                                                   pFormattedRectangle->getBackgroundColor().getBlue(),
                                                   std::max(std::min((int)(pFormattedRectangle->getOpacity() * pFormattedRectangle->getBackgroundColor().getAlpha()), 255), 0));
                        const std::array<Vertex, 9> vertices = {{
                            {{pFormattedRectangle->getRenderLeft(),  pFormattedRectangle->getRenderTop()   }, color},
                            {{pFormattedRectangle->getRenderLeft(),  pFormattedRectangle->getRenderBottom()}, color},
                            {{pFormattedRectangle->getRenderRight(), pFormattedRectangle->getRenderTop()   }, color},
                            {{pFormattedRectangle->getRenderRight(), pFormattedRectangle->getRenderBottom()}, color}
                        }};
                        const std::array<unsigned int, 2*3> indices = {{
                            0, 1, 2,
                            1, 3, 2
                        }};
                        target.drawVertexArray(states, vertices.data(), vertices.size(), indices.data(), indices.size(), nullptr);
                        //target.drawTriangle(states, vertices[0], vertices[1], vertices[2]);
                        //target.drawTriangle(states, vertices[1], vertices[3], vertices[2]);
                    }
                    if (!pFormattedRectangle->getBoderWidth().isEmpty(innerSize))
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
                        Borders borders(1.0f, 1.0f, 1.0f, 1.0f);
                        auto color = Vertex::Color(pFormattedRectangle->getBorderColor().getRed(),
                                                   pFormattedRectangle->getBorderColor().getGreen(),
                                                   pFormattedRectangle->getBorderColor().getBlue(),
                                                   std::max(std::min((int)(pFormattedRectangle->getOpacity() * pFormattedRectangle->getBorderColor().getAlpha()), 255), 0));
                        const std::array<Vertex, 9> vertices = {{
                            {{pFormattedRectangle->getRenderLeft(),                       pFormattedRectangle->getRenderTop()                         }, color},
                            {{pFormattedRectangle->getRenderLeft()  + borders.getLeft(),  pFormattedRectangle->getRenderTop()                         }, color},
                            {{pFormattedRectangle->getRenderLeft(),                       pFormattedRectangle->getRenderBottom()                      }, color},
                            {{pFormattedRectangle->getRenderLeft()  + borders.getLeft(),  pFormattedRectangle->getRenderBottom() - borders.getBottom()}, color},
                            {{pFormattedRectangle->getRenderRight(),                      pFormattedRectangle->getRenderBottom()                      }, color},
                            {{pFormattedRectangle->getRenderRight() - borders.getRight(), pFormattedRectangle->getRenderBottom() - borders.getBottom()}, color},
                            {{pFormattedRectangle->getRenderRight(),                      pFormattedRectangle->getRenderTop()                         }, color},
                            {{pFormattedRectangle->getRenderRight() - borders.getRight(), pFormattedRectangle->getRenderTop()    + borders.getTop()   }, color},
                            {{pFormattedRectangle->getRenderLeft()  + borders.getLeft(),  pFormattedRectangle->getRenderTop()    + borders.getTop()   }, color}
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
       }

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
