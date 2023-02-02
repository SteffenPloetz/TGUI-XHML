/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Generic file helper (zlib license, or BOOST license alternatively) provided for TGUI - Texus' Graphical User Interface
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


#ifndef TGUI_FILE_HELPER_HPP
#define TGUI_FILE_HELPER_HPP

#include <iostream>
#include <fstream>
#include <vector>

namespace tgui
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Generic file helper methods
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API FileHelper
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Reads a binary file to an unsigned character vector
        ///
        /// @param path  The path to the binary file to read
        /// @param data  The buffer to read the data to
        ///
        /// @return The true flag on success, or the false flag otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool readBinaryFile(const std::string& path, std::vector<unsigned char>& data)
        {
            std::ifstream fileStram(path, std::ios::in | std::ios::binary);
            if (fileStram.is_open())
            {
                fileStram.seekg (0, std::ios::end);
                size_t fileSizeInByte = fileStram.tellg();
                fileStram.seekg (0, std::ios::beg);
                data.resize(fileSizeInByte);
                fileStram.read((char*)(&data[0]), fileSizeInByte);

                return true;
            }
            return false;
        }
    };
}

#endif // TGUI_FILE_HELPER_HPP
