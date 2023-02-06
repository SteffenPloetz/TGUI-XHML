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
    class TGUI_API u32string
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Provides an empty string
        /// @return An empty string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static const std::u32string& emptyString()
        {   return m_emptyString;   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks if the given wide character is a white-space character as classified by the currently installed C locale
        ///
        /// ATTENTION: Since Win32 stores unicode using a wchar_t of 16 bit (while all others use a wchar_t of 32 bit) data
        /// loss can happen here! For use on Win32 deeper investigation might be necessary.
        ///
        /// @param character  The character to be checked
        ///
        /// @return Non-zero value if the wide character is a whitespace character, zero otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static inline int isSpace(char32_t character)
        {   return std::iswspace((wchar_t)character);   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks if the given wide character is an alphabetic character (either upper case or lower case)
        ///
        /// ATTENTION: Since Win32 stores unicode using a wchar_t of 16 bit (while all others use a wchar_t of 32 bit) data
        /// loss can happen here! For use on Win32 deeper investigation might be necessary.
        ///
        /// @param character  The character to be checked
        ///
        /// @return Non-zero value if the wide character is an alphabetic character, zero otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static inline int isAlpha(char32_t character)
        {   return std::iswalpha((wchar_t)character);   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks if the given wide character is an alphabetic (either upper case or lower case) or numeric character
        ///
        /// ATTENTION: Since Win32 stores unicode using a wchar_t of 16 bit (while all others use a wchar_t of 32 bit) data
        /// loss can happen here! For use on Win32 deeper investigation might be necessary.
        ///
        /// @param character  The character to be checked
        ///
        /// @return Non-zero value if the wide character is an alphabetic or numeric character, zero otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static inline int isAlnum(char32_t character)
        {   return std::iswalnum((wchar_t)character);   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks if the given wide character is an numeric character
        ///
        /// ATTENTION: Since Win32 stores unicode using a wchar_t of 16 bit (while all others use a wchar_t of 32 bit) data
        /// loss can happen here! For use on Win32 deeper investigation might be necessary.
        ///
        /// @param character  The character to be checked
        ///
        /// @return Non-zero value if the wide character is an numeric character, zero otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static inline int isNum(char32_t character)
        {   return (character >= U'0' && character <= U'9');   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks if the given wide character is an upper case character
        ///
        /// ATTENTION: Since Win32 stores unicode using a wchar_t of 16 bit (while all others use a wchar_t of 32 bit) data
        /// loss can happen here! For use on Win32 deeper investigation might be necessary.
        ///
        /// @param character  The character to be checked
        ///
        /// @return Non-zero value if the wide character is an upper case character, zero otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static inline int isUpper(char32_t character)
        {   return std::iswupper((wchar_t)character);   }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether indicated strings are case insensitive lighter/equal/greater
        ///
        /// @param left   The string to be compared with
        /// @param right  The string to compare against
        ///
        /// @return A value < 0, if left string is lighter than right string, or 0 if left string is equal to right string,
        /// or > 1 if left string is greater than right string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static int compareNoCase(const std::u32string& left, const std::u32string& right);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether indicated string contains the pattern case insensitive
        ///
        /// @param left     The string to be compared with the predicates
        /// @param pattern  The string to check containment
        ///
        /// @return The flag indicating whether pattern is contained (true) or not (false)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool containsNoCase(const std::u32string& nocaseLeft, const std::u32string& pattern);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether indicated string contains case insensitive one of the predicates
        ///
        /// @param left      The string to be compared with the predicates
        /// @param patterns  The strings to check containment
        ///
        /// @return The flag indicating whether one of the predicates is contained (true) or not (false)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool containsAnyNoCase(const std::u32string& nocaseLeft, const std::vector<std::u32string>& patterns);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes leading white space characters in-place
        ///
        /// @param text  The string to be trimmed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void trimLeft(std::u32string& text);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes tailing white space characters in-place
        ///
        /// @param text  The string to be trimmed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void trimRight(std::u32string& text);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes all occurrences of a pattern in-place
        ///
        /// @param text     The string to remove all occurrences of the pattern from
        /// @param pattern  The pattern to be removed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void remove(std::u32string& text, char32_t pattern);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Replaces all occurrences of a pattern against a replacer in-place
        ///
        /// @param text       The string to replace all occurrences of the pattern in
        /// @param pattern    The pattern to be replaced
        /// @param replacer   The replacer for the pattern
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void replace(std::u32string& text, char32_t pattern, char32_t replacer);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Replaces all occurrences of a pattern against a replacer in-place
        ///
        /// @param text       The string to replace all occurrences of the pattern in
        /// @param pattern    The pattern to be replaced
        /// @param replacer   The replacer for the pattern
        /// @param recursive  The flag whether to repeat replacement if application of replacer created a new pattern occurence
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void replace(std::u32string& text, const std::u32string& pattern, const std::u32string& replacer, bool recursive = false);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Find the first occurrence of the pattern
        ///
        /// @param text      The string to find the pattern in
        /// @param pattern   The pattern to be found
        /// @param startPos  The search start position in text
        ///
        /// @return The zero-based index, including the offset set by startPos, of the first position that matches the pattern
        ///         on success, or SIZE_MAX otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static size_t find(const std::u32string& text, const std::u32string& pattern, const size_t startPos = 0);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Find the first occurrence of the pattern
        ///
        /// @param text      The string to find the pattern in
        /// @param pattern   The pattern to be found
        /// @param startPos  The search start position in text
        ///
        /// @return The zero-based index, including the offset set by startPos, of the first position that matches the pattern
        ///         on success, or SIZE_MAX otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static size_t find(const std::u32string& text, const char32_t pattern, const size_t startPos = 0);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Split the indicated text on indicated separator
        ///
        /// @param text           The string split
        /// @param separator      The separator to use as split delimiter
        /// @param suppressEmpty  The flag indicating whether to suppress empty split parts
        ///
        /// @return The collection of split strings
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::vector<std::u32string> split(const std::u32string& s, char32_t separator, bool suppressEmpty = false);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Split the indicated text on indicated separator
        ///
        /// @param text       The string split
        /// @param separator  The separator to use as split delimiter
        /// @param suppressEmpty  The flag indicating whether to suppress empty split parts
        ///
        /// @return The collection of split strings
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::vector<std::u32string> split(const std::u32string& s, const std::u32string& separator, bool suppressEmpty = false);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Simplifies a wide character string to a legacy string
        ///
        /// @param data  The string to be simplified (might cause data loss)
        ///
        /// @return The simplified legacy string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::string stringfromu32string(const std::u32string& text);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts a legacy string to a wide character string
        ///
        /// @param data  The legacy string to be converted
        ///
        /// @return The converted wide character string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::u32string u32stringfromstring(const char* text);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts a UTF-8 byte stream to a wide character string
        ///
        /// @param data  The UTF-8 byte stream to be converted, MUST end with a '\0'
        ///
        /// @return The converted wide character string
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::u32string u32stringfromUtf8(const std::vector<char>& data);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Interprets a float value in the indicated string
        ///
        /// @param value  The string to be interpreted
        ///
        /// @return The value converted to float
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static float tof(const char32_t* value);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Interprets an unsigned integer value in the indicated string
        ///
        /// @param value  The string to be interpreted
        /// @param radix  The number base
        ///
        /// @return The value converted to unsigned integer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static unsigned long toul(const char32_t* value, int radix = 10);

    private:
        static const std::u32string m_emptyString;
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
