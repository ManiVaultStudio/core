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
    _fetchTimer(new QTimer(this))
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
    if (_model->hasPointData())
        _fetchTimer->start();
}

void PointsViewWidget::fetchVisibleRange()
{
    if (!_model->hasPointData())
        return;

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
