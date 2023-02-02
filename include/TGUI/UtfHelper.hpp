/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Generic UTF helper provided for TGUI - Texus' Graphical User Interface
// Copyright (C) 2023 Steffen Ploetz (Steffen.Ploetz@cityweb.de)
//
// The methods provided here have been distributed under various terms of use.
// The respective conditions are indicated directly above the method.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_UTF_HELPER_HPP
#define TGUI_UTF_HELPER_HPP

#include <cwctype>
#include <string>
#include <vector>

namespace tgui
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Provides UTF string convenience helper
    ///
    /// Provides convenience helper for UTF string manipulation.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API UtfHelper
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Origin:           http://www.zedwood.com/article/cpp-is-valid-utf8-string-function
        // Original name:    utf8_check_is_valid
        // Original author:  Not published
        // Original license: Creative Commons CC-By-SA 3.0
        //
        // LICENSE
        // =======
        //
        // You are free to:
        // ----------------
        // SHARE — copy and redistribute the material in any medium or format.
        // ADAPT — remix, transform, and build upon the material for any purpose, even commercially.
        //
        // The licensor cannot revoke these freedoms as long as you follow the license terms.
        //
        // Under the following terms:
        // --------------------------
        // ATTRIBUTION — You must give appropriate credit, provide a link to the license, and indicate if changes were made.
        // You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
        // SHARE ALIKE — If you remix, transform, or build upon the material, you must distribute your contributions under
        // the same license as the original.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Provides UTF string convenience helper
        ///
        /// Provides convenience helper for UTF string manipulation.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool checkValid(const std::vector<char>& data, bool utf8 = true);
    };

}
#endif // TGUI_UTF_HELPER_HPP
