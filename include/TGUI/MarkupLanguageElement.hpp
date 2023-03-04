/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Formatted elements (zlib license) provided for TGUI - Texus' Graphical User Interface
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


#ifndef TGUI_MARKUP_LANGUAGE_ELEMENT_HPP
#define TGUI_MARKUP_LANGUAGE_ELEMENT_HPP

namespace tgui
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The markup language element.
    ///
    /// The generic base class for markup (XHTML, markdown, RTF, ...) language elements.
    /// Specialized markup language elements are derived from this class.
    ///
    /// This class defines abstract methods, that are at least to implement by dericed (specialized) markup language elements
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API MarkupLanguageElement
    {
    public:
        using Ptr = std::shared_ptr<MarkupLanguageElement>; //!< Shared markup language element (abstract base) pointer
        using ConstPtr = std::shared_ptr<const MarkupLanguageElement>; //!< Shared constant markup language element (abstract base) pointer

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the element ID or an empty string
        ///
        /// @return The element ID on success or an empty string otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const tgui::String& getId() const = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the element name or an empty string
        ///
        /// @return The element name on success or an empty string otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const tgui::String& getName() const = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the type name of the element
        ///
        /// @return The type name of the element
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const char* getTypeName() const = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets an empty string
        ///
        /// @return The empty string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static const tgui::String& getEmptyString();
    };

}

#endif // TGUI_MARKUP_LANGUAGE_ELEMENT_HPP
