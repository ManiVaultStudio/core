// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointsViewWidget.h"

#include <CoreInterface.h>
#include "PointData.h"

#include <QAbstractItemView>
#include <QEvent>
#include <QHeaderView>
#include <QLabel>
#include <QScrollBar>
#include <QSpinBox>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>
#include <limits>
#include <numeric>
#include <unordered_set>

namespace
{
float valueAt(const PointDataAccessView& view, const std::uint64_t row, const std::uint64_t column)
{
    const auto* bytes = view.rowData(row) + column * view.elementSizeBytes;

    switch (view.elementTypeIndex)
    {
        case 0:
            return *reinterpret_cast<const float*>(bytes);

        case 1:
            return static_cast<float>(*reinterpret_cast<const biovault::bfloat16_t*>(bytes));

        case 2:
            return static_cast<float>(*reinterpret_cast<const std::int32_t*>(bytes));

        case 3:
            return static_cast<float>(*reinterpret_cast<const std::uint32_t*>(bytes));

        case 4:
            return static_cast<float>(*reinterpret_cast<const std::int16_t*>(bytes));

        case 5:
            return static_cast<float>(*reinterpret_cast<const std::uint16_t*>(bytes));

        case 6:
            return static_cast<float>(*reinterpret_cast<const std::int8_t*>(bytes));

        case 7:
            return static_cast<float>(*reinterpret_cast<const std::uint8_t*>(bytes));

        default:
            return 0.0f;
    }
}
}

PointsViewWidget::PointsViewWidget(QWidget* parent, const bool showDatasetControls) :
    QWidget(parent),
    _showDatasetControls(showDatasetControls),
    _datasetLabel(showDatasetControls ? new QLabel("No Points dataset selected", this) : nullptr),
    _table(new QTableView(this)),
    _model(new PointsTableModel(this)),
    _fetchTimer(new QTimer(this)),
    _filterTimer(new QTimer(this)),
    _filterGroupAction(this, "Point-data filters"),
    _facadeGroupAction(this, "Point-data queries"),
    _getRowGroupAction(&_facadeGroupAction, "Single row"),
    _getRowRangeGroupAction(&_facadeGroupAction, "Row range"),
    _getRowsGroupAction(&_facadeGroupAction, "Rows"),
    _getColumnGroupAction(&_facadeGroupAction, "Single column"),
    _getColumnRangeGroupAction(&_facadeGroupAction, "Column range"),
    _getColumnsGroupAction(&_facadeGroupAction, "Columns"),
    _getColumnsForRowsGroupAction(&_facadeGroupAction, "Columns for rows"),
    _getRangeGroupAction(&_facadeGroupAction, "Row-column range"),
    _rowIdAction(&_getRowGroupAction, "Row ID", 0, 1000000000, 0),
    _columnIdAction(&_getColumnGroupAction, "Column ID", 0, 1000000000, 0),
    _rowStartAction(&_getRowRangeGroupAction, "Row start", 0, 1000000000, 0),
    _rowEndAction(&_getRowRangeGroupAction, "Row end", 0, 1000000000, 0),
    _columnStartAction(&_getColumnRangeGroupAction, "Column start", 0, 1000000000, 0),
    _columnEndAction(&_getColumnRangeGroupAction, "Column end", 0, 1000000000, 0),
    _rangeRowStartAction(&_getRangeGroupAction, "Row start", 0, 1000000000, 0),
    _rangeRowEndAction(&_getRangeGroupAction, "Row end", 0, 1000000000, 0),
    _rangeColumnStartAction(&_getRangeGroupAction, "Column start", 0, 1000000000, 0),
    _rangeColumnEndAction(&_getRangeGroupAction, "Column end", 0, 1000000000, 0),
    _columnsForRowsStartAction(&_getColumnsForRowsGroupAction, "Column start", 0, 1000000000, 0),
    _columnsForRowsEndAction(&_getColumnsForRowsGroupAction, "Column end", 0, 1000000000, 0),
    _rowIdsAction(&_getRowsGroupAction, "IDs"),
    _columnIdsAction(&_getColumnsGroupAction, "Column IDs"),
    _columnRowIdsAction(&_getColumnsForRowsGroupAction, "Row IDs"),
    _getRowAction(&_getRowGroupAction, "Get row"),
    _getRowRangeAction(&_getRowRangeGroupAction, "Get row range"),
    _getRowsAction(&_getRowsGroupAction, "Get rows"),
    _getColumnAction(&_getColumnGroupAction, "Get column"),
    _getColumnRangeAction(&_getColumnRangeGroupAction, "Get column range"),
    _getColumnsAction(&_getColumnsGroupAction, "Get columns"),
    _getColumnsForRowsAction(&_getColumnsForRowsGroupAction, "Get columns for rows"),
    _getRangeAction(&_getRangeGroupAction, "Get range"),
    _rowFilterAction(this, "Rows"),
    _columnFilterAction(this, "Columns"),
    _liveTableAction(this, "Live", true)
{
    _rowFilterAction.setPlaceHolderString("Required; e.g. 1, 3, 10-20");
    _columnFilterAction.setPlaceHolderString("Optional = all columns; e.g. 0-4, DIM_8");
    _rowFilterAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
    _columnFilterAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
    _filterGroupAction.setShowLabels(true);
    _filterGroupAction.addAction(&_rowFilterAction);
    _filterGroupAction.addAction(&_columnFilterAction);
    _filterGroupAction.addAction(&_liveTableAction, mv::gui::ToggleAction::CheckBox);
    _facadeGroupAction.setIconByName("magnifying-glass");
    _facadeGroupAction.setShowLabels(true);
    _facadeGroupAction.setConfigurationFlag(mv::gui::WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _facadeGroupAction.setConfigurationFlag(mv::gui::WidgetAction::ConfigurationFlag::NoLabelInGroup);
    _facadeGroupAction.setPopupSizeHint(QSize(280, 0));
    _facadeGroupAction.setToolTip("Point-data queries provide storage-independent row and column retrieval.\n\nUse the single, range, ID-list, and row-column range actions below to exercise the Points facade.\n\nExamples are implemented in ManiVault/src/plugins/PointData/src/PointsViewWidget.cpp: fetchVisibleRange() demonstrates table access, while getRow(), getRowRange(), getRows(), getColumn(), getColumnRange(), getColumns(), getColumnsForRows(), and getRange() demonstrate the individual facade calls. displayQuery() shows how returned views are displayed.");
    _getRowGroupAction.setShowLabels(false);
    _getRowRangeGroupAction.setShowLabels(false);
    _getRowsGroupAction.setShowLabels(false);
    _getColumnGroupAction.setShowLabels(false);
    _getColumnRangeGroupAction.setShowLabels(false);
    _getColumnsGroupAction.setShowLabels(false);
    _getColumnsForRowsGroupAction.setShowLabels(false);
    _getRangeGroupAction.setShowLabels(false);
    const auto configureIntegralWidget = [](mv::gui::WidgetAction* action, QWidget* widget) {
        action->setStretch(0);
        widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        widget->setFixedWidth(40);

        if (auto* spinBox = widget->findChild<QSpinBox*>("SpinBox"))
            spinBox->setFixedWidth(40);
    };

    _rowIdAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
    _columnIdAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
    _rowStartAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
    _rowEndAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
    _columnStartAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
    _columnEndAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
    _rangeRowStartAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
    _rangeRowEndAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
    _rangeColumnStartAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
    _rangeColumnEndAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
    _columnsForRowsStartAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
    _columnsForRowsEndAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
    _rowIdAction.setPrefix("ID: ");
    _columnIdAction.setPrefix("ID: ");
    _rowStartAction.setPrefix("Start: ");
    _rowEndAction.setPrefix("End: ");
    _rangeRowStartAction.setPrefix("Row start: ");
    _rangeRowEndAction.setPrefix("Row end: ");
    _columnStartAction.setPrefix("Start: ");
    _columnEndAction.setPrefix("End: ");
    _rangeColumnStartAction.setPrefix("Column start: ");
    _rangeColumnEndAction.setPrefix("Column end: ");
    _columnsForRowsStartAction.setPrefix("Start: ");
    _columnsForRowsEndAction.setPrefix("End: ");
    _getRowAction.setIconByName("download");
    _getRowRangeAction.setIconByName("download");
    _getRowsAction.setIconByName("download");
    _getColumnAction.setIconByName("download");
    _getColumnRangeAction.setIconByName("download");
    _getColumnsAction.setIconByName("download");
    _getColumnsForRowsAction.setIconByName("download");
    _getRangeAction.setIconByName("download");
    _getRowGroupAction.setToolTip("Points::getRow(row)");
    _getRowRangeGroupAction.setToolTip("Points::visitRows(firstRow, rowCount)");
    _getRowsGroupAction.setToolTip("Points::visitRows(rowIds)");
    _getColumnGroupAction.setToolTip("Points::visitColumns(firstColumn, columnCount)");
    _getColumnRangeGroupAction.setToolTip("Points::visitColumns(firstColumn, columnCount)");
    _getColumnsGroupAction.setToolTip("Points::visitColumns(columnIds)");
    _getColumnsForRowsGroupAction.setToolTip("Points::visitColumns(firstColumn, columnCount, rowIds)");
    _getRangeGroupAction.setToolTip("Points::visitRange(firstRow, rowCount, firstColumn, columnCount)");
    _getRowAction.setToolTip("Points::getRow(row)");
    _getRowRangeAction.setToolTip("Points::visitRows(firstRow, rowCount)");
    _getRowsAction.setToolTip("Points::visitRows(rowIds)");
    _getColumnAction.setToolTip("Points::visitColumns(firstColumn, columnCount)");
    _getColumnRangeAction.setToolTip("Points::visitColumns(firstColumn, columnCount)");
    _getColumnsAction.setToolTip("Points::visitColumns(columnIds)");
    _getColumnsForRowsAction.setToolTip("Points::visitColumns(firstColumn, columnCount, rowIds)");
    _getRangeAction.setToolTip("Points::visitRange(firstRow, rowCount, firstColumn, columnCount)");
    _rowIdsAction.setPlaceHolderString("e.g. 1, 3, 10-20");
    _columnIdsAction.setPlaceHolderString("e.g. 1, 3, DIM_10");
    _columnRowIdsAction.setPlaceHolderString("Row IDs; e.g. 1, 3, 10-20");
    _getRowGroupAction.addAction(&_rowIdAction, -1, configureIntegralWidget);
    _getRowGroupAction.addAction(&_getRowAction, mv::gui::TriggerAction::Icon);
    _getRowRangeGroupAction.addAction(&_rowStartAction, -1, configureIntegralWidget);
    _getRowRangeGroupAction.addAction(&_rowEndAction, -1, configureIntegralWidget);
    _getRowRangeGroupAction.addAction(&_getRowRangeAction, mv::gui::TriggerAction::Icon);
    _getRowsGroupAction.addAction(&_rowIdsAction);
    _getRowsGroupAction.addAction(&_getRowsAction, mv::gui::TriggerAction::Icon);
    _getColumnGroupAction.addAction(&_columnIdAction, -1, configureIntegralWidget);
    _getColumnGroupAction.addAction(&_getColumnAction, mv::gui::TriggerAction::Icon);
    _getColumnRangeGroupAction.addAction(&_columnStartAction, -1, configureIntegralWidget);
    _getColumnRangeGroupAction.addAction(&_columnEndAction, -1, configureIntegralWidget);
    _getColumnRangeGroupAction.addAction(&_getColumnRangeAction, mv::gui::TriggerAction::Icon);
    _getColumnsGroupAction.addAction(&_columnIdsAction);
    _getColumnsGroupAction.addAction(&_getColumnsAction, mv::gui::TriggerAction::Icon);
    _getColumnsForRowsGroupAction.addAction(&_columnsForRowsStartAction, -1, configureIntegralWidget);
    _getColumnsForRowsGroupAction.addAction(&_columnsForRowsEndAction, -1, configureIntegralWidget);
    _getColumnsForRowsGroupAction.addAction(&_columnRowIdsAction);
    _getColumnsForRowsGroupAction.addAction(&_getColumnsForRowsAction, mv::gui::TriggerAction::Icon);
    _getRangeGroupAction.addAction(&_rangeRowStartAction, -1, configureIntegralWidget);
    _getRangeGroupAction.addAction(&_rangeRowEndAction, -1, configureIntegralWidget);
    _getRangeGroupAction.addAction(&_rangeColumnStartAction, -1, configureIntegralWidget);
    _getRangeGroupAction.addAction(&_rangeColumnEndAction, -1, configureIntegralWidget);
    _getRangeGroupAction.addAction(&_getRangeAction, mv::gui::TriggerAction::Icon);
    _facadeGroupAction.addAction(&_getRowGroupAction);
    _facadeGroupAction.addAction(&_getRowRangeGroupAction);
    _facadeGroupAction.addAction(&_getRowsGroupAction);
    _facadeGroupAction.addAction(&_getColumnGroupAction);
    _facadeGroupAction.addAction(&_getColumnRangeGroupAction);
    _facadeGroupAction.addAction(&_getColumnsGroupAction);
    _facadeGroupAction.addAction(&_getColumnsForRowsGroupAction);
    _facadeGroupAction.addAction(&_getRangeGroupAction);
    _filterGroupAction.addAction(&_facadeGroupAction);

    _table->setModel(_model);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setAlternatingRowColors(true);
    _table->setWordWrap(false);
    _table->verticalHeader()->setDefaultSectionSize(22);
    _table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    _table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    auto* layout = new QVBoxLayout(this);
    if (_datasetLabel)
        layout->addWidget(_datasetLabel);

    layout->addWidget(_filterGroupAction.createWidget(this));
    layout->addWidget(_table, 1);

    _table->verticalScrollBar()->installEventFilter(this);
    _table->horizontalScrollBar()->installEventFilter(this);
    _table->viewport()->installEventFilter(this);
    _fetchTimer->setSingleShot(true);
    _fetchTimer->setInterval(100);
    _filterTimer->setSingleShot(true);
    _filterTimer->setInterval(350);

    connect(&_rowFilterAction, &mv::gui::StringAction::stringChanged, this, [this] {
        updateUiState();
        _filterTimer->stop();

        QString error;
        const auto maximumRow = _model->totalRows() > 0 ? _model->totalRows() - 1 : 0;
        const auto maximumColumn = _model->totalColumns() > 0 ? _model->totalColumns() - 1 : 0;
        const auto rowIds = parseFilter(_rowFilterAction.getString(), maximumRow, error);
        const auto columnIds = parseFilter(_columnFilterAction.getString(), maximumColumn, error, _points.isValid() ? _points->getDimensionNames() : std::vector<QString>());

        if (_model->hasPointData() && !_liveTableAction.isChecked() && error.isEmpty() && !rowIds.empty() && (_columnFilterAction.getString().trimmed().isEmpty() || !columnIds.empty()))
            _filterTimer->start();
    });
    connect(&_columnFilterAction, &mv::gui::StringAction::stringChanged, this, [this] {
        updateUiState();
        _filterTimer->stop();

        QString error;
        const auto maximumRow = _model->totalRows() > 0 ? _model->totalRows() - 1 : 0;
        const auto maximumColumn = _model->totalColumns() > 0 ? _model->totalColumns() - 1 : 0;
        const auto rowIds = parseFilter(_rowFilterAction.getString(), maximumRow, error);
        const auto columnIds = parseFilter(_columnFilterAction.getString(), maximumColumn, error, _points.isValid() ? _points->getDimensionNames() : std::vector<QString>());

        if (_model->hasPointData() && !_liveTableAction.isChecked() && error.isEmpty() && !rowIds.empty() && (_columnFilterAction.getString().trimmed().isEmpty() || !columnIds.empty()))
            _filterTimer->start();
    });

    connect(_filterTimer, &QTimer::timeout, this, &PointsViewWidget::applyFilters);
    connect(&_liveTableAction, &mv::gui::ToggleAction::toggled, this, &PointsViewWidget::showLiveTable);
    connect(&_getRowAction, &mv::gui::TriggerAction::triggered, this, &PointsViewWidget::getRow);
    connect(&_getRowRangeAction, &mv::gui::TriggerAction::triggered, this, &PointsViewWidget::getRowRange);
    connect(&_getRowsAction, &mv::gui::TriggerAction::triggered, this, &PointsViewWidget::getRows);
    connect(&_getColumnAction, &mv::gui::TriggerAction::triggered, this, &PointsViewWidget::getColumn);
    connect(&_getColumnRangeAction, &mv::gui::TriggerAction::triggered, this, &PointsViewWidget::getColumnRange);
    connect(&_getColumnsAction, &mv::gui::TriggerAction::triggered, this, &PointsViewWidget::getColumns);
    connect(&_getColumnsForRowsAction, &mv::gui::TriggerAction::triggered, this, &PointsViewWidget::getColumnsForRows);
    connect(&_getRangeAction, &mv::gui::TriggerAction::triggered, this, &PointsViewWidget::getRange);
    connect(&_rowIdAction, &mv::gui::IntegralAction::valueChanged, this, &PointsViewWidget::updateFacadeActionState);
    connect(&_columnIdAction, &mv::gui::IntegralAction::valueChanged, this, &PointsViewWidget::updateFacadeActionState);
    connect(&_rowStartAction, &mv::gui::IntegralAction::valueChanged, this, &PointsViewWidget::updateFacadeActionState);
    connect(&_rowEndAction, &mv::gui::IntegralAction::valueChanged, this, &PointsViewWidget::updateFacadeActionState);
    connect(&_columnStartAction, &mv::gui::IntegralAction::valueChanged, this, &PointsViewWidget::updateFacadeActionState);
    connect(&_columnEndAction, &mv::gui::IntegralAction::valueChanged, this, &PointsViewWidget::updateFacadeActionState);
    connect(&_rangeRowStartAction, &mv::gui::IntegralAction::valueChanged, this, &PointsViewWidget::updateFacadeActionState);
    connect(&_rangeRowEndAction, &mv::gui::IntegralAction::valueChanged, this, &PointsViewWidget::updateFacadeActionState);
    connect(&_rangeColumnStartAction, &mv::gui::IntegralAction::valueChanged, this, &PointsViewWidget::updateFacadeActionState);
    connect(&_rangeColumnEndAction, &mv::gui::IntegralAction::valueChanged, this, &PointsViewWidget::updateFacadeActionState);
    connect(&_columnsForRowsStartAction, &mv::gui::IntegralAction::valueChanged, this, &PointsViewWidget::updateFacadeActionState);
    connect(&_columnsForRowsEndAction, &mv::gui::IntegralAction::valueChanged, this, &PointsViewWidget::updateFacadeActionState);
    connect(&_rowIdsAction, &mv::gui::StringAction::stringChanged, this, [this] {
        updateFacadeActionState();
    });
    connect(&_columnIdsAction, &mv::gui::StringAction::stringChanged, this, [this] {
        updateFacadeActionState();
    });
    connect(&_columnRowIdsAction, &mv::gui::StringAction::stringChanged, this, [this] {
        updateFacadeActionState();
    });
    connect(_fetchTimer, &QTimer::timeout, this, &PointsViewWidget::fetchVisibleRange);
    connect(_table->verticalScrollBar(), &QScrollBar::valueChanged, this, &PointsViewWidget::scheduleVisibleRangeFetch);
    connect(_table->horizontalScrollBar(), &QScrollBar::valueChanged, this, &PointsViewWidget::scheduleVisibleRangeFetch);

}

void PointsViewWidget::setPoints(const mv::Dataset<Points>& points)
{
    _points = points;
    refreshPoints();
}

PointsViewWidget::ViewState PointsViewWidget::getViewState() const
{
    return { _rowFilterAction.getString(), _columnFilterAction.getString(), _liveTableAction.isChecked() };
}

void PointsViewWidget::setViewState(const ViewState& state)
{
    _filterTimer->stop();
    _rowFilterAction.setString(state.rowFilter);
    _columnFilterAction.setString(state.columnFilter);
    _liveTableAction.setChecked(state.live);

    if (!state.live && !_rowFilterAction.getString().trimmed().isEmpty())
        applyFilters();
}

void PointsViewWidget::setStatus(const QString& text)
{
    Q_UNUSED(text);
}

void PointsViewWidget::updateUiState()
{
    const auto hasPointData    = _model->hasPointData();
    const auto controlsEnabled = hasPointData;
    const auto tableEnabled    = hasPointData;
    const auto liveTable       = _liveTableAction.isChecked();

    _table->setEnabled(tableEnabled);

    _rowFilterAction.setEnabled(controlsEnabled && !liveTable);
    _columnFilterAction.setEnabled(controlsEnabled && !liveTable);
    _liveTableAction.setEnabled(controlsEnabled);
    _facadeGroupAction.setEnabled(controlsEnabled);
    updateFacadeActionState();
}

void PointsViewWidget::updateFacadeActionState()
{
    if (!_points.isValid())
    {
        _getRowAction.setEnabled(false);
        _getRowRangeAction.setEnabled(false);
        _getRowsAction.setEnabled(false);
        _getColumnAction.setEnabled(false);
        _getColumnRangeAction.setEnabled(false);
        _getColumnsAction.setEnabled(false);
        _getColumnsForRowsAction.setEnabled(false);
        _getRangeAction.setEnabled(false);
        return;
    }

    const auto pointCount           = _points->getNumPoints();
    const auto dimensionCount       = _points->getNumDimensions();
    const auto rowId                = static_cast<std::uint64_t>(_rowIdAction.getValue());
    const auto columnId             = static_cast<std::uint64_t>(_columnIdAction.getValue());
    const auto rowStart             = static_cast<std::uint64_t>(_rowStartAction.getValue());
    const auto rowEnd               = static_cast<std::uint64_t>(_rowEndAction.getValue());
    const auto columnStart          = static_cast<std::uint64_t>(_columnStartAction.getValue());
    const auto columnEnd            = static_cast<std::uint64_t>(_columnEndAction.getValue());
    const auto rangeRowStart        = static_cast<std::uint64_t>(_rangeRowStartAction.getValue());
    const auto rangeRowEnd          = static_cast<std::uint64_t>(_rangeRowEndAction.getValue());
    const auto rangeColumnStart     = static_cast<std::uint64_t>(_rangeColumnStartAction.getValue());
    const auto rangeColumnEnd       = static_cast<std::uint64_t>(_rangeColumnEndAction.getValue());
    const auto columnsForRowsStart  = static_cast<std::uint64_t>(_columnsForRowsStartAction.getValue());
    const auto columnsForRowsEnd    = static_cast<std::uint64_t>(_columnsForRowsEndAction.getValue());

    QString rowIdsError;
    QString columnIdsError;
    QString columnRowsError;
    const auto rowIds       = pointCount > 0 ? parseFilter(_rowIdsAction.getString(), pointCount - 1, rowIdsError) : std::vector<std::uint64_t>();
    const auto columnIds    = dimensionCount > 0 ? parseFilter(_columnIdsAction.getString(), dimensionCount - 1, columnIdsError, _points->getDimensionNames()) : std::vector<std::uint64_t>();
    const auto columnRowIds = pointCount > 0 ? parseFilter(_columnRowIdsAction.getString(), pointCount - 1, columnRowsError) : std::vector<std::uint64_t>();

    _getRowAction.setEnabled(pointCount > 0 && rowId < pointCount);
    _getRowRangeAction.setEnabled(pointCount > 0 && rowStart <= rowEnd && rowEnd < pointCount);
    _getRowsAction.setEnabled(!rowIds.empty() && rowIdsError.isEmpty());
    _getColumnAction.setEnabled(dimensionCount > 0 && columnId < dimensionCount);
    _getColumnRangeAction.setEnabled(dimensionCount > 0 && columnStart <= columnEnd && columnEnd < dimensionCount);
    _getColumnsAction.setEnabled(!columnIds.empty() && columnIdsError.isEmpty());
    _getColumnsForRowsAction.setEnabled(dimensionCount > 0 && !columnRowIds.empty() && columnRowsError.isEmpty() && columnsForRowsStart <= columnsForRowsEnd && columnsForRowsEnd < dimensionCount);
    _getRangeAction.setEnabled(pointCount > 0 && dimensionCount > 0 && rangeRowStart <= rangeRowEnd && rangeRowEnd < pointCount && rangeColumnStart <= rangeColumnEnd && rangeColumnEnd < dimensionCount);
}

void PointsViewWidget::refreshPoints()
{
    _model->clear();
    updateUiState();

    if (!_points.isValid())
    {
        if (_datasetLabel)
            _datasetLabel->setText("Select exactly one Points dataset");
        setStatus("Scroll after selecting a dataset to fetch visible values");
        return;
    }

    _model->setPoints(_points.get(), _points->getNumPoints(), _points->getNumDimensions());
    updateUiState();
    if (_datasetLabel)
        _datasetLabel->setText(QString("%1 — %2 points × %3 dimensions (%4)")
            .arg(_points->getGuiName())
            .arg(_points->getNumPoints())
            .arg(_points->getNumDimensions())
            .arg(_points->getStorageBackendType() == Points::StorageBackendType::Tiled ? "tiled" : "eager"));
    setStatus("Fetching the visible table window...");
    QTimer::singleShot(0, this, &PointsViewWidget::fetchVisibleRange);
}

void PointsViewWidget::scheduleVisibleRangeFetch()
{
    if (_model->hasPointData() && !_model->isQueryResult())
        _fetchTimer->start();
}

void PointsViewWidget::fetchVisibleRange()
{
    if (!_model->hasPointData() || _model->isQueryResult())
        return;

    if (_model->totalRows() == 0 || _model->totalColumns() == 0)
    {
        setStatus("The selected point dataset is empty");
        return;
    }

    const auto firstRow    = std::max(0, _table->rowAt(0));
    const auto lastRow     = _table->rowAt(std::max(0, _table->viewport()->height() - 1));
    const auto firstColumn = std::max(0, _table->columnAt(0));
    const auto lastColumn  = _table->columnAt(std::max(0, _table->viewport()->width() - 1));
    const auto rowCount    = lastRow >= firstRow
        ? static_cast<std::uint64_t>(lastRow - firstRow + 1)
        : 1;
    const auto columnCount = lastColumn >= firstColumn
        ? static_cast<std::uint64_t>(lastColumn - firstColumn + 1)
        : 1;
    const auto callbacks = _model->fetchRange(static_cast<std::uint64_t>(firstRow), rowCount, static_cast<std::uint64_t>(firstColumn), columnCount);

    if (!_model->getLastError().isEmpty())
    {
        setStatus(QString("Unable to load the requested point-data tiles: %1").arg(_model->getLastError()));
        return;
    }

    if (!_points.isValid())
    {
        return;
    }

    setStatus(QString("Fetched visible rows %1–%2, columns %3–%4 in %5 access segment(s). Drag/scroll and release to fetch again.")
        .arg(firstRow)
        .arg(firstRow + static_cast<int>(rowCount) - 1)
        .arg(firstColumn)
        .arg(firstColumn + static_cast<int>(columnCount) - 1)
        .arg(callbacks));
}

std::vector<std::uint64_t> PointsViewWidget::parseFilter(const QString& filter, const std::uint64_t maximum, QString& error, const std::vector<QString>& dimensionNames)
{
    std::vector<std::uint64_t> values;
    std::unordered_set<std::uint64_t> seen;

    for (const auto& part : filter.split(',', Qt::SkipEmptyParts))
    {
        const auto expression = part.trimmed();
        const auto rangeParts = expression.split('-', Qt::KeepEmptyParts);

        bool matchedName = false;

        for (std::size_t index = 0; index < dimensionNames.size(); ++index)
        {
            if (dimensionNames[index].compare(expression, Qt::CaseInsensitive) == 0)
            {
                const auto value = static_cast<std::uint64_t>(index);

                if (seen.insert(value).second)
                    values.push_back(value);

                matchedName = true;
                break;
            }
        }

        if (matchedName)
            continue;

        if (rangeParts.size() > 2 || rangeParts.contains(QString()))
        {
            error = QString("Invalid filter expression: %1").arg(expression);
            return {};
        }

        bool firstOk = false;
        auto first = rangeParts.first().trimmed().toULongLong(&firstOk);
        auto last = first;

        if (rangeParts.size() > 1)
            last = rangeParts.last().trimmed().toULongLong(&firstOk);

        if (!firstOk || first > maximum || last > maximum || first > last)
        {
            error = QString("Invalid filter range: %1").arg(expression);
            return {};
        }

        for (auto value = first; value <= last; ++value)
        {
            if (seen.insert(value).second)
                values.push_back(value);

            if (value == std::numeric_limits<std::uint64_t>::max())
                break;
        }
    }

    return values;
}

void PointsViewWidget::displayQuery(const std::vector<std::uint64_t>& rowIds, const std::vector<std::uint64_t>& columnIds, const std::function<void(const PointDataAccessVisitor&)>& request)
{
    if (rowIds.empty() || columnIds.empty())
    {
        setStatus("The query must contain at least one row and one column");
        return;
    }

    std::vector<float> values(rowIds.size() * columnIds.size(), 0.0f);
    std::uint64_t destinationRow    = 0;
    std::uint64_t destinationColumn = 0;

    try
    {
        request([&](const PointDataAccessView& view) {
            for (std::uint64_t row = 0; row < view.rows; ++row)
            {
                for (std::uint64_t column = 0; column < view.columns; ++column)
                {
                    const auto targetRow    = destinationRow + row;
                    const auto targetColumn = destinationColumn + column;

                    if (targetRow < rowIds.size() && targetColumn < columnIds.size())
                        values[static_cast<std::size_t>(targetRow * columnIds.size() + targetColumn)] = valueAt(view, row, column);
                }
            }

            destinationColumn += view.columns;

            if (destinationColumn >= columnIds.size())
            {
                destinationColumn = 0;
                destinationRow += view.rows;
            }
        });
    }
    catch (const std::exception& exception)
    {
        setStatus(QString("Unable to load the requested point-data tiles: %1").arg(QString::fromUtf8(exception.what())));
        return;
    }
    catch (...)
    {
        setStatus("Unable to load the requested point-data tiles: unknown error");
        return;
    }

    _liveTableAction.setChecked(false);
    _model->setQueryResult(rowIds, columnIds, values);
    _table->resizeColumnsToContents();
    setStatus(QString("Displayed %1 row(s) × %2 column(s) from a facade query. Click Live to resume viewport loading.").arg(rowIds.size()).arg(columnIds.size()));
}

void PointsViewWidget::applyFilters()
{
    if (!_points.isValid())
        return;

    QString error;
    const auto maximumRow    = _points->getNumPoints() > 0 ? _points->getNumPoints() - 1 : 0;
    const auto maximumColumn = _points->getNumDimensions() > 0 ? _points->getNumDimensions() - 1 : 0;
    auto rowIds    = parseFilter(_rowFilterAction.getString(), maximumRow, error);
    if (!error.isEmpty())
    {
        setStatus(error);
        return;
    }

    auto columnIds = parseFilter(_columnFilterAction.getString(), maximumColumn, error, _points->getDimensionNames());
    if (!error.isEmpty())
    {
        setStatus(error);
        return;
    }

    if (rowIds.empty())
    {
        setStatus("The row filter must contain at least one valid value");
        return;
    }

    if (_columnFilterAction.getString().trimmed().isEmpty())
    {
        columnIds.resize(static_cast<std::size_t>(_points->getNumDimensions()));
        std::iota(columnIds.begin(), columnIds.end(), 0);
    }

    const auto contiguousRows    = std::adjacent_find(rowIds.begin(), rowIds.end(), [](const auto first, const auto second) {
        return second != first + 1;
    }) == rowIds.end();
    const auto contiguousColumns = std::adjacent_find(columnIds.begin(), columnIds.end(), [](const auto first, const auto second) {
        return second != first + 1;
    }) == columnIds.end();

    auto points = _points;

    displayQuery(rowIds, columnIds, [points, rowIds, columnIds, contiguousRows, contiguousColumns](const PointDataAccessVisitor& visitor) {
        if (contiguousRows && contiguousColumns)
        {
            points->visitRange(rowIds.front(), rowIds.size(), columnIds.front(), columnIds.size(), visitor);
            return;
        }

        std::vector<std::pair<std::uint64_t, std::uint64_t>> columnRanges;
        for (std::size_t index = 0; index < columnIds.size();)
        {
            const auto first = columnIds[index];
            auto end = index + 1;

            while (end < columnIds.size() && columnIds[end] == columnIds[end - 1] + 1)
                ++end;

            columnRanges.emplace_back(first, end - index);
            index = end;
        }

        std::vector<std::pair<std::uint64_t, std::uint64_t>> rowRanges;
        for (std::size_t index = 0; index < rowIds.size();)
        {
            const auto first = rowIds[index];
            auto end = index + 1;

            while (end < rowIds.size() && rowIds[end] == rowIds[end - 1] + 1)
                ++end;

            rowRanges.emplace_back(first, end - index);
            index = end;
        }

        for (const auto& [firstRow, rowCount] : rowRanges)
        {
            for (const auto& [firstColumn, columnCount] : columnRanges)
                points->visitRange(firstRow, rowCount, firstColumn, columnCount, visitor);
        }
    });
}

void PointsViewWidget::showLiveTable(const bool live)
{
    _filterTimer->stop();

    if (live)
    {
        updateUiState();
        refreshPoints();
    }
    else
    {
        updateUiState();
    }
}

void PointsViewWidget::getRow()
{
    auto points = _points;

    if (!points.isValid() || _rowIdAction.getValue() >= points->getNumPoints())
    {
        setStatus("The requested row is outside the selected point dataset");
        return;
    }

    const auto row = static_cast<std::uint64_t>(_rowIdAction.getValue());
    std::vector<std::uint64_t> rowIds{ row };
    std::vector<std::uint64_t> columnIds(points->getNumDimensions());
    std::iota(columnIds.begin(), columnIds.end(), 0);

    displayQuery(rowIds, columnIds, [points, row](const PointDataAccessVisitor& visitor) {
        visitor(points->getRow(row));
    });
}

void PointsViewWidget::getRowRange()
{
    const auto points = _points;

    const auto firstRow = static_cast<std::uint64_t>(_rowStartAction.getValue());
    const auto lastRow  = static_cast<std::uint64_t>(_rowEndAction.getValue());

    if (!points.isValid() || firstRow > lastRow || lastRow >= points->getNumPoints())
    {
        setStatus("The requested row range is invalid");
        return;
    }

    std::vector<std::uint64_t> rowIds(lastRow - firstRow + 1);
    std::iota(rowIds.begin(), rowIds.end(), firstRow);
    std::vector<std::uint64_t> columnIds(points->getNumDimensions());
    std::iota(columnIds.begin(), columnIds.end(), 0);

    displayQuery(rowIds, columnIds, [points, firstRow, rowIds](const PointDataAccessVisitor& visitor) {
        points->visitRows(firstRow, rowIds.size(), visitor);
    });
}

void PointsViewWidget::getRows()
{
    const auto points = _points;

    QString error;
    const auto rowIds = points.isValid() ? parseFilter(_rowIdsAction.getString(), points->getNumPoints() - 1, error) : std::vector<std::uint64_t>();

    if (!points.isValid() || rowIds.empty() || !error.isEmpty())
    {
        setStatus(error.isEmpty() ? "The requested row list is invalid" : error);
        return;
    }

    std::vector<std::uint64_t> columnIds(points->getNumDimensions());
    std::iota(columnIds.begin(), columnIds.end(), 0);

    displayQuery(rowIds, columnIds, [points, rowIds](const PointDataAccessVisitor& visitor) {
        points->visitRows(rowIds, visitor);
    });
}

void PointsViewWidget::getColumn()
{
    const auto points = _points;

    const auto column = static_cast<std::uint64_t>(_columnIdAction.getValue());

    if (!points.isValid() || column >= points->getNumDimensions())
    {
        setStatus("The requested column is outside the selected point dataset");
        return;
    }

    std::vector<std::uint64_t> rowIds(points->getNumPoints());
    std::iota(rowIds.begin(), rowIds.end(), 0);
    std::vector<std::uint64_t> columnIds{ column };

    displayQuery(rowIds, columnIds, [points, column](const PointDataAccessVisitor& visitor) {
        points->visitColumns(column, 1, visitor);
    });
}

void PointsViewWidget::getColumnRange()
{
    const auto points = _points;

    const auto firstColumn = static_cast<std::uint64_t>(_columnStartAction.getValue());
    const auto lastColumn  = static_cast<std::uint64_t>(_columnEndAction.getValue());

    if (!points.isValid() || firstColumn > lastColumn || lastColumn >= points->getNumDimensions())
    {
        setStatus("The requested column range is invalid");
        return;
    }

    std::vector<std::uint64_t> rowIds(points->getNumPoints());
    std::iota(rowIds.begin(), rowIds.end(), 0);
    std::vector<std::uint64_t> columnIds(lastColumn - firstColumn + 1);
    std::iota(columnIds.begin(), columnIds.end(), firstColumn);

    displayQuery(rowIds, columnIds, [points, firstColumn, columnIds](const PointDataAccessVisitor& visitor) {
        points->visitColumns(firstColumn, columnIds.size(), visitor);
    });
}

void PointsViewWidget::getColumns()
{
    const auto points = _points;

    QString error;
    const auto columnIds = points.isValid() ? parseFilter(_columnIdsAction.getString(), points->getNumDimensions() - 1, error, points->getDimensionNames()) : std::vector<std::uint64_t>();

    if (!points.isValid() || columnIds.empty() || !error.isEmpty())
    {
        setStatus(error.isEmpty() ? "The requested column list is invalid" : error);
        return;
    }

    std::vector<std::uint64_t> rowIds(points->getNumPoints());
    std::iota(rowIds.begin(), rowIds.end(), 0);

    displayQuery(rowIds, columnIds, [points, columnIds](const PointDataAccessVisitor& visitor) {
        points->visitColumns(columnIds, visitor);
    });
}

void PointsViewWidget::getColumnsForRows()
{
    const auto points = _points;

    const auto firstColumn = static_cast<std::uint64_t>(_columnsForRowsStartAction.getValue());
    const auto lastColumn  = static_cast<std::uint64_t>(_columnsForRowsEndAction.getValue());

    QString error;
    const auto rowIds = points.isValid() ? parseFilter(_columnRowIdsAction.getString(), points->getNumPoints() - 1, error) : std::vector<std::uint64_t>();

    if (!points.isValid() || rowIds.empty() || !error.isEmpty() || firstColumn > lastColumn || lastColumn >= points->getNumDimensions())
    {
        setStatus(error.isEmpty() ? "The requested columns-for-rows range is invalid" : error);
        return;
    }

    std::vector<std::uint64_t> columnIds(lastColumn - firstColumn + 1);
    std::iota(columnIds.begin(), columnIds.end(), firstColumn);

    displayQuery(rowIds, columnIds, [points, firstColumn, columnIds, rowIds](const PointDataAccessVisitor& visitor) {
        points->visitColumns(firstColumn, columnIds.size(), rowIds, visitor);
    });
}

void PointsViewWidget::getRange()
{
    const auto points = _points;

    const auto firstRow    = static_cast<std::uint64_t>(_rangeRowStartAction.getValue());
    const auto lastRow     = static_cast<std::uint64_t>(_rangeRowEndAction.getValue());
    const auto firstColumn = static_cast<std::uint64_t>(_rangeColumnStartAction.getValue());
    const auto lastColumn  = static_cast<std::uint64_t>(_rangeColumnEndAction.getValue());

    if (!points.isValid() || firstRow > lastRow || lastRow >= points->getNumPoints() || firstColumn > lastColumn || lastColumn >= points->getNumDimensions())
    {
        setStatus("The requested point-data range is invalid");
        return;
    }

    std::vector<std::uint64_t> rowIds(lastRow - firstRow + 1);
    std::iota(rowIds.begin(), rowIds.end(), firstRow);
    std::vector<std::uint64_t> columnIds(lastColumn - firstColumn + 1);
    std::iota(columnIds.begin(), columnIds.end(), firstColumn);

    displayQuery(rowIds, columnIds, [points, firstRow, firstColumn, rowIds, columnIds](const PointDataAccessVisitor& visitor) {
        points->visitRange(firstRow, rowIds.size(), firstColumn, columnIds.size(), visitor);
    });
}

bool PointsViewWidget::eventFilter(QObject* watched, QEvent* event)
{
    if ((watched == _table->verticalScrollBar() || watched == _table->horizontalScrollBar() || watched == _table->viewport()) &&
        (event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::Wheel || event->type() == QEvent::KeyRelease))
        scheduleVisibleRangeFetch();

    return QWidget::eventFilter(watched, event);
}
