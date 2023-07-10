/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Formatted document (zlib license) provided for TGUI - Texus' Graphical User Interface
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


#ifndef TGUI_FORMATTED_DOCUMENT_HPP
#define TGUI_FORMATTED_DOCUMENT_HPP

#include "TGUI/Xhtml/MarkupListItemType.hpp"
#include "TGUI/Xhtml/Widgets/FormattedElements.hpp"

namespace tgui  { namespace xhtml
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Helper to determine the best matching font pack from the most common font family names
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class DefaultFontFamilies
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether the font family name can be recognized as the name of a Serif font family
        ///
        /// @param left  The font family name
        ///
        /// @return The flag determining whether the font family name is recognized as the name of a Serif font family
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool isSerif(const String& familyName)
        {
            // This line will execute only on the first execution of this function:
            static std::vector<String> m_serif = { U"Times New Roman", U"Georgia", U"Garamond", U"Serif" };
            for (size_t index = 0; index < m_serif.size(); index++)
                if (familyName.contains(m_serif[index].c_str()))
                return true;
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether the font family name can be recognized as the name of a Sans font family
        ///
        /// @param left  The font family name
        ///
        /// @return The flag determining whether the font family name is recognized as the name of a Sans font family
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool isSans(const String& familyName)
        {
            static std::vector<String> m_sans = { U"Arial" U"Verdana" U"Helvetica" U"Sans-serif" };
            for (size_t index = 0; index < m_sans.size(); index++)
                if (familyName.contains(m_sans[index].c_str()))
                return true;
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Determines whether the font family name can be recognized as the name of a Mono font family
        ///
        /// @param left  The font family name
        ///
        /// @return The flag determining whether the font family name is recognized as the name of a Mono font family
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool isMono(const String& familyName)
        {
            static std::vector<String> m_mono = { U"Courier New", U"Lucida Console", U"Monaco", U"Monospace" };
            for (size_t index = 0; index < m_mono.size(); index++)
                if (familyName.contains(m_mono[index].c_str()))
                return true;
            return false;
        }
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief The formatted document
    ///
    /// The generic base class for formatted (XHTML, markdown, RTF, ...) data holder.
    /// Specialized formatted data holder are derived from this class.
    ///
    /// This class defines abstract methods, that are at least to implement by dericed (specialized) formatted data holder
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API FormattedDocument
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        using Ptr = std::shared_ptr<FormattedDocument>;            //!< Shared widget pointer
        using ConstPtr = std::shared_ptr<const FormattedDocument>; //!< Shared constant widget pointer

        static constexpr const char StaticWidgetType[] = "FormattedDocument";
        static constexpr const char32_t LinebreakDelimitercharacters[] = U"\r\n\t\v -";

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The font-pack supports the minimum variety of font-style (normal, italic/oblique) and font-weight (normal, bold)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API FontPack
        {
        public:
            Font Regular;    //!< The regular font of this font pack
            Font Bold;       //!< The bold font of this font pack. A dedicated bold font provides better display quality than an widened regular font
            Font Italic;     //!< The italic font of this font pack. A dedicated italic font provides better display quality than an slanted regular font
            Font BoldItalic; //!< The bold italic font of this font pack. A dedicated bold italic font provides better display quality than an widened and slanted regular font

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief The default constructor
            ///
            /// Initializes with the global font. Application should override initialization
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FontPack()
            {
                Regular    = tgui::Font::getGlobalFont();
                Bold       = tgui::Font::getGlobalFont();
                Italic     = tgui::Font::getGlobalFont();
                BoldItalic = tgui::Font::getGlobalFont();
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Asserts the validity of this font collection
            ///
            /// @return Either true on a valid font collection, or false otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            bool inline assertValid() const
            {
                return Regular != nullptr &&
                       Bold != nullptr &&
                       Italic != nullptr &&
                       BoldItalic != nullptr;
            }
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The font-collection supports the required minimum variety of font-faces (sans, serif, mono)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API FontCollection
        {
        public:
            std::shared_ptr<FontPack> Sans;  //!< The sand font pack of this font collection
            std::shared_ptr<FontPack> Serif; //!< The serif font pack of this font collection
            std::shared_ptr<FontPack> Mono;  //!< The monospace font pack of this font collection

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief The default constructor
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FontCollection()
                : Sans(), Serif(), Mono()
            {   ;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Asserts the validity of this font collection
            ///
            /// @return Either true on a valid font collection, or false otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            bool inline assertValid() const
            {
                return Sans != nullptr && Sans->assertValid() &&
                       Serif != nullptr && Serif->assertValid() &&
                       Mono != nullptr && Mono->assertValid();
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Returns the operation system specific font collection (currently optimized for Windows, Open Suse and Manjaro)
            ///
            /// The font collection includes the font faces sans/serif/mono and font styles regular/bold/italic/bold-italic
            ///
            /// @return The operation system specific font collection
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            static std::shared_ptr<FormattedDocument::FontCollection> platformOptimizedFontCollection();

        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The table-cell-data supports the individual specifications of a table-cell
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API TableCellMetric
        {
        public:
            typedef std::shared_ptr<TableCellMetric> Ptr; //!< Shared table cell data pointer

        public:
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the associated formatted element
            ///
            /// @param formattedElement  The associated formatted element
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFormattedElement(FormattedElement::Ptr formattedElement)
            {   m_formattedElement = formattedElement;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the associated formatted element
            ///
            /// @return  The associated formatted element on success, or nullptr otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline FormattedElement::Ptr getFormattedElement() const
            {   return m_formattedElement;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the content origin data element, that is to be (partly) represented by this formatted element
            ///
            /// @param origin  The new content origin data element to set
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setContentOrigin(MarkupLanguageElement::Ptr origin)
            {   m_contentOrigin = origin;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the origin data element, that is (partly) represented by this formatted element
            ///
            /// @return The origin data element
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline MarkupLanguageElement::Ptr getContentOrigin() const
            {   return m_contentOrigin;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the start coordinate
            ///
            /// @param startCoordinate  The start coordinate
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setStartCoordinate(float startCoordinate)
            {   m_startCoordinate = startCoordinate;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the start coordinate
            ///
            /// @return  The start coordinate
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline float getStartCoordinate() const
            {   return m_startCoordinate;   }

        private:
            FormattedElement::Ptr            m_formattedElement;       //!< The associated formatted element
            MarkupLanguageElement::Ptr       m_contentOrigin;          //!< The origin data element, that is (partly) represented by this formatted element
            float                            m_startCoordinate = 0.0f; //!< The start coordinate
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The table-row-data supports the individual specifications of a table-row
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API TableRowMetric
        {
        public:
            typedef std::shared_ptr<TableRowMetric> Ptr; //!< Shared table row data pointer

        public:
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Adds a table cell metric to a row
            ///
            /// @param tableCellMetric  The cell metric to add
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void addCellMetric(TableCellMetric::Ptr tableCellMetric)
            {   m_cellMetrics.push_back(tableCellMetric);   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the number of registered cell metrics
            ///
            /// @return  The number of registered cell metrics
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline size_t getCellMetricCount() const
            {   return m_cellMetrics.size();   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the cell metric of a cell
            ///
            /// @param index  The cell index to get the metric for
            ///
            /// @return  The cell metric of a cell on success, or nullptr otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline TableCellMetric::Ptr getCellMetric(size_t index) const
            {   return index < m_cellMetrics.size() ? m_cellMetrics[index] : nullptr;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the cell metric of the first cell that is associated to the indicated content origin data element
            ///
            /// @param origin  The content origin data element to get the cell metric for
            ///
            /// @return  The cell metric on success, or nullptr otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline TableCellMetric::Ptr firstOrDefaultCellMetricByContentOrigin(MarkupLanguageElement::Ptr origin) const
            {
                for (size_t index = 0; index < m_cellMetrics.size(); index++)
                    if (m_cellMetrics[index]->getContentOrigin() == origin)
                        return m_cellMetrics[index];

                return nullptr;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the cell index of a the first cell that is associated to the indicated content origin data element
            ///
            /// @param origin  The content origin data element to get the cell index for
            ///
            /// @return  The cell index on success, or 0 otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline size_t firstOrDefaultCellIndexByContentOrigin(MarkupLanguageElement::Ptr origin) const
            {
                for (size_t index = 0; index < m_cellMetrics.size(); index++)
                    if (m_cellMetrics[index]->getContentOrigin() == origin)
                        return index;

                return 0;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the associated formatted element
            ///
            /// @param formattedElement  The associated formatted element
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFormattedElement(FormattedElement::Ptr formattedElement)
            {   m_formattedElement = formattedElement;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the associated formatted element
            ///
            /// @return  The associated formatted element on success, or nullptr otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline FormattedElement::Ptr getFormattedElement() const
            {   return m_formattedElement;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the content origin data element, that is to be (partly) represented by this formatted element
            ///
            /// @param origin  The new content origin data element to set
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setContentOrigin(MarkupLanguageElement::Ptr origin)
            {   m_contentOrigin = origin;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the origin data element, that is (partly) represented by this formatted element
            ///
            /// @return The origin data element
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline MarkupLanguageElement::Ptr getContentOrigin() const
            {   return m_contentOrigin;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the start coordinate
            ///
            /// @param startCoordinate  The start coordinate
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setStartCoordinate(float startCoordinate)
            {   m_startCoordinate = startCoordinate;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the start coordinate
            ///
            /// @return  The start coordinate
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline float getStartCoordinate() const
            {   return m_startCoordinate;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the end coordinate
            ///
            /// @param endCoordinate  The end coordinate
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setEndCoordinate(float endCoordinate)
            {   m_endCoordinate = endCoordinate;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the end coordinate
            ///
            /// @return  The end coordinate
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline float getEndCoordinate() const
            {   return m_endCoordinate;   }

        private:
            FormattedElement::Ptr             m_formattedElement;        //!< The associated formatted element
            MarkupLanguageElement::Ptr        m_contentOrigin;           //!< The origin data element, that is (partly) represented by this formatted element
            std::vector<TableCellMetric::Ptr> m_cellMetrics;             //!< Specifications of the table cells
            float                             m_startCoordinate = -0.1f; //!< The start coordinate
            float                             m_endCoordinate = 0.0f;    //!< The end coordinate
        };

        enum class TableRowRange
        {
            Head,   //!< The head row range
            Body,   //!< The body row range
            Foot    //!< The foot row range
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The table-data supports the individual specifications of a table
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API TableMetric
        {
        public:
            typedef std::shared_ptr<TableMetric> Ptr; //!< Shared table data pointer

        public:
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief The default constructor
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            TableMetric()
                : m_columnRequestedSizes(), m_formattedElement(nullptr), m_contentOrigin(nullptr), m_currentRowRange(TableRowRange::Body),
                  m_headRowMetrics(), m_bodyRowMetrics(), m_footRowMetrics()
            {   ;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Updates the requested size of a column
            ///
            /// @param index             The column index of the column to update the requested size for
            /// @param updateSize        The requested size to update to
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void updateRequestedColumnSize(size_t index, OneDimSize updateSize);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the number of registered requested sizes of columns
            ///
            /// @return                  The number of registered requested sizes of columns
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline size_t getRequestedColumnSizesCount() const
            {   return m_columnRequestedSizes.size();   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the requested size of a column
            ///
            /// @param columnIndex       The column index of the column to get the requested size for
            ///
            /// @return                  The requested size of a column on success, or the default (SizeType::ViewportWidth, 0.0f)
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline OneDimSize getRequestedColumnSize(size_t columnIndex) const
            {
                // SizeType::ViewportWidth with 0.0f acts as a default.
                return (columnIndex < m_columnRequestedSizes.size()) ? m_columnRequestedSizes[columnIndex] : OneDimSize(SizeType::ViewportWidth, 0.0f);
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Updates the preferred size of a column
            ///
            /// @param columnIndex       The column index of the column to update the preferred size for
            /// @param updateSize        The preferred size to update to
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void updatePreferredColumnSize(size_t columnIndex, SizePreference updateSize);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the aggregated preferred size of all columns
            ///
            /// @return                  The aggregated preferred size of all columns
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            float getTotalPreferredColumnSizes() const;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the current row range
            ///
            /// @param currentRowRange   The row range to set as current
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setCurrentRowRange(TableRowRange currentRowRange)
            {   m_currentRowRange = currentRowRange;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the current row range
            ///
            /// @return                  The current row range
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline TableRowRange getCurrentRowRange() const
            {   return m_currentRowRange;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the number of rows for a row range
            ///
            /// @param rowRange          The row range to get the number of rows for
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            size_t getRowRangeRowMetricCount(TableRowRange rowRange) const;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Adds a row metric to a row range
            ///
            /// @param rowRange          The row range to add the row metric to
            /// @param tableRowMetric    The row metric to add
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void addRowRangeRowMetric(TableRowRange rowRange, TableRowMetric::Ptr tableRowMetric);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets a row metric of a row range
            ///
            /// @param rowRange          The row range to get the row metric from
            /// @param index             The index of the row metric to get
            ///
            /// @return                  The row metric on success, or nullptr otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            TableRowMetric::Ptr getRowRangeRowMetric(TableRowRange rowRange, size_t index) const;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the tailing row metric of a row range
            ///
            /// @param rowRange          The row range to get the tailing row metric from
            ///
            /// @return                  The row metric on success, or nullptr otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            TableRowMetric::Ptr getRowRangeTailRowMetric(TableRowRange rowRange) const;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the tailing row metric of the tailing row range
            ///
            /// @return                  The row metric on success, or nullptr otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            TableRowMetric::Ptr getTailRowMetric() const;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the associated formatted element
            ///
            /// @param formattedElement  The associated formatted element
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFormattedElement(FormattedElement::Ptr formattedElement)
            {   m_formattedElement = formattedElement;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the associated formatted element
            ///
            /// @return                  The associated formatted element on success, or nullptr otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline FormattedElement::Ptr getFormattedElement() const
            {   return m_formattedElement;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the content origin data element, that is to be (partly) represented by this formatted element
            ///
            /// @param origin            The new content origin data element to set
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setContentOrigin(MarkupLanguageElement::Ptr origin)
            {   m_contentOrigin = origin;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the origin data element, that is (partly) represented by this formatted element
            ///
            /// @return                  The origin data element
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline MarkupLanguageElement::Ptr getContentOrigin() const
            {   return m_contentOrigin;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the row metric of a the first row that is associated to the indicated content origin data element
            ///
            /// @param origin            The row content origin data element to get the row metric for
            /// @param rowIndex          [OUT] The row index on succes, or the number of rows otherwise
            ///
            /// @return                  The row metric on success, or nullptr otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            TableRowMetric::Ptr firstOrDefaultRowMetricByContentOrigin(MarkupLanguageElement::Ptr origin, size_t& rowIndex) const;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the row metric of a the first row that is associated to the indicated content origin data element
            ///
            /// @param origin            The row content origin data element to get the row metric for
            ///
            /// @return                  The row metric on success, or nullptr otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline TableRowMetric::Ptr firstOrDefaultRowMetricByContentOrigin(MarkupLanguageElement::Ptr origin) const
            {   size_t rowIndex;    return firstOrDefaultRowMetricByContentOrigin(origin, rowIndex);   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the row metric of a the first row that contains a cell that is associated to the indicated content origin data element
            ///
            /// @param origin            The cell content origin data element to get the row metric for
            /// @param rowIndex          [OUT] The row index on succes, or the number of rows otherwise
            ///
            /// @return                  The row metric on success, or nullptr otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            TableRowMetric::Ptr firstOrDefaultRowMetricByAnyCellContentOrigin(MarkupLanguageElement::Ptr origin, size_t& rowIndex) const;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the row metric of a the first row that contains a cell that is associated to the indicated content origin data element
            ///
            /// @param origin            The cell content origin data element to get the row metric for
            ///
            /// @return                  The row metric on success, or nullptr otherwise
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline TableRowMetric::Ptr firstOrDefaultRowMetricByAnyCellContentOrigin(MarkupLanguageElement::Ptr origin) const
            {   size_t rowIndex = 0;    return firstOrDefaultRowMetricByAnyCellContentOrigin(origin, rowIndex);   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the layout area. Must be done before before table columns are calculated
            ///
            /// @param cachedLayoutArea  The layout ares
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setCachedLayoutArea(FloatRect cachedLayoutArea)
            {   m_cachedLayoutArea = cachedLayoutArea;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the layout area
            ///
            /// @return                  The layout area
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline FloatRect getCachedLayoutArea() const
            {   return m_cachedLayoutArea;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the table to cell spacing. Must be done before table columns are calculated
            ///
            /// @param cachedLayoutArea  The table to cell spacing (margin + border + default spacing) from table's layout area
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setTableLayoutAreaToCellSpacing(Outline tableToCellSpacing)
            {   m_tableToCellSpacing = tableToCellSpacing;   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Gets the table margis
            ///
            /// @return  The table margin, fallback is 0.0f
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Outline getTableMargin() const;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Calculates the table render area (layout area + margin) to cell spacing
            ///
            /// @return  The table render area (layout area + margin) to cell spacing (border + default spacing), fallback is 0.0f
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Outline calculateTableRenderAreaToCellSpacing() const;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Calculates the table layout area to cell spacing
            ///
            /// @return  The table layout area to cell spacing (margin + border + default spacing), fallback is 0.0f
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Outline calculateTableLayoutAreaToCellSpacing() const;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Calculates the cell layout area for a cell
            ///
            /// @param columnIndex  The column index of the cell to calculate the cell layout area for
            ///
            /// @return  The cell layout area for a cell
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FloatRect calculateCellLayoutArea(size_t columnIndex) const;

        private:
            std::vector<OneDimSize>          m_columnRequestedSizes;   //!< The column requested sizes
            std::vector<SizePreference>      m_columnPreferredSizes;   //!< The column preferred sizes
            FormattedElement::Ptr            m_formattedElement;       //!< The associated formatted element
            MarkupLanguageElement::Ptr       m_contentOrigin;          //!< The origin data element, that is (partly) represented by this formatted element
            TableRowRange                    m_currentRowRange;        //!< The currently selected row range, used to add rows
            std::vector<TableRowMetric::Ptr> m_headRowMetrics;         //!< Specifications of the table head rows
            std::vector<TableRowMetric::Ptr> m_bodyRowMetrics;         //!< Specifications of the table body rows
            std::vector<TableRowMetric::Ptr> m_footRowMetrics;         //!< Specifications of the table foot rows
            FloatRect                        m_cachedLayoutArea;       //!< Caches the layout area before table columns are calculated
            Outline                          m_tableToCellSpacing;     //!< The table to cell spacing (margin + border + default spacing)
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The list-data supports the individual specifications of a list
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API ListData
        {
        public:
            typedef std::shared_ptr<ListData> Ptr; //!< Shared list data pointer

            bool               Ordered;          //!< Determine whether the list is ordered or unordered
            unsigned int       ActualItemIndex;  //!< The 1-based index of the actual list item
            MarkupListItemType ItemType;         //!< The list item type (defining the bullet or enumeration of a list item)

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Calculates the bullet of a list item
            ///
            /// @param nestingDepth     The nesting depth of the current list
            ///
            /// @return The bullet of a list item
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            String calculateBullet(size_t nestingDepth)
            {
                String bullet = U"";

                if (!Ordered)
                {
                    if     (nestingDepth % 3 == 1)
                    {
                        if ((ItemType == MarkupListItemType::InheritOrDefault) || (ItemType == MarkupListItemType::Disc))
                            bullet.push_back(U'\u25CF'); // ●
                        else if (ItemType == MarkupListItemType::Circle)
                            bullet.push_back(U'\u25CB'); // ○
                        else if (ItemType == MarkupListItemType::Square)
                            bullet.push_back(U'\u25A0'); // ■
                    }
                    else if (nestingDepth % 3 == 2)
                    {
                        if (ItemType == MarkupListItemType::InheritOrDefault || ItemType == MarkupListItemType::Circle)
                            bullet.push_back(U'\u25CB'); // ○
                        else if (ItemType == MarkupListItemType::Disc)
                            bullet.push_back(U'\u25CF'); // ●
                        else if (ItemType == MarkupListItemType::Square)
                            bullet.push_back(U'\u25A0'); // ■
                    }
                    else
                    {
                        if (ItemType == MarkupListItemType::InheritOrDefault || ItemType == MarkupListItemType::Square)
                            bullet.push_back(U'\u25A0'); // ■
                        else if (ItemType == MarkupListItemType::Disc)
                            bullet.push_back(U'\u25CF'); // ●
                        else if (ItemType == MarkupListItemType::Circle)
                            bullet.push_back(U'\u25CB'); // ○
                    }
                }
                else
                {
                    if (nestingDepth % 2 == 1)
                    {
                        if(ItemType == MarkupListItemType::InheritOrDefault || ItemType == MarkupListItemType::Arabic)
                            bullet = std::to_wstring(ActualItemIndex);
                        else if(ItemType == MarkupListItemType::UpperAlpha)
                            bullet = static_cast<char32_t>('@' + (wchar_t)ActualItemIndex);
                        else if(ItemType == MarkupListItemType::LowerAlpha)
                            bullet = static_cast<char32_t>('`' + (wchar_t)ActualItemIndex);
                    }
                    else
                    {
                        if(ItemType == MarkupListItemType::InheritOrDefault || ItemType == MarkupListItemType::LowerAlpha)
                            bullet = static_cast<char32_t>('`' + (wchar_t)ActualItemIndex);
                        else if(ItemType == MarkupListItemType::UpperAlpha)
                            bullet = static_cast<char32_t>('@' + (wchar_t)ActualItemIndex);
                        else if(ItemType == MarkupListItemType::Arabic)
                            bullet = std::to_wstring(ActualItemIndex);
                    }
                    bullet += U".";
                }

                return bullet;
            }
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief The formatting-state holds the current formatting
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API FormattingState
        {
        public:
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief The default constructor
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FormattingState(const Color& foreColor)
                : ForeColor(foreColor), BackColor(Color(255, 255, 255)), Opacity(1), TextHeight(14.0f),
                  TextFont(nullptr), Style(TextStyle::Regular), Superscript(false), Subscript(false),
                  TableMetrics(), ListMetrics(), BorderWidth(SizeType::Pixel, 0.0f), BlockPadding(SizeType::Pixel, 0.0f)
            { ; }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief The copy constructor
            ///
            /// @param formattingState  The new formatting state
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FormattingState(const FormattingState& formattingState)
                : ForeColor(formattingState.ForeColor), BackColor(formattingState.BackColor), Opacity(formattingState.Opacity),
                  TextHeight(formattingState.TextHeight), TextFont(formattingState.TextFont),
                  Style(formattingState.Style), Superscript(formattingState.Superscript), Subscript(formattingState.Subscript),
                  TableMetrics(formattingState.TableMetrics), ListMetrics(formattingState.ListMetrics),
                  BorderWidth(formattingState.BorderWidth), BlockPadding(formattingState.BlockPadding)
            { ; }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief The assignment operator
            ///
            /// @param formattingState  The new formatting state
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            FormattingState& operator =(FormattingState& formattingState)
            {
                ForeColor       = formattingState.ForeColor;
                BackColor       = formattingState.BackColor;
                Opacity         = formattingState.Opacity;
                TextHeight        = formattingState.TextHeight;
                TextFont        = formattingState.TextFont;
                Style           = formattingState.Style;
                Superscript     = formattingState.Superscript;
                Subscript       = formattingState.Subscript;
                TableMetrics    = formattingState.TableMetrics;
                ListMetrics     = formattingState.ListMetrics;
                BorderWidth     = formattingState.BorderWidth;
                BlockPadding    = formattingState.BlockPadding;

                return *this;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current text size to the requested text size
            ///
            /// @param textSize  The text size to set
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setTextSize(OneDimSize textSize)
            {
                if (textSize.sizeType == SizeType::Pixel)
                    TextHeight = textSize.value;
                else if (textSize.sizeType == SizeType::EquivalentOfM)
                    TextHeight = textSize.value * 16;
                else if (textSize.sizeType == SizeType::Point)
                    TextHeight = textSize.value * 1.3333333333f;
                else // (textSize.sizeType == SizeType::Relative)
                    TextHeight = TextHeight * textSize.value;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching font of the sans font family
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the sans regular font weight
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToSansFamily(const FormattedDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == fontCollection.Sans->Regular    || TextFont == fontCollection.Serif->Regular    || TextFont == fontCollection.Mono->Regular)
                   TextFont = fontCollection.Sans->Regular;
                else if (TextFont == fontCollection.Sans->Italic     || TextFont == fontCollection.Serif->Italic     || TextFont == fontCollection.Mono->Italic)
                    TextFont = fontCollection.Sans->Italic;
                else if (TextFont == fontCollection.Sans->BoldItalic || TextFont == fontCollection.Serif->BoldItalic || TextFont == fontCollection.Mono->BoldItalic)
                    TextFont = fontCollection.Sans->BoldItalic;
                else if (forceOnNoMatch)
                    TextFont = fontCollection.Sans->Regular;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching font of the serif font family
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the serif regular font weight
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToSerifFamily(const FormattedDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == fontCollection.Sans->Regular    || TextFont == fontCollection.Serif->Regular    || TextFont == fontCollection.Mono->Regular)
                    TextFont = fontCollection.Serif->Regular;
                else if (TextFont == fontCollection.Sans->Italic     || TextFont == fontCollection.Serif->Italic     || TextFont == fontCollection.Mono->Italic)
                    TextFont = fontCollection.Serif->Italic;
                else if (TextFont == fontCollection.Sans->BoldItalic || TextFont == fontCollection.Serif->BoldItalic || TextFont == fontCollection.Mono->BoldItalic)
                    TextFont = fontCollection.Serif->BoldItalic;
                else if (forceOnNoMatch)
                    TextFont = fontCollection.Serif->Regular;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching font of the mono font family
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the mono regular font weight
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToMonoFamily(const FormattedDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == fontCollection.Sans->Regular    || TextFont == fontCollection.Serif->Regular    || TextFont == fontCollection.Mono->Regular)
                    TextFont = fontCollection.Mono->Regular;
                else if (TextFont == fontCollection.Sans->Italic     || TextFont == fontCollection.Serif->Italic     || TextFont == fontCollection.Mono->Italic)
                    TextFont = fontCollection.Mono->Italic;
                else if (TextFont == fontCollection.Sans->BoldItalic || TextFont == fontCollection.Serif->BoldItalic || TextFont == fontCollection.Mono->BoldItalic)
                    TextFont = fontCollection.Mono->BoldItalic;
                else if (forceOnNoMatch)
                    TextFont = fontCollection.Mono->Regular;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching font family
            ///
            /// @param fontFamily      The name of the requested font family
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font family is forced to the sans font family
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontFamily(const String& fontFamily, const FormattedDocument::FontCollection& fontCollection,
                                      bool forceOnNoMatch = false)
            {
                if      (DefaultFontFamilies::isSans(fontFamily))  setFontToSansFamily(fontCollection);
                else if (DefaultFontFamilies::isSerif(fontFamily)) setFontToSerifFamily(fontCollection);
                else if (DefaultFontFamilies::isMono(fontFamily))  setFontToMonoFamily(fontCollection);
                else if (forceOnNoMatch)                           setFontToSansFamily(fontCollection);
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching regular font weight (if not already regular font weight)
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the sans regular font weight
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToRegularWeight(const FormattedDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == fontCollection.Sans->Regular  || TextFont == fontCollection.Sans->Bold)
                    TextFont = fontCollection.Sans->Regular;
                else if (TextFont == fontCollection.Sans->Italic   || TextFont == fontCollection.Sans->BoldItalic)
                    TextFont = fontCollection.Sans->Italic;
                else if (TextFont == fontCollection.Serif->Regular || TextFont == fontCollection.Serif->Bold)
                    TextFont = fontCollection.Serif->Regular;
                else if (TextFont == fontCollection.Serif->Italic  || TextFont == fontCollection.Serif->BoldItalic)
                    TextFont = fontCollection.Serif->Italic;
                else if (TextFont == fontCollection.Mono->Regular  || TextFont == fontCollection.Mono->Bold)
                    TextFont = fontCollection.Mono->Regular;
                else if (TextFont == fontCollection.Mono->Italic   || TextFont == fontCollection.Mono->BoldItalic)
                    TextFont = fontCollection.Mono->Italic;
                else if (forceOnNoMatch)
                    TextFont = fontCollection.Sans->Regular;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching bold font (if not already bold)
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the sans bold font
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToBold(const FormattedDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == fontCollection.Sans->Regular  || TextFont == fontCollection.Sans->Bold)
                    TextFont = fontCollection.Sans->Bold;
                else if (TextFont == fontCollection.Sans->Italic   || TextFont == fontCollection.Sans->BoldItalic)
                    TextFont = fontCollection.Sans->BoldItalic;
                else if (TextFont == fontCollection.Serif->Regular || TextFont == fontCollection.Serif->Bold)
                    TextFont = fontCollection.Serif->Bold;
                else if (TextFont == fontCollection.Serif->Italic  || TextFont == fontCollection.Serif->BoldItalic)
                    TextFont = fontCollection.Serif->BoldItalic;
                else if (TextFont == fontCollection.Mono->Regular  || TextFont == fontCollection.Mono->Bold)
                    TextFont = fontCollection.Mono->Bold;
                else if (TextFont == fontCollection.Mono->Italic   || TextFont == fontCollection.Mono->BoldItalic)
                    TextFont = fontCollection.Mono->BoldItalic;
                else if (forceOnNoMatch)
                    TextFont = fontCollection.Sans->Bold;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font weight to the requested font weight
            ///
            /// @param bold            The requested font weight (bold or regular)
            /// @param fontCollection  The collection of available fonts
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontWeight(bool bold, const FormattedDocument::FontCollection& fontCollection)
            {   if (bold) setFontToBold(fontCollection); else setFontToRegularWeight(fontCollection);   }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching regular font slant (if not already regular font slant)
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the sans regular font
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToRegularSlant(const FormattedDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == fontCollection.Sans->Regular  || TextFont == fontCollection.Sans->Italic)
                    TextFont = fontCollection.Sans->Regular;
                else if (TextFont == fontCollection.Sans->Bold     || TextFont == fontCollection.Sans->BoldItalic)
                    TextFont = fontCollection.Sans->Bold;
                else if (TextFont == fontCollection.Serif->Regular || TextFont == fontCollection.Serif->Italic)
                    TextFont = fontCollection.Serif->Regular;
                else if (TextFont == fontCollection.Serif->Bold    || TextFont == fontCollection.Serif->BoldItalic)
                    TextFont = fontCollection.Serif->Bold;
                else if (TextFont == fontCollection.Mono->Regular  || TextFont == fontCollection.Mono->Italic)
                    TextFont = fontCollection.Mono->Regular;
                else if (TextFont == fontCollection.Mono->Bold     || TextFont == fontCollection.Mono->BoldItalic)
                    TextFont = fontCollection.Mono->Bold;
                else if (forceOnNoMatch)
                    TextFont = fontCollection.Sans->Regular;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font to the best matching italic font (if not already italic)
            ///
            /// @param fontCollection  The collection of available fonts
            /// @param forceOnNoMatch  The flag to determine whether an unrecognized font is forced to the sans italic font
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontToItalic(const FormattedDocument::FontCollection& fontCollection, bool forceOnNoMatch = false)
            {
                if      (TextFont == fontCollection.Sans->Regular  || TextFont == fontCollection.Sans->Italic)
                    TextFont = fontCollection.Sans->Italic;
                else if (TextFont == fontCollection.Sans->Bold     || TextFont == fontCollection.Sans->BoldItalic)
                    TextFont = fontCollection.Sans->BoldItalic;
                else if (TextFont == fontCollection.Serif->Regular || TextFont == fontCollection.Serif->Italic)
                    TextFont = fontCollection.Serif->Italic;
                else if (TextFont == fontCollection.Serif->Bold    || TextFont == fontCollection.Serif->BoldItalic)
                    TextFont = fontCollection.Serif->BoldItalic;
                else if (TextFont == fontCollection.Mono->Regular  || TextFont == fontCollection.Mono->Italic)
                    TextFont = fontCollection.Mono->Italic;
                else if (TextFont == fontCollection.Mono->Bold     || TextFont == fontCollection.Mono->BoldItalic)
                    TextFont = fontCollection.Mono->BoldItalic;
                else if (forceOnNoMatch)
                    TextFont = fontCollection.Sans->Italic;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Set the current font slant to the requested font slant
            ///
            /// @param italic          The requested font slant (italic or regular)
            /// @param fontCollection  The collection of available fonts
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            inline void setFontSlant(bool italic, const FormattedDocument::FontCollection& fontCollection)
            {   if (italic) setFontToItalic(fontCollection); else setFontToRegularSlant(fontCollection);   }

        public:
            Color                     ForeColor;    //!< The rurrent fore color
            Color                     BackColor;    //!< The rurrent back color
            float                     Opacity;      //!< The rurrent opacity
            float                     TextHeight;   //!< The rurrent text height
            Font                      TextFont;     //!< The rurrent font
            TextStyle                 Style;        //!< The rurrent text style
            float                     Superscript;  //!< Superscript offset to normal text
            float                     Subscript;    //!< Subscript offset to normal text
            std::list<TableMetric::Ptr> TableMetrics;  //!< Specifications of the (nested) tables
            std::list<ListData::Ptr>  ListMetrics;  //!< Specifications of the (nested) lists
            FourDimSize               BorderWidth;  //!< Specifications of the border
            FourDimSize               BlockPadding; //!< Specifications of the padding
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Virtual destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~FormattedDocument() = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gets the formatted content of this document
        ///
        /// @return The formatted content of this document
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual const std::vector<std::shared_ptr<FormattedElement>>& getContent() const = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads an XHTML document into an XHTML DOM and replaces the previous XHTML DOM root
        ///
        /// This method combines
        /// @see readXhtmlDocument
        /// @see parseXhtmlDocument
        /// in a most common way. To get a better control, the methods can also be called separately.
        ///
        /// @param filePath  The XHTML file to load into this document's XHTML DOM
        /// @param trace     Determine whether to trace the result (element tree) and errors/warnings to std::cout.
        ///
        /// @return The state with the value 0 on succes, value -1 if file can not be red, value -2 if file can not be parsed or
        ///         is empty or value -3 if no XHTML document root tag (<html ...></html>) is available.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual int loadDocument(const std::string filePath, bool trace = false) = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Rearrange the complete visible content
        ///
        /// @param clientSize       The available size of the area to render to
        /// @param defaultTextSize  The default text size
        /// @param defaultForeColor The default foreground/text color
        /// @param defaultOpacity   The default foreground/text and background opacity
        /// @param fontCollection   The collection of supported fonts
        /// @param keepSelection    Determine whether to keep the selection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void layout(Vector2f clientSize, float defaultTextSize, Color defaultForeColor, float defaultOpacity,
                            const FormattedDocument::FontCollection& fontCollection, bool keepSelection) = 0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the layout size
        ///
        /// @return The layout size
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual inline Vector2f getOccupiedLayoutSize() const = 0;
    };
} }

#endif // TGUI_FORMATTED_DOCUMENT_HPP
