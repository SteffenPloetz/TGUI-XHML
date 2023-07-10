#include <iostream>
#include <memory>
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
#include "TGUI/Xhtml/Dom/XhtmlParser.hpp"

namespace tgui  { namespace xhtml
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool XhtmlParser::parseComment(tgui::String& comment)
    {
        const tgui::String commentEnd(U"--");
        if (m_bufferPos + 4 > m_buffer.size())
            return false;

        if (m_buffer[m_bufferPos] != '<' || m_buffer[m_bufferPos + 1] != '!' || m_buffer[m_bufferPos + 2] != '-' || m_buffer[m_bufferPos + 3] != '-')
            return false;

        size_t startPosition = m_bufferPos + 4;
        size_t workPosition = m_buffer.find(commentEnd, startPosition);
        if (workPosition == tgui::String::npos)
        {
            tgui::String message(U"XhtmlParser::parseComment() -> Probably not closed comment tag, starting at position: ");
            message.append(tgui::String::fromNumber(m_bufferPos));
            m_messages.push_back(std::make_tuple(MessageType::ERROR, message));

            comment = m_buffer.substr(startPosition);
            m_bufferPos += 4 + comment.size();
            return true;
        }

        comment = m_buffer.substr(startPosition, workPosition - startPosition);

        workPosition += 2;
        if (workPosition >= m_buffer.size())
        {
            tgui::String message(U"XhtmlParser::parseComment() -> Probably incomplete character stream at the end of comment tag, starting at position: ");
            message.append(tgui::String::fromNumber(m_bufferPos));
            m_messages.push_back(std::make_tuple(MessageType::ERROR, message));

            return false;
        }

        while (tgui::isWhitespace(m_buffer[workPosition]))
            workPosition++;

        if(m_buffer[workPosition] != '>')
        {
            tgui::String message(U"XhtmlParser::parseComment() -> Probably not accurate closed comment tag, starting at position: ");
            message.append(tgui::String::fromNumber(m_bufferPos));
            m_messages.push_back(std::make_tuple(MessageType::ERROR, message));

            m_bufferPos = workPosition;
            return false;
        }

        m_bufferPos = workPosition + 1;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    size_t XhtmlParser::parseDocument(bool resolveEntities, bool traceResult)
    {
        if (m_buffer.empty())
            return false;

        char32_t                       workCharacter = acquireChar(); //!< Current character to evaluate
        size_t	                       workStartPosition = 0;         //!< Start position of character data
        size_t                         workDataLen = 0;               //!< Length of character data
        tgui::String                   characters;                    //!< Last collection of unassociated characters
        tgui::String                   comment;                       //!< Last comment text buffer
        bool                           isOpeningTag = false;          //!< Determine whether current character position is inside opening tag
        bool                           isClosingTag = false;          //!< Determine whether current character position is inside closing tag
        bool                           isInsideScript = false;        //!< Determine whether current character position is between '<script ...>' and '</script>' tags
        int                            preformattedDepth = 0;         //!< Count the currently open '<pre ...>' tags
        std::vector<XhtmlElement::Ptr> parentElements;

        // set a defined element tree root to prevent problems with parentElements.back()
        parentElements.push_back(nullptr);

        while (tgui::isWhitespace(workCharacter))
            workCharacter = acquireChar();
        rejectChar();

        while ((workCharacter = acquireChar()) != 0)
        {
            switch(workCharacter)
            {
                // handle tags initiating characters
                case '<':
                {
                    // we can't assume that the next '<' following a '<script ...>' is a tag start - it
                    // can also be a part of the code, e.g. a compare operator or a tag within the code

                    XhtmlElement::Ptr newElement = nullptr;
                    rejectChar();

                    // try to recognize a comment tag prior an element tag, because comment tags can not be nested and are easier
                    comment.clear();
                    if (!parseComment(comment))
                    {
                        isOpeningTag        = false;
                        isClosingTag        = false;

                        // try to recognize element tag, if recognition of a comment tag failed
                        if ((newElement = parseElement(nullptr, isOpeningTag, isClosingTag, isInsideScript)) != nullptr)
                        {
                            // [1 - see below] postpone the registation of the 'newElement' to the parent/root element collection here
                            // (there might be remaining 'characters' that must be processed first)

                            if (newElement->isAutoClosedTagByDefinition())
                            {
                                isOpeningTag = true;
                                isClosingTag = true;
                            }
                            // special script mode handling
                            if (isOpeningTag && !isInsideScript)
                            {
                                if (tgui::viewEqualIgnoreCase(newElement->getTypeName(), "script"))
                                    if (!newElement->getParsedAsAutoClosed())
                                        isInsideScript = true;
                            }
                            if (isClosingTag && isInsideScript)
                            {
                                if (tgui::viewEqualIgnoreCase(newElement->getTypeName(), "script"))
                                    isInsideScript = false;
                            }
                        }
                        // neither comment nor element tag recognized, handle the current character like tag content
                        else
                        {
                            workDataLen++;
                            acquireChar();
                            break;
                        }
                    }

                    // prepare current character(s) for tag content, if any
                    if (workDataLen)
                        characters.append(m_buffer.substr(workStartPosition, workDataLen));
                    if (characters.size())
                    {
                        if (preformattedDepth == 0)
                            parseDocument_cleanBreaksAndSpaces(parentElements.back(), characters);
                        else
                            characters.replace(U"\r\n", U"\n");
                    }

                    // register current character(s) for tag content, if any
                    if (characters.size())
                    {
                        auto parentElement  = parentElements.back();
                        if (parentElement != nullptr)
                        {
                            auto parentStyle = std::dynamic_pointer_cast<XhtmlStyle>(parentElement);
                            auto parentContainer = std::dynamic_pointer_cast<XhtmlContainerElement>(parentElement);
                            if (parentStyle != nullptr)
                            {
                                parentStyle->createEntriesFromParseData(m_messages, characters);
                            }
                            else if (parentContainer != nullptr)
                            {
                                parseDocument_createInnerText(parentElement, characters, preformattedDepth);
                            }
                            else
                            {
                                tgui::String message(U"XhtmlParser::parseDocument() -> An inner text '");
                                message.append(characters).append(U"' should have a parent style element or parent container element, but parent element is of type '");
                                message.append(parentElement->getTypeNameU32()).append(U"'!");
                                m_messages.push_back(std::make_tuple(MessageType::ERROR, message));
                            }
                        }
                        else
                        {
                            tgui::String message(U"XhtmlParser::parseDocument() -> An inner text '");
                            message.append(characters).append(U"' should have a parent container element, but hasn't!");
                            m_messages.push_back(std::make_tuple(MessageType::ERROR, message));
                        }
                        characters.clear();
                    }

                    workDataLen = 0;
                    workStartPosition = m_bufferPos;

                    // update element hierarchy
                    if (comment.size())
                    {
                        ; // NOP
                    }
                    else if (newElement == nullptr)
                    {
                        tgui::String message(U"XhtmlParser::parseDocument() -> An XHTML element should be created, but isn't!");
                        m_messages.push_back(std::make_tuple(MessageType::ERROR, message));
                    }
                    else
                    {
                        auto parentElement = parentElements.back();
                        if (isOpeningTag) // includs: isOpeningTag && isClosingTag
                        {
                            // [1 - see above] catch up the postponed registration to the parent/root element collection here
                            if (parentElement == nullptr)
                                m_rootElements.push_back(newElement);
                            else
                            {
                                auto parentContainer = std::dynamic_pointer_cast<XhtmlContainerElement>(parentElement);
                                XhtmlElement::addChildAndSetPatent(parentContainer, newElement);
                            }

                            if (!isClosingTag)
                                parentElements.push_back(newElement);
                        }
                        else if (isClosingTag)
                        {
                            if (parseDocument_checkStartEndTagParity(parentElement, newElement))
                                parentElements.pop_back();
                        }

                        // special preformatted mode handling
                        if (isOpeningTag && tgui::viewEqualIgnoreCase(newElement->getTypeName(), "pre"))
                            preformattedDepth++;
                        if (isClosingTag && tgui::viewEqualIgnoreCase(newElement->getTypeName(), "pre"))
                            preformattedDepth--;
                        if (preformattedDepth < 0)
                            preformattedDepth = 0;
                    }

                    break;
                }

                // handle entities (HTML special characters) initiating characters
                case '&':
                {
                    workCharacter = rejectChar();

                    unsigned int charOffset = 0;
                    if (resolveEntities)
                        charOffset = XhtmlEntityResolver::resolveEntity(m_messages, &m_buffer[m_bufferPos], workCharacter);

                    if (charOffset)
                    {
                        characters.append(m_buffer.substr(workStartPosition, workDataLen));
                        characters.append(workCharacter);
                        m_bufferPos += charOffset;
                        workStartPosition = m_bufferPos;
                        workDataLen = 0L;
                    }
                    else
                    {
                        workCharacter = acquireChar();
                        workDataLen++;
                    }

                    break;
                }

                // handle not specific characters like tag content
                default:
                {
                    workDataLen++;
                    break;
                }
            }
        };

        // do a simple quality check
        if (workDataLen || characters.size())
        {
            tgui::String message(U"XhtmlParser::parseDocument() -> There are remaining characters '");
            message.append(characters).append(U"' after the end of document parsing!");
            m_messages.push_back(std::make_tuple(MessageType::ERROR, message));
        }

        size_t htmlRootElements = 0;
        for (XhtmlElement::Ptr e : m_rootElements)
            if (tgui::viewEqualIgnoreCase(e->getTypeName(), "html"))
                htmlRootElements++;

        if (htmlRootElements > 1)
        {
            tgui::String message(U"XhtmlParser::parseDocument() -> There is more than 1 'html' root element, but shouldn't!");
            m_messages.push_back(std::make_tuple(MessageType::ERROR, message));
        }

        if (traceResult)
        {
            auto htmlElement = XhtmlElement::getFirstElement(m_rootElements, "html");
            if (htmlElement != nullptr)
                htmlElement->trace(L"");
        }

        return m_bufferPos;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void XhtmlParser::parseDocument_cleanBreaksAndSpaces(const XhtmlElement::Ptr parentElement, tgui::String& characters)
    {
        characters.replace(U"\r\n", U"\n");
        characters.replace(U'\n', U' ');
        characters.replace(U'\t', U' ');
        characters.replace(U'\v', U' ');
        while(characters.contains(U"  "))
            characters.replace(U"  ", U" ");
        characters.replace(U'\xA0', U'\x20');

        if (parentElement != nullptr && characters.size() > 0 && characters[0] == U' ')
        {
            auto previousElement = (parentElement->getChildren() != nullptr && parentElement->getChildren()->size() > 0 ? parentElement->getChildren()->back() : nullptr);

            if (previousElement == nullptr ||
                (previousElement != nullptr && (previousElement->isStructuringBlock() || previousElement->isTextBlock()) ))
            {
                characters.erase(0, 1);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void XhtmlParser::parseDocument_createInnerText(XhtmlElement::Ptr parentElement, tgui::String& characters, int preformattedDepth)
    {
        tgui::String match = U"";
        if (characters.find(U"\n", 0) != SIZE_MAX)
            match = U"\n";

        if (match.length() > 0)
        {
            auto charactersParts = characters.split(match);
            for (size_t index = 0; index < charactersParts.size(); index++)
            {
                auto charactersPart = charactersParts[index];

                if (preformattedDepth == 0)
                {
                    if (index == 0)
                        charactersPart = StringEx::trimRight(charactersPart);
                    else if (index == charactersParts.size() - 1)
                        charactersPart = StringEx::trimLeft(charactersPart);
                    else
                        charactersPart = charactersPart.trim();
                }

                if (charactersPart.size() > 0)
                    XhtmlElement::createInnerText(parentElement, charactersPart);
                if (index < charactersParts.size() - 1)
                    XhtmlElement::createBreak(parentElement);
            }
        }
        else
            XhtmlElement::createInnerText(parentElement, characters);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool XhtmlParser::parseDocument_checkStartEndTagParity(XhtmlElement::Ptr startTagElement, XhtmlElement::Ptr endTagElement)
    {
        if (startTagElement != nullptr)
        {
            auto startTagName = startTagElement->getTypeName();
            auto stopTagName = endTagElement->getTypeName();
            if (!tgui::viewEqualIgnoreCase(startTagName, stopTagName))
            {
                tgui::String message(U"XhtmlParser::parseDocument_checkStartEndTagParity() -> Can't close tag of type '");
                message.append(startTagElement->getTypeNameU32()).append(U"' with tag of type '");
                message.append(endTagElement->getTypeNameU32()).append(U"'!");
                m_messages.push_back(std::make_tuple(MessageType::WARNING, message));
            }

            // whether start tag and end tag match or not - there is a parity
            return true;
        }
        else
        {
            tgui::String message(U"XhtmlParser::parseDocument_checkStartEndTagParity() -> Can't find matching open tag of type '");
            message.append(endTagElement->getTypeNameU32()).append(U"' for current tag of type '");
            message.append(endTagElement->getTypeNameU32()).append(U"'!");
            m_messages.push_back(std::make_tuple(MessageType::WARNING, message));

            // there is no parity
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    XhtmlElement::Ptr XhtmlParser::parseElement(XhtmlElement::Ptr parent, bool& isOpeningTag, bool& isClosingTag, bool& isInsideScript)
    {
        if (m_buffer.size() == 0)
        {
            tgui::String message(U"XhtmlParser::parseElement() -> Invalid buffer!");
            m_messages.push_back(std::make_tuple(MessageType::ERROR, message));
            return nullptr;
        }
        if (m_bufferPos + 4 >= m_buffer.size())
        {
            tgui::String message(U"XhtmlParser::parseElement() -> Index overrun!");
            m_messages.push_back(std::make_tuple(MessageType::ERROR, message));
            return nullptr;
        }

        // we can't assume that the next '<' following a '<script ...>' is a tag start - it
        // can also be a part of the code, e.g. a compare operator or a tag within the code
        size_t processedLength;
        auto element = XhtmlElement::createElementFromParseStr(m_messages, parent, m_buffer, m_bufferPos, isOpeningTag, isClosingTag, isInsideScript, processedLength);
        if (!processedLength)
            return nullptr;

        m_bufferPos += processedLength;
        return element;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<std::vector<String>> XhtmlParser::getErrorMessages() const
    {
        auto errors = std::make_shared<std::vector<String>>();
        for (XhtmlParser::MsgType m : m_messages)
            if (std::get<0>(m) == MessageType::ERROR)
                errors->push_back(std::get<1>(m));
        return errors;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<std::vector<String>> XhtmlParser::getWarningMessages() const
    {
        auto warnings = std::make_shared<std::vector<String>>();
        for (XhtmlParser::MsgType m : m_messages)
            if (std::get<0>(m) == MessageType::WARNING)
                warnings->push_back(std::get<1>(m));
        return warnings;
    }

} }
