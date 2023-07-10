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

#include "TGUI/String.hpp"
#include "TGUI/Xhtml/StringHelper.hpp"
#include "TGUI/Xhtml/Widgets/FormattedDocument.hpp"

namespace tgui  { namespace xhtml
{

    void FormattedDocument::TableMetric::updateRequestedColumnSize(size_t index, OneDimSize updateSize)
    {
        if (index == SIZE_MAX)
            return;

        // SizeType::ViewportWidth with 0.0f acts as a default.
        for (size_t idx = m_columnRequestedSizes.size(); idx <= index; idx++)
            m_columnRequestedSizes.push_back(OneDimSize(SizeType::ViewportWidth, 0.0f));

        if (updateSize.sizeType == SizeType::ViewportWidth)
            return;

        if (m_columnRequestedSizes[index].sizeType == SizeType::ViewportWidth && updateSize.sizeType != SizeType::ViewportWidth)
        {
            m_columnRequestedSizes[index] = updateSize;
            return;
        }
        if (m_columnRequestedSizes[index].sizeType == SizeType::Relative && updateSize.sizeType == SizeType::Relative)
        {
            m_columnRequestedSizes[index].value = std::max(m_columnRequestedSizes[index].value, updateSize.value);
            return;
        }

        if (updateSize.sizeType == SizeType::Relative)
            return;

        auto currentSize = m_columnRequestedSizes[index].toPixel(100);
        updateSize = updateSize.toPixel(100);
        updateSize.value = std::max(currentSize.value, updateSize.value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedDocument::TableMetric::updatePreferredColumnSize(size_t columnIndex, SizePreference updateSize)
    {
        if (columnIndex == SIZE_MAX)
            return;

        for (size_t idx = m_columnPreferredSizes.size(); idx <= columnIndex; idx++)
            m_columnPreferredSizes.push_back({ false, 0.1f });

        m_columnPreferredSizes[columnIndex] = updateSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float FormattedDocument::TableMetric::getTotalPreferredColumnSizes() const
    {
        float result = 0;
        for (size_t index = 0; index < m_columnPreferredSizes.size(); index++)
            result += m_columnPreferredSizes[index].value;

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    size_t FormattedDocument::TableMetric::getRowRangeRowMetricCount(FormattedDocument::TableRowRange rowRange) const
    {
        if (rowRange == TableRowRange::Head)
            return m_headRowMetrics.size();
        else if (rowRange == TableRowRange::Foot)
            return m_footRowMetrics.size();
        else
            return m_bodyRowMetrics.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void FormattedDocument::TableMetric::addRowRangeRowMetric(FormattedDocument::TableRowRange rowRange, FormattedDocument::TableRowMetric::Ptr tableRowMetric)
    {
        if (rowRange == TableRowRange::Head)
            m_headRowMetrics.push_back(tableRowMetric);
        else if (rowRange == TableRowRange::Foot)
            m_footRowMetrics.push_back(tableRowMetric);
        else
            m_bodyRowMetrics.push_back(tableRowMetric);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FormattedDocument::TableRowMetric::Ptr FormattedDocument::TableMetric::getRowRangeRowMetric(FormattedDocument::TableRowRange rowRange, size_t index) const
    {
        if (rowRange == TableRowRange::Head)
            return index < m_headRowMetrics.size() ? m_headRowMetrics[index] : nullptr;
        else if (rowRange == TableRowRange::Foot)
            return index < m_footRowMetrics.size() ? m_footRowMetrics[index] : nullptr;
        else
            return index < m_bodyRowMetrics.size() ? m_bodyRowMetrics[index] : nullptr;
        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FormattedDocument::TableRowMetric::Ptr FormattedDocument::TableMetric::getRowRangeTailRowMetric(FormattedDocument::TableRowRange rowRange) const
    {
        if (rowRange == TableRowRange::Head)
            return !m_headRowMetrics.empty() ? m_headRowMetrics.back() : nullptr;
        else if (rowRange == TableRowRange::Foot)
            return !m_footRowMetrics.empty() ? m_footRowMetrics.back() : nullptr;
        else
            return !m_bodyRowMetrics.empty() ? m_bodyRowMetrics.back() : nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FormattedDocument::TableRowMetric::Ptr FormattedDocument::TableMetric::getTailRowMetric() const
    {
        auto rowRange = getRowRangeTailRowMetric(TableRowRange::Foot);
        if (rowRange)
            return rowRange;
        rowRange = getRowRangeTailRowMetric(TableRowRange::Body);
        if (rowRange)
            return rowRange;
        return getRowRangeTailRowMetric(TableRowRange::Head);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FormattedDocument::TableRowMetric::Ptr FormattedDocument::TableMetric::firstOrDefaultRowMetricByContentOrigin(MarkupLanguageElement::Ptr origin, size_t& rowIndex) const
    {
        rowIndex = 0;
        for (size_t index = 0; index < m_headRowMetrics.size(); index++)
        {
            auto rowMetric = m_headRowMetrics[index];
            if (rowMetric && rowMetric->getContentOrigin() == origin)
                return rowMetric;
            rowIndex++;
        }
        for (size_t index = 0; index < m_bodyRowMetrics.size(); index++)
        {
            auto rowMetric = m_bodyRowMetrics[index];
            if (rowMetric && rowMetric->getContentOrigin() == origin)
                return rowMetric;
            rowIndex++;
        }
        for (size_t index = 0; index < m_footRowMetrics.size(); index++)
        {
            auto rowMetric = m_footRowMetrics[index];
            if (rowMetric && rowMetric->getContentOrigin() == origin)
                return rowMetric;
            rowIndex++;
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FormattedDocument::TableRowMetric::Ptr FormattedDocument::TableMetric::firstOrDefaultRowMetricByAnyCellContentOrigin(MarkupLanguageElement::Ptr origin, size_t& rowIndex) const
    {
        rowIndex = 0;
        for (size_t index = 0; index < m_headRowMetrics.size(); index++)
        {
            auto rowMetric = m_headRowMetrics[index];
            for (size_t cellIndex = 0; rowMetric && cellIndex < rowMetric->getCellMetricCount(); cellIndex++)
            {
                auto cellMetric = rowMetric->getCellMetric(cellIndex);
                if (cellMetric && cellMetric->getContentOrigin() == origin)
                    return rowMetric;
            }
            rowIndex++;
        }
        for (size_t index = 0; index < m_bodyRowMetrics.size(); index++)
        {
            auto rowMetric = m_bodyRowMetrics[index];
            for (size_t cellIndex = 0; rowMetric && cellIndex < rowMetric->getCellMetricCount(); cellIndex++)
            {
                auto cellMetric = rowMetric->getCellMetric(cellIndex);
                if (cellMetric && cellMetric->getContentOrigin() == origin)
                    return rowMetric;
            }
            rowIndex++;
        }
        for (size_t index = 0; index < m_footRowMetrics.size(); index++)
        {
            auto rowMetric = m_footRowMetrics[index];
            for (size_t cellIndex = 0; rowMetric && cellIndex < rowMetric->getCellMetricCount(); cellIndex++)
            {
                auto cellMetric = rowMetric->getCellMetric(cellIndex);
                if (cellMetric && cellMetric->getContentOrigin() == origin)
                    return rowMetric;
            }
            rowIndex++;
        }

        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Outline FormattedDocument::TableMetric::getTableMargin() const
    {
        auto formattedRect = std::dynamic_pointer_cast<FormattedRectangle>(m_formattedElement);
        if (formattedRect)
        {
            auto margin = formattedRect->getMargin();

            // Spacing between table and cells is always 2px.
            Outline result(margin.left, margin.top, margin.right, margin.bottom);

            return result;
        }
        else
            return Outline(0.0f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Outline FormattedDocument::TableMetric::calculateTableRenderAreaToCellSpacing() const
    {
        auto formattedRect = std::dynamic_pointer_cast<FormattedRectangle>(m_formattedElement);
        if (formattedRect)
        {
            auto borderWidth = formattedRect->getBoderWidth();

            // Spacing between table and cells is always 2px.
            Outline result(2.0f + borderWidth.left, 2.0f + borderWidth.top, 2.0f + borderWidth.right, 2.0f + borderWidth.bottom);
            return result;
        }
        else
            return Outline(0.0f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Outline FormattedDocument::TableMetric::calculateTableLayoutAreaToCellSpacing() const
    {
        auto formattedRect = std::dynamic_pointer_cast<FormattedRectangle>(m_formattedElement);
        if (formattedRect)
        {
            auto borderWidth = formattedRect->getBoderWidth();
            auto margin = formattedRect->getMargin();

            // Spacing between table and cells is always 2px.
            Outline result(2.0f + borderWidth.left + margin.left,
                2.0f + borderWidth.top + margin.top,
                2.0f + borderWidth.right + margin.right,
                2.0f + borderWidth.bottom + margin.bottom);

            return result;
        }
        else
            return Outline(0.0f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FloatRect FormattedDocument::TableMetric::calculateCellLayoutArea(size_t columnIndex) const
    {
        if (columnIndex >= m_columnPreferredSizes.size())
            return FloatRect(m_cachedLayoutArea.left, 0, m_cachedLayoutArea.width, 0);

        float left = columnIndex * 2.0f;
        for (size_t index = 0; index < columnIndex; index++)
            left += m_columnPreferredSizes[index].value;

        return FloatRect(left, 0, m_columnPreferredSizes[columnIndex].value, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<FormattedDocument::FontCollection> FormattedDocument::FontCollection::platformOptimizedFontCollection()
    {
        auto fontCollection = std::make_shared<FormattedDocument::FontCollection>();

        fontCollection->Sans = std::make_shared<FormattedDocument::FontPack>();
        fontCollection->Serif = std::make_shared<FormattedDocument::FontPack>();
        fontCollection->Mono = std::make_shared<FormattedDocument::FontPack>();

        // ------------------------------------------------------------------------------------------------
        // We can achieve a higher display quality for bold text if - instead of setting m_textStyleCached
        // to TextStyle::Bold - we use our own font. Therefore, the Bold and BoldItalic fonts are provided.
        // ------------------------------------------------------------------------------------------------

#if defined(TGUI_SYSTEM_WINDOWS)
        tgui::String fontRoot = L"C:\\Windows\\Fonts\\";

        fontCollection->Sans->Regular = tgui::Font(fontRoot + L"arial.ttf");
        fontCollection->Sans->Bold = tgui::Font(fontRoot + L"arialbd.ttf");
        fontCollection->Sans->Italic = tgui::Font(fontRoot + L"ariali.ttf");
        fontCollection->Sans->BoldItalic = tgui::Font(fontRoot + L"arialbi.ttf");

        fontCollection->Serif->Regular = tgui::Font(fontRoot + L"times.ttf");
        fontCollection->Serif->Bold = tgui::Font(fontRoot + L"timesbd.ttf");
        fontCollection->Serif->Italic = tgui::Font(fontRoot + L"timesi.ttf");
        fontCollection->Serif->BoldItalic = tgui::Font(fontRoot + L"timesbi.ttf");

        fontCollection->Mono->Regular = tgui::Font(fontRoot + L"consola.ttf"); // L"cour.ttf");
        fontCollection->Mono->Bold = tgui::Font(fontRoot + L"consolab.ttf"); // L"courbd.ttf");
        fontCollection->Mono->Italic = tgui::Font(fontRoot + L"consolai.ttf"); // L"couri.ttf");
        fontCollection->Mono->BoldItalic = tgui::Font(fontRoot + L"consolaz.ttf"); // L"courbi.ttf");
#elif defined(TGUI_SYSTEM_LINUX)
        tgui::String fontRoot = L"/usr/share/fonts/";

        // Alternative sans fonts (Linux Firefox uses Roboto by default): OpenSans, Roboto, DejaVuSans
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        std::filesystem::path suseSansPath{ "/usr/share/fonts/truetype/Roboto-Regular.ttf" };
        std::filesystem::path manjaroSansPath{ "/usr/share/fonts/TTF/DejaVuSans.ttf" };
#else
        std::experimental::filesystem::path suseSansPath{ "/usr/share/fonts/truetype/Roboto-Regular.ttf" };
        std::experimental::filesystem::path manjaroSansPath{ "/usr/share/fonts/TTF/DejaVuSans.ttf" };
#endif
        // openSUSE
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        if (std::filesystem::exists(suseSansPath))
#else
        if (std::experimental::filesystem::exists(suseSansPath))
#endif
        {
            fontCollection->Sans->Regular = tgui::Font(fontRoot + L"truetype/Roboto-Regular.ttf");
            fontCollection->Sans->Bold = tgui::Font(fontRoot + L"truetype/Roboto-Bold.ttf");
            fontCollection->Sans->Italic = tgui::Font(fontRoot + L"truetype/Roboto-Italic.ttf");
            fontCollection->Sans->BoldItalic = tgui::Font(fontRoot + L"truetype/Roboto-BoldItalic.ttf");

            fontCollection->Serif->Regular = tgui::Font(fontRoot + L"truetype/DejaVuSerif.ttf");
            fontCollection->Serif->Bold = tgui::Font(fontRoot + L"truetype/DejaVuSerif-Bold.ttf");
            fontCollection->Serif->Italic = tgui::Font(fontRoot + L"truetype/DejaVuSerif-Italic.ttf");
            fontCollection->Serif->BoldItalic = tgui::Font(fontRoot + L"truetype/DejaVuSerif-BoldItalic.ttf");

            fontCollection->Mono->Regular = tgui::Font(fontRoot + L"truetype/DejaVuSansMono.ttf");
            fontCollection->Mono->Bold = tgui::Font(fontRoot + L"truetype/DejaVuSansMono-Bold.ttf");
            fontCollection->Mono->Italic = tgui::Font(fontRoot + L"truetype/DejaVuSansMono-Oblique.ttf");
            fontCollection->Mono->BoldItalic = tgui::Font(fontRoot + L"truetype/DejaVuSansMono-BoldOblique.ttf");
        }
        // Manjaro
#if TGUI_COMPILED_WITH_CPP_VER >= 17
        else if (std::filesystem::exists(manjaroSansPath))
#else
        if (std::experimental::filesystem::exists(manjaroSansPath))
#endif
        {
            fontCollection->Sans->Regular = tgui::Font(fontRoot + L"TTF/DejaVuSans.ttf");
            fontCollection->Sans->Bold = tgui::Font(fontRoot + L"TTF/DejaVuSans-Bold.ttf");
            fontCollection->Sans->Italic = tgui::Font(fontRoot + L"TTF/DejaVuSans-Oblique.ttf");
            fontCollection->Sans->BoldItalic = tgui::Font(fontRoot + L"TTF/DejaVuSans-BoldOblique.ttf");

            fontCollection->Serif->Regular = tgui::Font(fontRoot + L"TTF/DejaVuSerif.ttf");
            fontCollection->Serif->Bold = tgui::Font(fontRoot + L"TTF/DejaVuSerif-Bold.ttf");
            fontCollection->Serif->Italic = tgui::Font(fontRoot + L"TTF/DejaVuSerif-Italic.ttf");
            fontCollection->Serif->BoldItalic = tgui::Font(fontRoot + L"TTF/DejaVuSerif-BoldItalic.ttf");

            fontCollection->Mono->Regular = tgui::Font(fontRoot + L"TTF/DejaVuSansMono.ttf");
            fontCollection->Mono->Bold = tgui::Font(fontRoot + L"TTF/DejaVuSansMono-Bold.ttf");
            fontCollection->Mono->Italic = tgui::Font(fontRoot + L"TTF/DejaVuSansMono-Oblique.ttf");
            fontCollection->Mono->BoldItalic = tgui::Font(fontRoot + L"TTF/DejaVuSansMono-BoldOblique.ttf");
        }
        else
        {
            std::cerr << "ERROR: The fonts can not be loaded.\n";
        }
#elif defined(TGUI_SYSTEM_IOS)
        throw "IOS fonts currently not supported.";
#elif defined(TGUI_SYSTEM_MACOS)
        throw "MACOS fonts currently not supported.";
#elif defined(TGUI_SYSTEM_ANDROID)
        throw "ANDROID fonts currently not supported.";
#else
        throw "Unsupported platform.";
#endif

        fontCollection->assertValid();
        fontCollection->Sans->Regular.setSmooth(false);
        fontCollection->Sans->Bold.setSmooth(false);
        fontCollection->Sans->Italic.setSmooth(false);
        fontCollection->Sans->BoldItalic.setSmooth(false);

        return fontCollection;
    }
} }
