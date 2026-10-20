// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "PointsTableModel.h"
#include "pointdata_export.h"

#include "storage/interface/PointDataAccess.h"

#include <Dataset.h>

#include <actions/StringAction.h>
#include <actions/HorizontalGroupAction.h>
#include <actions/IntegralAction.h>
#include <actions/ToggleAction.h>
#include <actions/TriggerAction.h>
#include <actions/VerticalGroupAction.h>

#include <QWidget>

#include <cstdint>
#include <functional>
#include <vector>

class QEvent;
class QLabel;
class QShowEvent;
class QSpinBox;
class QTableView;
class QTimer;

class Points;

/**
 * @brief Interactive table view for inspecting point data and tiled access.
 *
 * The widget is a diagnostic consumer of the storage-neutral PointData access
 * API. It fetches only the visible row and column window after table
 * navigation and can inspect both eager and tiled datasets. This makes the
 * plugin useful as a basic point-data browser and as a manual test surface
 * for lazy loading behavior.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT PointsViewWidget final : public QWidget
{
    Q_OBJECT

public:
    /**
     * Captures the user-facing table settings that can be transferred to
     * another point-data view.
     */
    struct ViewState
    {
        QString rowFilter;
        QString columnFilter;
        bool live = true;
    };

    /**
     * Constructs the points view.
     *
     * When \p showDatasetControls is false, the widget displays only the
     * supplied dataset and leaves dataset selection to its embedding action
     * or plugin.
     */
    explicit PointsViewWidget(QWidget* parent = nullptr, bool showDatasetControls = true);

    /** Sets the point-data source displayed by the widget and clears old query results. */
    void setPoints(const mv::Dataset<Points>& points);

    /** Returns the current filters and live-table setting. */
    ViewState getViewState() const;

    /** Applies filters and live-table state to the widget. */
    void setViewState(const ViewState& state);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void showEvent(QShowEvent* event) override;

private slots:
    void scheduleVisibleRangeFetch();
    void fetchVisibleRange();
    void applyFilters();
    void showLiveTable(bool live);
    void getRow();
    void getRowRange();
    void getRows();
    void getColumn();
    void getColumnRange();
    void getColumns();
    void getColumnsForRows();
    void getRange();

private:
    void refreshPoints();
    void setStatus(const QString& text);
    void updateUiState();
    void updateFacadeActionState();
    static std::vector<std::uint64_t> parseFilter(const QString& filter, std::uint64_t maximum, QString& error, const std::vector<QString>& dimensionNames = {});
    void displayQuery(const std::vector<std::uint64_t>& rowIds, const std::vector<std::uint64_t>& columnIds, const std::function<void(const PointDataAccessVisitor&)>& request);

    mv::Dataset<Points>             _points;                                 /**< Dataset currently displayed by the table. */
    bool                            _showDatasetControls;                    /**< Whether standalone dataset controls are shown. */
    QLabel*                         _datasetLabel;                           /**< Optional dataset summary shown above the table. */
    QTableView*                     _table;                                  /**< Table view displaying the fetched window. */
    PointsTableModel*               _model;                                  /**< Model storing logical dimensions and fetched values. */
    QTimer*                         _fetchTimer;                             /**< Coalesces visible-range requests. */
    QTimer*                         _filterTimer;                            /**< Debounces filter-driven requests. */
    mv::gui::HorizontalGroupAction  _filterGroupAction;                      /**< Row, column, and live-table controls. */
    mv::gui::VerticalGroupAction    _facadeGroupAction;                      /**< Point-data query actions. */
    mv::gui::HorizontalGroupAction  _getRowGroupAction;                      /**< Controls a single-row query. */
    mv::gui::HorizontalGroupAction  _getRowRangeGroupAction;                 /**< Controls a contiguous row-range query. */
    mv::gui::HorizontalGroupAction  _getRowsGroupAction;                     /**< Controls a selected-row query. */
    mv::gui::HorizontalGroupAction  _getColumnGroupAction;                   /**< Controls a single-column query. */
    mv::gui::HorizontalGroupAction  _getColumnRangeGroupAction;              /**< Controls a contiguous column-range query. */
    mv::gui::HorizontalGroupAction  _getColumnsGroupAction;                  /**< Controls a selected-column query. */
    mv::gui::HorizontalGroupAction  _getColumnsForRowsGroupAction;           /**< Controls selected columns for selected rows. */
    mv::gui::HorizontalGroupAction  _getRangeGroupAction;                    /**< Controls a rectangular range query. */
    mv::gui::IntegralAction         _rowIdAction;                            /**< Row ID for a single-row query. */
    mv::gui::IntegralAction         _columnIdAction;                         /**< Column ID for a single-column query. */
    mv::gui::IntegralAction         _rowStartAction;                         /**< First row of a row-range query. */
    mv::gui::IntegralAction         _rowEndAction;                           /**< Last row of a row-range query. */
    mv::gui::IntegralAction         _columnStartAction;                      /**< First column of a column-range query. */
    mv::gui::IntegralAction         _columnEndAction;                        /**< Last column of a column-range query. */
    mv::gui::IntegralAction         _rangeRowStartAction;                    /**< First row of a rectangular range query. */
    mv::gui::IntegralAction         _rangeRowEndAction;                      /**< Last row of a rectangular range query. */
    mv::gui::IntegralAction         _rangeColumnStartAction;                 /**< First column of a rectangular range query. */
    mv::gui::IntegralAction         _rangeColumnEndAction;                   /**< Last column of a rectangular range query. */
    mv::gui::IntegralAction         _columnsForRowsStartAction;              /**< First selected column for a row-list query. */
    mv::gui::IntegralAction         _columnsForRowsEndAction;                /**< Last selected column for a row-list query. */
    mv::gui::StringAction           _rowIdsAction;                           /**< Comma- and range-separated row IDs. */
    mv::gui::StringAction           _columnIdsAction;                        /**< Comma- and range-separated column IDs. */
    mv::gui::StringAction           _columnRowIdsAction;                     /**< Row IDs used by the selected-column query. */
    mv::gui::TriggerAction          _getRowAction;                           /**< Executes the single-row query. */
    mv::gui::TriggerAction          _getRowRangeAction;                      /**< Executes the row-range query. */
    mv::gui::TriggerAction          _getRowsAction;                          /**< Executes the row-list query. */
    mv::gui::TriggerAction          _getColumnAction;                        /**< Executes the single-column query. */
    mv::gui::TriggerAction          _getColumnRangeAction;                   /**< Executes the column-range query. */
    mv::gui::TriggerAction          _getColumnsAction;                       /**< Executes the column-list query. */
    mv::gui::TriggerAction          _getColumnsForRowsAction;                /**< Executes the selected-columns-for-rows query. */
    mv::gui::TriggerAction          _getRangeAction;                         /**< Executes the rectangular range query. */
    mv::gui::StringAction           _rowFilterAction;                        /**< Filter expression selecting displayed rows. */
    mv::gui::StringAction           _columnFilterAction;                     /**< Filter expression selecting displayed columns. */
    mv::gui::ToggleAction           _liveTableAction;                        /**< Whether visible table ranges update automatically. */
};
