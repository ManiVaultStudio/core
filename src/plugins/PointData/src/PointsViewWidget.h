// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "PointsTableModel.h"
#include "pointdata_export.h"

#include "PointDataAccess.h"

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
    /** Constructs the points view. */
    explicit PointsViewWidget(QWidget* parent = nullptr, bool showDatasetControls = true);

    /** Sets the point-data source displayed by the widget. */
    void setPoints(const mv::Dataset<Points>& points);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

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
    static std::vector<std::uint64_t> parseFilter(const QString& filter, std::uint64_t maximum, QString& error);
    void displayQuery(const std::vector<std::uint64_t>& rowIds, const std::vector<std::uint64_t>& columnIds, const std::function<void(const PointDataAccessVisitor&)>& request);

    mv::Dataset<Points>             _points;                    /**< Dataset currently displayed by the table. */
    bool                            _showDatasetControls;       /**< Whether standalone dataset controls are shown. */
    QLabel*                         _datasetLabel;              /**< Optional dataset summary shown above the table. */
    QTableView*                     _table;                     /**< Table view displaying the fetched window. */
    PointsTableModel*               _model;                     /**< Model storing logical dimensions and fetched values. */
    QTimer*                         _fetchTimer;                /**< Coalesces visible-range requests. */
    QTimer*                         _filterTimer;               /**< Debounces filter-driven requests. */
    mv::gui::HorizontalGroupAction  _filterGroupAction;          /**< Row, column, and live-table controls. */
    mv::gui::VerticalGroupAction    _facadeGroupAction;         /**< Point-data query actions. */
    mv::gui::HorizontalGroupAction  _getRowGroupAction;
    mv::gui::HorizontalGroupAction  _getRowRangeGroupAction;
    mv::gui::HorizontalGroupAction  _getRowsGroupAction;
    mv::gui::HorizontalGroupAction  _getColumnGroupAction;
    mv::gui::HorizontalGroupAction  _getColumnRangeGroupAction;
    mv::gui::HorizontalGroupAction  _getColumnsGroupAction;
    mv::gui::HorizontalGroupAction  _getColumnsForRowsGroupAction;
    mv::gui::HorizontalGroupAction  _getRangeGroupAction;
    mv::gui::IntegralAction         _rowIdAction;
    mv::gui::IntegralAction         _columnIdAction;
    mv::gui::IntegralAction         _rowStartAction;
    mv::gui::IntegralAction         _rowEndAction;
    mv::gui::IntegralAction         _columnStartAction;
    mv::gui::IntegralAction         _columnEndAction;
    mv::gui::IntegralAction         _rangeRowStartAction;
    mv::gui::IntegralAction         _rangeRowEndAction;
    mv::gui::IntegralAction         _rangeColumnStartAction;
    mv::gui::IntegralAction         _rangeColumnEndAction;
    mv::gui::IntegralAction         _columnsForRowsStartAction;
    mv::gui::IntegralAction         _columnsForRowsEndAction;
    mv::gui::StringAction           _rowIdsAction;
    mv::gui::StringAction           _columnIdsAction;
    mv::gui::StringAction           _columnRowIdsAction;
    mv::gui::TriggerAction          _getRowAction;
    mv::gui::TriggerAction          _getRowRangeAction;
    mv::gui::TriggerAction          _getRowsAction;
    mv::gui::TriggerAction          _getColumnAction;
    mv::gui::TriggerAction          _getColumnRangeAction;
    mv::gui::TriggerAction          _getColumnsAction;
    mv::gui::TriggerAction          _getColumnsForRowsAction;
    mv::gui::TriggerAction          _getRangeAction;
    mv::gui::StringAction           _rowFilterAction;
    mv::gui::StringAction           _columnFilterAction;
    mv::gui::ToggleAction           _liveTableAction;
};
