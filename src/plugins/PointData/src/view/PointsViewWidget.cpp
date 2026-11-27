// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointsViewWidget.h"

#include <CoreInterface.h>
#include "../PointData.h"
#include "../storage/tiled/TiledPointsStorageBackend.h"

#include <QAbstractItemView>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QSaveFile>
#include <QScrollBar>
#include <QSettings>
#include <QShowEvent>
#include <QSpinBox>
#include <QSysInfo>
#include <QTableView>
#include <QTimer>
#include <QThread>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QtConcurrent/QtConcurrentRun>
#include <util/StyledIcon.h>
#include <algorithm>
#include <exception>
#include <functional>
#include <limits>
#include <numeric>
#include <unordered_set>

namespace
{
float pointViewValueAt(const PointDataAccessView& view, const std::uint64_t row, const std::uint64_t column)
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

QString makeSafeFileName(QString name)
{
    name = name.trimmed();
    for (auto& character : name)
        if (QStringLiteral("<>:\"/\\|?*").contains(character) || character.unicode() < 32)
            character = QLatin1Char('_');

    while (name.endsWith(QLatin1Char('.')) || name.endsWith(QLatin1Char(' ')))
        name.chop(1);

    return name.isEmpty() ? QStringLiteral("point-data") : name;
}

QString saveFetchBenchmark(const mv::Dataset<Points>& points, const QString& outputPath)
{
    try
    {
        const auto startedUtc = QDateTime::currentDateTimeUtc();
        constexpr int repetitions = 3;
        constexpr std::uint64_t requestedColumnCount = 11;
        constexpr std::uint64_t requestedRangeRows = 1000;
        constexpr std::uint64_t requestedRangeColumns = 32;
        const auto rowCount = points->getNumPoints();
        const auto columnCount = points->getNumDimensions();
        const auto actualMultiColumnCount = std::min(requestedColumnCount, columnCount);
        const auto actualRangeRows = std::min(requestedRangeRows, rowCount);
        const auto actualRangeColumns = std::min(requestedRangeColumns, columnCount);
        const auto* tiledBackend = dynamic_cast<const mv::pointdata::storage::TiledPointsStorageBackend*>(points->getStorageBackend());
        const auto tiledStorage = tiledBackend ? tiledBackend->getStorage() : nullptr;
        const auto resetApplicationCache = [&] {
            if (tiledStorage)
                tiledStorage->resetCache();
        };

        QJsonObject datasetObject;
        datasetObject.insert(QStringLiteral("name"), points->getGuiName());
        datasetObject.insert(QStringLiteral("rows"), static_cast<qint64>(rowCount));
        datasetObject.insert(QStringLiteral("columns"), static_cast<qint64>(columnCount));
        datasetObject.insert(QStringLiteral("element_type"), points->getElementTypeName());
        if (const auto backendType = points->getStorageBackendType())
            datasetObject.insert(QStringLiteral("storage_backend"), mv::pointdata::storage::getStorageBackendTypeName(*backendType));

        if (tiledStorage)
        {
            QJsonObject storageObject;
            storageObject.insert(QStringLiteral("tile_rows"), static_cast<qint64>(tiledStorage->rowTileSize()));
            storageObject.insert(QStringLiteral("tile_columns"), static_cast<qint64>(tiledStorage->columnTileSize()));
            storageObject.insert(QStringLiteral("cache_tiles"), static_cast<qint64>(tiledStorage->cacheCapacity()));
            storageObject.insert(QStringLiteral("cache_bytes"), static_cast<qint64>(tiledStorage->cacheCapacityBytes()));
            storageObject.insert(QStringLiteral("packed_tiles"), tiledStorage->packedTiles());
            storageObject.insert(QStringLiteral("supertile_rows"), static_cast<qint64>(tiledStorage->superTileRows()));
            storageObject.insert(QStringLiteral("supertile_columns"), static_cast<qint64>(tiledStorage->superTileColumns()));
            storageObject.insert(QStringLiteral("codec"), tiledStorage->serializationCodecName());
            datasetObject.insert(QStringLiteral("tiled_storage"), storageObject);
        }

        QJsonArray results;
        const auto measure = [&](const QString& name, const std::uint64_t requestedRows, const std::uint64_t requestedColumns, const std::function<void(const PointDataAccessVisitor&)>& query) {
            resetApplicationCache();
            QJsonArray samples;
            QJsonArray checksums;
            std::vector<double> elapsedMilliseconds;
            elapsedMilliseconds.reserve(repetitions);

            for (int repetition = 0; repetition < repetitions; ++repetition)
            {
                double checksum = 0.0;
                QElapsedTimer timer;
                timer.start();
                query([&](const PointDataAccessView& view) {
                    for (std::uint64_t row = 0; row < view.rows; ++row)
                    {
                        for (std::uint64_t column = 0; column < view.columns; ++column)
                        {
                            checksum += pointViewValueAt(view, row, column);
                        }
                    }
                });
                const auto elapsed = static_cast<double>(timer.nsecsElapsed()) / 1.0e6;
                elapsedMilliseconds.push_back(elapsed);
                samples.append(elapsed);
                checksums.append(checksum);
            }

            auto sortedTimes = elapsedMilliseconds;
            std::sort(sortedTimes.begin(), sortedTimes.end());
            QJsonObject result;
            result.insert(QStringLiteral("pattern"), name);
            result.insert(QStringLiteral("requested_rows"), static_cast<qint64>(requestedRows));
            result.insert(QStringLiteral("requested_columns"), static_cast<qint64>(requestedColumns));
            result.insert(QStringLiteral("first_run_ms"), elapsedMilliseconds.front());
            result.insert(QStringLiteral("median_ms"), sortedTimes[sortedTimes.size() / 2]);
            result.insert(QStringLiteral("samples_ms"), samples);
            result.insert(QStringLiteral("checksums"), checksums);
            results.append(result);
        };

        measure(QStringLiteral("one_full_column"), rowCount, 1, [&](const PointDataAccessVisitor& visitor) {
            points->visitColumns(0, 1, visitor);
        });
        measure(QStringLiteral("eleven_full_columns"), rowCount, actualMultiColumnCount, [&](const PointDataAccessVisitor& visitor) {
            points->visitColumns(0, actualMultiColumnCount, visitor);
        });
        measure(QStringLiteral("one_full_row"), 1, columnCount, [&](const PointDataAccessVisitor& visitor) {
            points->visitRows(0, 1, visitor);
        });
        measure(QStringLiteral("range"), actualRangeRows, actualRangeColumns, [&](const PointDataAccessVisitor& visitor) {
            points->visitRange(0, actualRangeRows, 0, actualRangeColumns, visitor);
        });

        QJsonObject benchmarkObject;
        benchmarkObject.insert(QStringLiteral("started_utc"), startedUtc.toString(Qt::ISODateWithMs));
        benchmarkObject.insert(QStringLiteral("repetitions"), repetitions);
        benchmarkObject.insert(QStringLiteral("multi_column_count"), static_cast<qint64>(actualMultiColumnCount));
        benchmarkObject.insert(QStringLiteral("column_start"), 0);
        benchmarkObject.insert(QStringLiteral("row_index"), 0);
        benchmarkObject.insert(QStringLiteral("range_rows"), static_cast<qint64>(actualRangeRows));
        benchmarkObject.insert(QStringLiteral("range_columns"), static_cast<qint64>(actualRangeColumns));
        benchmarkObject.insert(QStringLiteral("range_start_row"), 0);
        benchmarkObject.insert(QStringLiteral("range_start_column"), 0);
        benchmarkObject.insert(QStringLiteral("logical_cores"), QThread::idealThreadCount());
        benchmarkObject.insert(QStringLiteral("operating_system"), QSysInfo::prettyProductName());
        benchmarkObject.insert(QStringLiteral("timing_notes"), QStringLiteral("Timings include consuming each returned value into a checksum. The application tiled cache is reset once before each pattern. OS file cache and importer-owned strip cache are not cleared. First-run timing is therefore not guaranteed to be a cold-cache measurement; later samples may be warm."));

        QJsonObject document;
        document.insert(QStringLiteral("schema_version"), 1);
        document.insert(QStringLiteral("dataset"), datasetObject);
        document.insert(QStringLiteral("benchmark"), benchmarkObject);
        document.insert(QStringLiteral("results"), results);

        QSaveFile output(outputPath);
        if (!output.open(QIODevice::WriteOnly))
            return QStringLiteral("ERROR: Unable to open %1: %2").arg(outputPath, output.errorString());
        const auto json = QJsonDocument(document).toJson(QJsonDocument::Indented);
        if (output.write(json) != json.size() || !output.commit())
            return QStringLiteral("ERROR: Unable to save %1: %2").arg(outputPath, output.errorString());

        return outputPath;
    }
    catch (const std::exception& exception)
    {
        return QStringLiteral("ERROR: %1").arg(QString::fromUtf8(exception.what()));
    }
    catch (...)
    {
        return QStringLiteral("ERROR: Unknown failure while benchmarking point data");
    }
}
}

PointsViewWidget::PointsViewWidget(QWidget* parent, const bool showDatasetControls) :
    QWidget(parent),
    _showDatasetControls(showDatasetControls),
    _datasetLabel(showDatasetControls ? new QLabel("No Points dataset selected", this) : nullptr),
    _table(new QTableView(this)),
    _rowScrollBar(new QScrollBar(Qt::Vertical, this)),
    _model(new PointsTableModel(this)),
    _fetchWatcher(new QFutureWatcher<PointsTableModel::RangeResult>(this)),
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
    _runFetchBenchmarkAction(&_facadeGroupAction, "Benchmark fetch patterns and save JSON"),
    _benchmarkWatcher(new QFutureWatcher<QString>(this)),
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
    _facadeGroupAction.setToolTip("Point-data queries provide storage-independent row and column retrieval.\n\nUse the single, range, ID-list, and row-column range actions below to exercise the Points facade.\n\nExamples are implemented in ManiVault/src/plugins/PointData/src/view/PointsViewWidget.cpp: fetchVisibleRange() demonstrates table access, while getRow(), getRowRange(), getRows(), getColumn(), getColumnRange(), getColumns(), getColumnsForRows(), and getRange() demonstrate the individual facade calls. displayQuery() shows how returned views are displayed.");
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
    _runFetchBenchmarkAction.setIconByName("gauge-high");
    _runFetchBenchmarkAction.setToolTip("Measure one full column, eleven full columns, one full row, and a 1,000 × 32 range three times each. Save timings, checksums, dataset settings, and cache notes to a JSON file.");
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
    _facadeGroupAction.addAction(&_runFetchBenchmarkAction, mv::gui::TriggerAction::Text);
    _filterGroupAction.addAction(&_facadeGroupAction);

    _table->setModel(_model);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setAlternatingRowColors(true);
    _table->setWordWrap(false);
    _table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _table->verticalHeader()->setDefaultSectionSize(22);
    _table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    _table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    _rowScrollBar->setToolTip("Navigate through dataset rows");
    auto* tableLayout = new QHBoxLayout();
    tableLayout->setContentsMargins(0, 0, 0, 0);
    tableLayout->setSpacing(0);
    tableLayout->addWidget(_table, 1);
    tableLayout->addWidget(_rowScrollBar);
    auto* layout = new QVBoxLayout(this);
    if (_datasetLabel)
        layout->addWidget(_datasetLabel);

    layout->addWidget(_filterGroupAction.createWidget(this));
    layout->addLayout(tableLayout, 1);

    _rowScrollBar->installEventFilter(this);
    _table->horizontalScrollBar()->installEventFilter(this);
    _table->installEventFilter(this);
    _table->viewport()->installEventFilter(this);
    _fetchTimer->setSingleShot(true);
    _fetchTimer->setInterval(50);
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
    connect(&_runFetchBenchmarkAction, &mv::gui::TriggerAction::triggered, this, [this] {
        if (!_points.isValid() || _benchmarkWatcher->isRunning())
            return;

        if (_fetchInProgress)
        {
            mv::help().addNotification("Fetch benchmark unavailable", "Wait for the current table fetch to finish before starting the benchmark.", mv::util::StyledIcon("circle-info"));
            return;
        }

        const auto timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss"));
        const auto suggestedName = QStringLiteral("%1-fetch-benchmark-%2.json").arg(makeSafeFileName(_points->getGuiName()), timestamp);
        QSettings settings;
        const auto lastBenchmarkDirectory = settings.value(QStringLiteral("PointData/fetchBenchmarkDirectory"), QDir::homePath()).toString();
        const auto suggestedPath = QDir(lastBenchmarkDirectory).filePath(suggestedName);
        const auto outputPath = QFileDialog::getSaveFileName(this, tr("Save fetch benchmark"), suggestedPath, tr("JSON files (*.json)"));
        if (outputPath.isEmpty())
            return;

        settings.setValue(QStringLiteral("PointData/fetchBenchmarkDirectory"), QFileInfo(outputPath).absolutePath());

        _fetchTimer->stop();
        _filterTimer->stop();
        _liveTableAction.setChecked(false);

        const auto points = _points;
        _runFetchBenchmarkAction.setEnabled(false);
        mv::help().addNotification("Fetch benchmark started", "Measuring four point-data fetch patterns and writing JSON results.", mv::util::StyledIcon("gauge-high"));
        _benchmarkWatcher->setFuture(QtConcurrent::run([points, outputPath] {
            return saveFetchBenchmark(points, outputPath);
        }));
    });
    connect(_benchmarkWatcher, &QFutureWatcher<QString>::finished, this, [this] {
        const auto result = _benchmarkWatcher->result();
        if (result.startsWith(QStringLiteral("ERROR: ")))
            mv::help().addNotification("Fetch benchmark failed", result.mid(7), mv::util::StyledIcon("times"));
        else
            mv::help().addNotification("Fetch benchmark saved", result, mv::util::StyledIcon("check"));

        updateFacadeActionState();
    });
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
    connect(_fetchWatcher, &QFutureWatcher<PointsTableModel::RangeResult>::finished, this, &PointsViewWidget::rangeFetchFinished);
    connect(_rowScrollBar, &QScrollBar::valueChanged, this, &PointsViewWidget::rowScrollValueChanged);
    connect(_table->horizontalScrollBar(), &QScrollBar::valueChanged, this, &PointsViewWidget::scheduleVisibleRangeFetch);

}

void PointsViewWidget::setPoints(const mv::Dataset<Points>& points)
{
    disconnect(&_points, &mv::Dataset<Points>::dataChanged, this, &PointsViewWidget::refreshPoints);
    _points = points;

    if (_points.isValid())
        connect(&_points, &mv::Dataset<Points>::dataChanged, this, &PointsViewWidget::refreshPoints);

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
    _rowScrollBar->setEnabled(hasPointData);

    _rowFilterAction.setEnabled(controlsEnabled && !liveTable);
    _columnFilterAction.setEnabled(controlsEnabled && !liveTable);
    _liveTableAction.setEnabled(controlsEnabled);
    _facadeGroupAction.setEnabled(controlsEnabled);
    updateFacadeActionState();
}

int PointsViewWidget::visibleRowCapacity() const
{
    const auto rowHeight = std::max(1, _table->verticalHeader()->defaultSectionSize());
    return std::max(1, (_table->viewport()->height() + rowHeight - 1) / rowHeight);
}

void PointsViewWidget::updateRowScrollBarRange(const std::uint64_t totalRows)
{
    const auto visibleRows = static_cast<std::uint64_t>(visibleRowCapacity());
    const auto maximumFirstRow = totalRows > visibleRows ? totalRows - visibleRows : 0;
    const auto maximumValue = static_cast<int>(std::min<std::uint64_t>(maximumFirstRow, static_cast<std::uint64_t>(std::numeric_limits<int>::max())));
    _rowScrollBar->setPageStep(static_cast<int>(std::min<std::uint64_t>(visibleRows, static_cast<std::uint64_t>(std::numeric_limits<int>::max()))));
    _rowScrollBar->setRange(0, maximumValue);
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
        _runFetchBenchmarkAction.setEnabled(false);
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
    _runFetchBenchmarkAction.setEnabled(pointCount > 0 && dimensionCount > 0 && !_benchmarkWatcher->isRunning());
}

void PointsViewWidget::refreshPoints()
{
    ++_fetchGeneration;
    _fetchPending = false;
    _model->clear();
    updateUiState();

    if (!_points.isValid())
    {
        _rowScrollBar->setRange(0, 0);
        if (_datasetLabel)
            _datasetLabel->setText("Select exactly one Points dataset");
        setStatus("Scroll after selecting a dataset to fetch visible values");
        return;
    }

    _model->setPoints(_points.get(), _points->getNumPoints(), _points->getNumDimensions());
    updateRowScrollBarRange(_model->displayRowCount());
    const auto visibleRows = visibleRowCapacity();
    _model->setWindow(static_cast<std::uint64_t>(_rowScrollBar->value()), static_cast<std::uint64_t>(visibleRows));
    updateUiState();
    const auto backendType = _points->getStorageBackendType();

    if (_datasetLabel)
        _datasetLabel->setText(QString("%1 — %2 points × %3 dimensions (%4)")
            .arg(_points->getGuiName())
            .arg(_points->getNumPoints())
            .arg(_points->getNumDimensions())
            .arg(backendType ? mv::pointdata::storage::getStorageBackendTypeName(*backendType).toLower() : QStringLiteral("not uniform")));
    setStatus("Fetching the visible table window...");
    QTimer::singleShot(0, this, &PointsViewWidget::fetchVisibleRange);
}

void PointsViewWidget::scheduleVisibleRangeFetch()
{
    if (_model->hasPointData())
        _fetchTimer->start();
}

void PointsViewWidget::rowScrollValueChanged(const int firstRow)
{
    if (!_model->hasPointData())
        return;

    _model->setWindow(static_cast<std::uint64_t>(std::max(0, firstRow)), static_cast<std::uint64_t>(visibleRowCapacity()));
    scheduleVisibleRangeFetch();
}

void PointsViewWidget::fetchVisibleRange()
{
    if (!_model->hasPointData())
        return;

    if (_model->displayRowCount() == 0 || _model->totalColumns() == 0)
    {
        setStatus("The selected point dataset is empty");
        return;
    }

    const auto visibleRows = visibleRowCapacity();
    updateRowScrollBarRange(_model->displayRowCount());
    const auto logicalFirstRow = static_cast<std::uint64_t>(_rowScrollBar->value());
    _model->setWindow(logicalFirstRow, static_cast<std::uint64_t>(visibleRows));
    if (_model->isQueryResult())
        return;

    const auto firstRow    = std::max(0, _table->rowAt(0));
    const auto lastRow     = _table->rowAt(std::max(0, _table->viewport()->height() - 1));
    const auto firstColumn = std::max(0, _table->columnAt(0));
    auto lastColumn = firstColumn;
    const auto viewportWidth = _table->viewport()->width();
    // The viewport can have unused space after the last table column, so columnAt() at its right edge can return -1.
    for (auto column = firstColumn; column < _model->columnCount(); ++column)
    {
        if (_table->isColumnHidden(column))
            continue;

        const auto sectionPosition = _table->horizontalHeader()->sectionViewportPosition(column);
        if (sectionPosition >= viewportWidth)
            break;

        if (sectionPosition + _table->horizontalHeader()->sectionSize(column) > 0)
            lastColumn = column;
    }
    const auto rowCount    = lastRow >= firstRow
        ? static_cast<std::uint64_t>(lastRow - firstRow + 1)
        : 1;
    const auto columnCount = lastColumn >= firstColumn
        ? static_cast<std::uint64_t>(lastColumn - firstColumn + 1)
        : 1;
    if (_fetchInProgress)
    {
        _fetchPending = true;
        return;
    }

    const auto points = _points;
    const auto totalRows = _model->totalRows();
    const auto totalColumns = _model->totalColumns();
    const auto generation = _fetchGeneration;
    const auto absoluteFirstRow = logicalFirstRow + static_cast<std::uint64_t>(firstRow);
    _fetchPending = false;
    _fetchInProgress = true;
    _fetchWatcher->setFuture(QtConcurrent::run([points, totalRows, totalColumns, absoluteFirstRow, rowCount, firstColumn, columnCount, generation] {
        return PointsTableModel::loadRange(points.get(), totalRows, totalColumns, absoluteFirstRow, rowCount, static_cast<std::uint64_t>(firstColumn), columnCount, generation);
    }));
    setStatus("Loading the visible table window...");
}

void PointsViewWidget::rangeFetchFinished()
{
    _fetchInProgress = false;
    auto result = _fetchWatcher->result();

    if (result.generation == _fetchGeneration && _model->hasPointData() && !_model->isQueryResult())
    {
        const auto firstRow = result.firstRow;
        const auto rowCount = result.rowCount;
        const auto firstColumn = result.firstColumn;
        const auto columnCount = result.columnCount;
        const auto callbacks = _model->applyRangeResult(std::move(result));

        if (!_model->getLastError().isEmpty())
        {
            setStatus(QString("Unable to load the requested point-data tiles: %1").arg(_model->getLastError()));
        }
        else
        {
            setStatus(QString("Fetched visible rows %1–%2, columns %3–%4 in %5 access segment(s). Drag/scroll and release to fetch again.")
                .arg(firstRow)
                .arg(firstRow + rowCount - 1)
                .arg(firstColumn)
                .arg(firstColumn + columnCount - 1)
                .arg(callbacks));
        }
    }

    if (_fetchPending)
    {
        _fetchPending = false;
        QTimer::singleShot(0, this, &PointsViewWidget::fetchVisibleRange);
    }
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
    ++_fetchGeneration;
    _fetchPending = false;

    if (rowIds.empty() || columnIds.empty())
    {
        setStatus("The query must contain at least one row and one column");
        return;
    }

    std::vector<float> values(rowIds.size() * columnIds.size(), 0.0f);
    std::uint64_t destinationRow    = 0;
    std::uint64_t destinationColumn = 0;
    QElapsedTimer fetchTimer;
    fetchTimer.start();

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
                        values[static_cast<std::size_t>(targetRow * columnIds.size() + targetColumn)] = pointViewValueAt(view, row, column);
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

    qInfo().noquote() << QStringLiteral("[PointData] Facade fetch: %1 rows × %2 columns in %3 ms")
        .arg(rowIds.size())
        .arg(columnIds.size())
        .arg(fetchTimer.elapsed());

    _liveTableAction.setChecked(false);
    _model->setQueryResult(rowIds, columnIds, values);
    _rowScrollBar->setValue(0);
    updateRowScrollBarRange(_model->displayRowCount());
    _model->setWindow(0, static_cast<std::uint64_t>(visibleRowCapacity()));
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
    const auto isTableResize = (watched == _table || watched == _table->viewport()) && event->type() == QEvent::Resize;
    const auto isTableNavigation = (watched == _rowScrollBar || watched == _table->horizontalScrollBar() || watched == _table->viewport()) &&
        (event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::Wheel || event->type() == QEvent::KeyRelease);

    if ((watched == _table || watched == _table->viewport()) && event->type() == QEvent::Wheel && _model->hasPointData())
    {
        const auto* wheelEvent = static_cast<QWheelEvent*>(event);
        auto delta = wheelEvent->angleDelta().y();
        if (delta == 0 && !wheelEvent->pixelDelta().isNull())
            delta = wheelEvent->pixelDelta().y() * 120 / std::max(1, _table->verticalHeader()->defaultSectionSize());

        if (delta != 0)
        {
            auto rowStep = delta / 120 * 3;
            if (rowStep == 0)
                rowStep = delta > 0 ? 1 : -1;
            _rowScrollBar->setValue(_rowScrollBar->value() - rowStep);
            return true;
        }
    }

    if (isTableResize || isTableNavigation)
        scheduleVisibleRangeFetch();

    return QWidget::eventFilter(watched, event);
}

void PointsViewWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (_model->hasPointData())
        QTimer::singleShot(0, this, &PointsViewWidget::fetchVisibleRange);
}
