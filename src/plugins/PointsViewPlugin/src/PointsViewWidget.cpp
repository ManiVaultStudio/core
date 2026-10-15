// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointsViewWidget.h"

#include <CoreInterface.h>
#include <PointData.h>
#include <Task.h>

#include <QAbstractItemView>
#include <QEvent>
#include <QElapsedTimer>
#include <QFutureWatcher>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QSpinBox>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>
#include <QtConcurrent>

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

PointsViewWidget::PointsViewWidget(QWidget* parent, mv::Task* progressTask) :
    QWidget(parent),
    _datasetLabel(new QLabel("No Points dataset selected", this)),
    _inspectorLabel(new QLabel(this)),
    _statusLabel(new QLabel(this)),
    _rowTileSize(createIndexSpinBox(this)),
    _columnTileSize(createIndexSpinBox(this)),
    _cacheCapacity(createIndexSpinBox(this)),
    _tileButton(new QPushButton("Enable tiled storage", this)),
    _table(new QTableView(this)),
    _accessLog(new QPlainTextEdit(this)),
    _model(new PointsTableModel(this)),
    _progressTask(progressTask),
    _tileWatcher(new QFutureWatcher<void>(this)),
    _fetchTimer(new QTimer(this)),
    _rowFilterAction(this, "Rows"),
    _columnFilterAction(this, "Columns"),
    _applyFiltersAction(this, "Apply filters"),
    _liveTableAction(this, "Show live table", true)
{
    _rowTileSize->setValue(256);
    _columnTileSize->setValue(8);
    _cacheCapacity->setValue(8);

    auto* controls = new QFormLayout();
    controls->addRow("Row tile size", _rowTileSize);
    controls->addRow("Column tile size", _columnTileSize);
    controls->addRow("Tile cache capacity", _cacheCapacity);

    auto* actions = new QVBoxLayout();
    actions->addWidget(_tileButton);
    actions->addStretch(1);

    auto* settings = new QGroupBox("Lazy table access", this);
    auto* settingsLayout = new QHBoxLayout(settings);
    settingsLayout->addLayout(controls, 1);
    settingsLayout->addLayout(actions);

    _rowFilterAction.setPlaceHolderString("Empty = all rows; e.g. 1, 3, 10-20");
    _columnFilterAction.setPlaceHolderString("Empty = all columns; e.g. 0-4, 8");

    auto* queryGroup = new QGroupBox("Point-data filters", this);
    auto* queryLayout = new QHBoxLayout(queryGroup);
    queryLayout->addWidget(_rowFilterAction.createWidget(queryGroup));
    queryLayout->addWidget(_columnFilterAction.createWidget(queryGroup));
    queryLayout->addWidget(_applyFiltersAction.createWidget(queryGroup));
    queryLayout->addWidget(_liveTableAction.createWidget(queryGroup, mv::gui::ToggleAction::CheckBox));

    _table->setModel(_model);
    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->setAlternatingRowColors(true);
    _table->setWordWrap(false);
    _table->verticalHeader()->setDefaultSectionSize(22);
    _table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    _table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    _accessLog->setReadOnly(true);
    _accessLog->setMaximumBlockCount(12);
    _accessLog->setMaximumHeight(110);
    _accessLog->setPlaceholderText("Visible-range requests will appear here");

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(_datasetLabel);
    layout->addWidget(settings);
    layout->addWidget(queryGroup);
    layout->addWidget(_table, 1);
    layout->addWidget(_inspectorLabel);
    layout->addWidget(_accessLog);
    layout->addWidget(_statusLabel);

    _table->verticalScrollBar()->installEventFilter(this);
    _table->horizontalScrollBar()->installEventFilter(this);
    _table->viewport()->installEventFilter(this);
    _fetchTimer->setSingleShot(true);
    _fetchTimer->setInterval(100);

    connect(_tileButton, &QPushButton::clicked, this, &PointsViewWidget::tileSelectedData);
    connect(&_applyFiltersAction, &mv::gui::TriggerAction::triggered, this, &PointsViewWidget::applyFilters);
    connect(&_liveTableAction, &mv::gui::ToggleAction::toggled, this, &PointsViewWidget::showLiveTable);
    connect(&mv::dataHierarchy(), &mv::AbstractDataHierarchyManager::selectedItemsChanged, this, &PointsViewWidget::refreshSelection);
    connect(_fetchTimer, &QTimer::timeout, this, &PointsViewWidget::fetchVisibleRange);
    connect(_table->verticalScrollBar(), &QScrollBar::valueChanged, this, &PointsViewWidget::scheduleVisibleRangeFetch);
    connect(_table->horizontalScrollBar(), &QScrollBar::valueChanged, this, &PointsViewWidget::scheduleVisibleRangeFetch);

    refreshSelection();
}

QSpinBox* PointsViewWidget::createIndexSpinBox(QWidget* parent) const
{
    auto* spinBox = new QSpinBox(parent);
    spinBox->setRange(1, 1000000000);
    return spinBox;
}

void PointsViewWidget::setStatus(const QString& text)
{
    _statusLabel->setText(text);
}

void PointsViewWidget::updateInspector(const Points* points)
{
    if (!points) {
        _inspectorLabel->setText("Storage inspector: no point data");
        return;
    }

    if (!points->isTiled()) {
        _inspectorLabel->setText(QString("Storage inspector: eager | type: %1 | tiled cache unavailable")
            .arg(points->getElementTypeName()));
        return;
    }

    const auto estimate = points->getTiledMemoryEstimate();
    _inspectorLabel->setText(QString("Storage inspector: tiled | type: %1 | tile: %2 × %3 | raw: %4 MiB | cache estimate: %5 MiB | cache: %6/%7 | hits: %8 | misses: %9 | evictions: %10")
        .arg(points->getElementTypeName())
        .arg(points->getTiledRowTileSize())
        .arg(points->getTiledColumnTileSize())
        .arg(static_cast<double>(estimate.rawDataBytes) / (1024.0 * 1024.0), 0, 'f', 1)
        .arg(static_cast<double>(estimate.cachedTileBytes) / (1024.0 * 1024.0), 0, 'f', 1)
        .arg(points->getTiledCacheSize())
        .arg(points->getTiledCacheCapacity())
        .arg(points->getTiledCacheHits())
        .arg(points->getTiledCacheMisses())
        .arg(points->getTiledCacheEvictions()));
}

void PointsViewWidget::refreshSelection()
{
    _model->clear();

    const auto selectedItems = mv::dataHierarchy().getSelectedItems();
    if (selectedItems.size() != 1) {
        _datasetLabel->setText("Select exactly one Points dataset");
        _accessLog->clear();
        updateInspector(nullptr);
        setStatus("Scroll after selecting a dataset to fetch visible values");
        return;
    }

    auto points = selectedItems.first()->getDataset<Points>();
    if (!points.isValid()) {
        _datasetLabel->setText("Selected dataset is not Points");
        _accessLog->clear();
        updateInspector(nullptr);
        setStatus("No point data selected");
        return;
    }

    _model->setPoints(points.get(), points->getNumPoints(), points->getNumDimensions());
    _datasetLabel->setText(QString("%1 — %2 points × %3 dimensions (%4)")
        .arg(points->getGuiName())
        .arg(points->getNumPoints())
        .arg(points->getNumDimensions())
        .arg(points->isTiled() ? "tiled" : "eager"));
    _accessLog->clear();
    updateInspector(points.get());
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

    const auto firstRow = std::max(0, _table->rowAt(0));
    const auto lastRow = _table->rowAt(std::max(0, _table->viewport()->height() - 1));
    const auto firstColumn = std::max(0, _table->columnAt(0));
    const auto lastColumn = _table->columnAt(std::max(0, _table->viewport()->width() - 1));
    const auto rowCount = lastRow >= firstRow
        ? static_cast<std::uint64_t>(lastRow - firstRow + 1)
        : 1;
    const auto columnCount = lastColumn >= firstColumn
        ? static_cast<std::uint64_t>(lastColumn - firstColumn + 1)
        : 1;
    QElapsedTimer timer;
    timer.start();
    const auto callbacks = _model->fetchRange(static_cast<std::uint64_t>(firstRow), rowCount, static_cast<std::uint64_t>(firstColumn), columnCount);
    const auto elapsedMilliseconds = timer.elapsed();

    if (!_model->getLastError().isEmpty())
    {
        setStatus(QString("Unable to load the requested point-data tiles: %1").arg(_model->getLastError()));
        return;
    }

    const auto selectedItems = mv::dataHierarchy().getSelectedItems();
    if (selectedItems.size() != 1)
        return;
    auto points = selectedItems.first()->getDataset<Points>();
    if (!points.isValid())
        return;

    _accessLog->appendPlainText(QString("rows %1–%2, columns %3–%4 | %5 segment(s) | %6 ms")
        .arg(firstRow)
        .arg(firstRow + static_cast<int>(rowCount) - 1)
        .arg(firstColumn)
        .arg(firstColumn + static_cast<int>(columnCount) - 1)
        .arg(callbacks)
        .arg(elapsedMilliseconds));
    updateInspector(points.get());
    setStatus(QString("Fetched visible rows %1–%2, columns %3–%4 in %5 access segment(s); cached tiles: %6. Drag/scroll and release to fetch again.")
        .arg(firstRow)
        .arg(firstRow + static_cast<int>(rowCount) - 1)
        .arg(firstColumn)
        .arg(firstColumn + static_cast<int>(columnCount) - 1)
        .arg(callbacks)
        .arg(points->getTiledCacheSize()));
}

std::vector<std::uint64_t> PointsViewWidget::parseFilter(const QString& filter, const std::uint64_t maximum, QString& error)
{
    std::vector<std::uint64_t> values;
    std::unordered_set<std::uint64_t> seen;

    for (const auto& part : filter.split(',', Qt::SkipEmptyParts))
    {
        const auto expression = part.trimmed();
        const auto rangeParts = expression.split('-', Qt::KeepEmptyParts);

        if (rangeParts.size() > 2 || rangeParts.contains(QString()))
        {
            error = QString("Invalid filter expression: %1").arg(expression);
            return {};
        }

        bool firstOk = false;
        const auto first = rangeParts.first().trimmed().toULongLong(&firstOk);
        const auto last = rangeParts.size() == 1 ? first : rangeParts.last().trimmed().toULongLong(&firstOk);

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
    std::uint64_t destinationRow = 0;
    std::uint64_t destinationColumn = 0;

    try
    {
        request([&](const PointDataAccessView& view) {
            for (std::uint64_t row = 0; row < view.rows; ++row)
            {
                for (std::uint64_t column = 0; column < view.columns; ++column)
                {
                    const auto targetRow = destinationRow + row;
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
    setStatus(QString("Displayed %1 row(s) × %2 column(s) from a facade query. Click Show live table to resume viewport loading.").arg(rowIds.size()).arg(columnIds.size()));
}

void PointsViewWidget::applyFilters()
{
    const auto selectedItems = mv::dataHierarchy().getSelectedItems();
    if (selectedItems.size() != 1)
        return;

    const auto points = selectedItems.first()->getDataset<Points>();
    if (!points.isValid())
        return;

    QString error;
    const auto maximumRow = points->getNumPoints() > 0 ? points->getNumPoints() - 1 : 0;
    const auto maximumColumn = points->getNumDimensions() > 0 ? points->getNumDimensions() - 1 : 0;
    auto rowIds = parseFilter(_rowFilterAction.getString(), maximumRow, error);
    if (!error.isEmpty())
    {
        setStatus(error);
        return;
    }

    auto columnIds = parseFilter(_columnFilterAction.getString(), maximumColumn, error);
    if (!error.isEmpty())
    {
        setStatus(error);
        return;
    }

    if (_rowFilterAction.getString().trimmed().isEmpty())
    {
        rowIds.resize(static_cast<std::size_t>(points->getNumPoints()));
        std::iota(rowIds.begin(), rowIds.end(), 0);
    }

    if (_columnFilterAction.getString().trimmed().isEmpty())
    {
        columnIds.resize(static_cast<std::size_t>(points->getNumDimensions()));
        std::iota(columnIds.begin(), columnIds.end(), 0);
    }

    const auto contiguousRows = std::adjacent_find(rowIds.begin(), rowIds.end(), [](const auto first, const auto second) {
        return second != first + 1;
    }) == rowIds.end();
    const auto contiguousColumns = std::adjacent_find(columnIds.begin(), columnIds.end(), [](const auto first, const auto second) {
        return second != first + 1;
    }) == columnIds.end();

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
    if (live)
        refreshSelection();
}

void PointsViewWidget::tileSelectedData()
{
    const auto selectedItems = mv::dataHierarchy().getSelectedItems();
    if (selectedItems.size() != 1)
        return;

    auto points = selectedItems.first()->getDataset<Points>();
    if (!points.isValid())
        return;

    points->setTiledCacheCapacity(static_cast<std::size_t>(_cacheCapacity->value()));

    if (points->isTiled()) {
        emit tiledStorageEnabled(points->getGuiName());
        refreshSelection();
        return;
    }

    const auto rowTileSize = static_cast<std::size_t>(_rowTileSize->value());
    const auto columnTileSize = static_cast<std::size_t>(_columnTileSize->value());
    const auto cacheCapacity = static_cast<std::size_t>(_cacheCapacity->value());
    const auto datasetName = points->getGuiName();
    const auto progressTask = _progressTask;

    _tileButton->setEnabled(false);
    if (progressTask) {
        progressTask->resetProgress();
        progressTask->setRunning();
        progressTask->setProgressDescription("Creating point-data tiles");
    }

    connect(_tileWatcher, &QFutureWatcher<void>::finished, this, [this, datasetName, progressTask, cacheCapacity]() {
        _tileButton->setEnabled(true);
        if (progressTask) {
            progressTask->setProgress(1.0f, "Point-data tiling complete");
            progressTask->setFinished();
        }

        const auto selectedItems = mv::dataHierarchy().getSelectedItems();
        if (selectedItems.size() == 1) {
            auto points = selectedItems.first()->getDataset<Points>();
            if (points.isValid())
                points->setTiledCacheCapacity(cacheCapacity);
        }

        emit tiledStorageEnabled(datasetName);
        refreshSelection();
    });

    _tileWatcher->setFuture(QtConcurrent::run([points, rowTileSize, columnTileSize, progressTask]() mutable {
        points->makeTiled(rowTileSize, columnTileSize, [progressTask](const float progress) {
            if (progressTask)
                progressTask->setProgress(progress, "Creating point-data tiles");
        });
    }));
}

bool PointsViewWidget::eventFilter(QObject* watched, QEvent* event)
{
    if ((watched == _table->verticalScrollBar() || watched == _table->horizontalScrollBar() || watched == _table->viewport()) &&
        (event->type() == QEvent::MouseButtonRelease || event->type() == QEvent::Wheel || event->type() == QEvent::KeyRelease))
        scheduleVisibleRangeFetch();

    return QWidget::eventFilter(watched, event);
}
