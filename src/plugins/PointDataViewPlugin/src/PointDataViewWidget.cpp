// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointDataViewWidget.h"

#include <CoreInterface.h>
#include <PointData.h>

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QAbstractItemView>

#include <algorithm>
#include <cstdint>
#include <vector>

namespace
{
float valueAt(const PointDataAccessView& view, std::uint64_t row, std::uint64_t column)
{
    const auto* bytes = view.rowData(row) + column * view.elementSizeBytes;

    switch (view.elementTypeIndex) {
    case 0: return *reinterpret_cast<const float*>(bytes);
    case 1: return static_cast<float>(*reinterpret_cast<const biovault::bfloat16_t*>(bytes));
    case 2: return static_cast<float>(*reinterpret_cast<const std::int32_t*>(bytes));
    case 3: return static_cast<float>(*reinterpret_cast<const std::uint32_t*>(bytes));
    case 4: return static_cast<float>(*reinterpret_cast<const std::int16_t*>(bytes));
    case 5: return static_cast<float>(*reinterpret_cast<const std::uint16_t*>(bytes));
    case 6: return static_cast<float>(*reinterpret_cast<const std::int8_t*>(bytes));
    case 7: return static_cast<float>(*reinterpret_cast<const std::uint8_t*>(bytes));
    default: return 0.0f;
    }
}
}

PointDataViewWidget::PointDataViewWidget(QWidget* parent) :
    QWidget(parent),
    _datasetLabel(new QLabel("No PointData selected", this)),
    _statusLabel(new QLabel(this)),
    _firstRow(createIndexSpinBox(this)),
    _rowCount(createIndexSpinBox(this)),
    _firstColumn(createIndexSpinBox(this)),
    _columnCount(createIndexSpinBox(this)),
    _rowTileSize(createIndexSpinBox(this)),
    _columnTileSize(createIndexSpinBox(this)),
    _cacheCapacity(createIndexSpinBox(this)),
    _refreshButton(new QPushButton("Read selected range", this)),
    _tileButton(new QPushButton("Enable tiled storage", this)),
    _table(new QTableWidget(this))
{
    _rowCount->setValue(8);
    _columnCount->setValue(8);
    _rowTileSize->setValue(256);
    _columnTileSize->setValue(8);
    _cacheCapacity->setValue(8);

    auto* controls = new QFormLayout();
    controls->addRow("First row", _firstRow);
    controls->addRow("Rows", _rowCount);
    controls->addRow("First column", _firstColumn);
    controls->addRow("Columns", _columnCount);
    controls->addRow("Row tile size", _rowTileSize);
    controls->addRow("Column tile size", _columnTileSize);
    controls->addRow("Tile cache capacity", _cacheCapacity);

    auto* actions = new QVBoxLayout();
    actions->addWidget(_refreshButton);
    actions->addWidget(_tileButton);
    actions->addStretch(1);

    auto* settings = new QGroupBox("Tile access", this);
    auto* settingsLayout = new QHBoxLayout(settings);
    settingsLayout->addLayout(controls, 1);
    settingsLayout->addLayout(actions);

    _table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    _table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(_datasetLabel);
    layout->addWidget(settings);
    layout->addWidget(_table, 1);
    layout->addWidget(_statusLabel);

    connect(_refreshButton, &QPushButton::clicked, this, &PointDataViewWidget::refresh);
    connect(_tileButton, &QPushButton::clicked, this, &PointDataViewWidget::tileSelectedData);
    connect(&mv::dataHierarchy(), &mv::AbstractDataHierarchyManager::selectedItemsChanged, this, &PointDataViewWidget::refresh);

    refresh();
}

QSpinBox* PointDataViewWidget::createIndexSpinBox(QWidget* parent) const
{
    auto* spinBox = new QSpinBox(parent);
    spinBox->setRange(0, 1000000000);
    return spinBox;
}

void PointDataViewWidget::setStatus(const QString& text)
{
    _statusLabel->setText(text);
}

void PointDataViewWidget::refresh()
{
    _table->clear();
    _table->setRowCount(0);
    _table->setColumnCount(0);

    const auto selectedItems = mv::dataHierarchy().getSelectedItems();
    if (selectedItems.size() != 1) {
        _datasetLabel->setText("Select exactly one Points dataset");
        setStatus("No point data selected");
        return;
    }

    auto points = selectedItems.first()->getDataset<Points>();
    if (!points.isValid()) {
        _datasetLabel->setText("Selected dataset is not Points");
        setStatus("No point data selected");
        return;
    }

    auto* rawData = points->getPointData();
    const auto rows = rawData->getNumPoints();
    const auto columns = rawData->getNumDimensions();
    _datasetLabel->setText(QString("%1 — %2 points × %3 dimensions (%4)")
        .arg(points->getGuiName()).arg(rows).arg(columns)
        .arg(rawData->isTiled() ? "tiled" : "eager"));

    const auto firstRow = std::min<std::uint64_t>(_firstRow->value(), rows);
    const auto firstColumn = std::min<std::uint64_t>(_firstColumn->value(), columns);
    const auto rowCount = std::min<std::uint64_t>(_rowCount->value(), rows - firstRow);
    const auto columnCount = std::min<std::uint64_t>(_columnCount->value(), columns - firstColumn);
    _table->setRowCount(static_cast<int>(rowCount));
    _table->setColumnCount(static_cast<int>(columnCount));

    std::size_t callbackCount = 0;
    rawData->visitRange(firstRow, rowCount, firstColumn, columnCount, [this, &callbackCount](const PointDataAccessView& view) {
        ++callbackCount;
        for (std::uint64_t row = 0; row < view.rows; ++row)
            for (std::uint64_t column = 0; column < view.columns; ++column)
                _table->setItem(static_cast<int>(row), static_cast<int>(column),
                    new QTableWidgetItem(QString::number(valueAt(view, row, column), 'g', 7)));
    });

    setStatus(QString("Read %1 × %2 values in %3 access segment(s); cached tiles: %4")
        .arg(rowCount).arg(columnCount).arg(callbackCount).arg(rawData->getTiledCacheSize()));
}

void PointDataViewWidget::tileSelectedData()
{
    const auto selectedItems = mv::dataHierarchy().getSelectedItems();
    if (selectedItems.size() != 1)
        return;

    auto points = selectedItems.first()->getDataset<Points>();
    if (!points.isValid())
        return;

    auto* rawData = points->getPointData();
    if (!rawData->isTiled())
        rawData->makeTiled(static_cast<std::size_t>(_rowTileSize->value()), static_cast<std::size_t>(_columnTileSize->value()));
    rawData->setTiledCacheCapacity(static_cast<std::size_t>(_cacheCapacity->value()));
    refresh();
}
