/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// XHTML entity resolver (zlib license) provided for TGUI - Texus' Graphical User Interface
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This resolver is heavily inspired by the great work 'HTML Reader C++ Class Library', version dated 29 March 2004
// (https://www.codeproject.com/Articles/6561/HTML-Reader-C-Class-Library), written by 'Gurmeet S. Kochar'
// <gomzygotit@hotmail.com> aka 'gUrM33T' (https://www.codeproject.com/script/Membership/View.aspx?mid=547523).
//
// Unfortunately, the original code is not cross-platform (uses MS Windows data types instead of STL data types) and has an
// customized infecting license. For these reasons, the resolver included here is a complete re-implementation.
//
// Nevertheless, a visit to the original is worthwhile in any case.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TINY_XHTML_ENTRITY_RESOLVER
#define TINY_XHTML_ENTRITY_RESOLVER

#include <tuple>
#include <vector>

namespace tgui  { namespace xhtml
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The enumeration of message types
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    enum class MessageType
    {
        ERROR,   //!< The message represents an error
        WARNING  //!< The message represents an warning
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Provides an entity map, that maps encoded entity names to decoded entities
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class XhtmlEntityResolver
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Find the first decoded entity, that is mapped to the indicated encoded entity name
        ///
        /// @param messages       The collection of error/warning messages, created during the parser run
        /// @param encode         The encoded entity name to be resolved
        /// @param substitute     The encoded entity that is mapped to an encoded entity name
        ///
        /// @return The number of processed character in encode on success, or 0 otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static unsigned int resolveEntity(std::vector<std::tuple<tgui::xhtml::MessageType, String>>& messages, const char32_t* encode, char32_t& substitute);

    private:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Interprets an unsigned long integer value in the indicated string
        ///
        /// @param value  The string to be interpreted
        /// @param radix  The number base
        ///
        /// @return The value converted to unsigned long integer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static unsigned long toULong(const char32_t* value, int radix = 10);

    private:
        static std::map<tgui::String, char32_t> m_charEntityRefs; /**< The map of encoded entity names to decoded entities */
    };

} }

#endif    // TINY_XHTML_ENTRITY_RESOLVER
