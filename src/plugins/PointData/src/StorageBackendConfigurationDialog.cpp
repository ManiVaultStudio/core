// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendConfigurationDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QSpinBox>

#include <limits>

StorageBackendConfigurationDialog::StorageBackendConfigurationDialog(QWidget* parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Configure tiled point-data storage"));

    auto* form = new QFormLayout(this);

    _rowTileSize    = new QSpinBox(this);
    _columnTileSize = new QSpinBox(this);
    _cacheCapacity  = new QSpinBox(this);

    for (auto* spinBox : { _rowTileSize, _columnTileSize, _cacheCapacity })
    {
        spinBox->setRange(1, std::numeric_limits<int>::max());
        spinBox->setMaximumWidth(120);
    }

    _rowTileSize->setValue(256);
    _columnTileSize->setValue(8);
    _cacheCapacity->setValue(8);

    form->addRow(tr("Rows per tile"), _rowTileSize);
    form->addRow(tr("Columns per tile"), _columnTileSize);
    form->addRow(tr("Tile cache capacity"), _cacheCapacity);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    form->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

mv::pointdata::storage::StorageBackendConfiguration StorageBackendConfigurationDialog::getConfiguration() const
{
    mv::pointdata::storage::StorageBackendConfiguration configuration;
    configuration.rowTileSize    = static_cast<std::size_t>(_rowTileSize->value());
    configuration.columnTileSize = static_cast<std::size_t>(_columnTileSize->value());
    configuration.cacheCapacity  = static_cast<std::size_t>(_cacheCapacity->value());
    return configuration;
}
