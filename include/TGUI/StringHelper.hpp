/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Generic string helper (zlib license, or BOOST license alternatively) provided for TGUI - Texus' Graphical User Interface
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


#ifndef EXT_STRING_HPP
#define EXT_STRING_HPP

#include <cwctype>
#include <string>
#include <vector>
#include <stdint.h>

namespace ext
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Provides string convenience helper
    ///
    /// Provides convenience helper for string manipulation.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API String
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Split the indicated text on indicated separator
        ///
        /// @param text           The string split
        /// @param separator      The separator to use as split delimiter
        /// @param suppressEmpty  The flag indicating whether to suppress empty split parts
        ///
        /// @return The collection of split strings
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::vector<tgui::String> split(const tgui::String& s, char32_t separator, bool suppressEmpty = false);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Split the indicated text on indicated separator
        ///
        /// @param text       The string split
        /// @param separator  The separator to use as split delimiter
        /// @param suppressEmpty  The flag indicating whether to suppress empty split parts
        ///
        /// @return The collection of split strings
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::vector<tgui::String> split(const tgui::String& s, const tgui::String& separator, bool suppressEmpty = false);
    };

    class TGUI_API string
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Compare two legacy strings (re-implemented, besause this method is not available outside UNIX)
        ///
        /// @param s1  The string to be compared
        /// @param s2  The string to compare with
        ///
        /// @return The negative value if s1 is lighter or shorter than s2, the positive value if s1 is greater or longer s2,
        /// or 0 is s1 is equal to s2
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static int strcasecmp(const char* s1, const char* s2);
    };

}
#endif // EXT_STRING_HPP
