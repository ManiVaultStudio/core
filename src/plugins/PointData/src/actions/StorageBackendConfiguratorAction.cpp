// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendConfiguratorAction.h"

#include "../storage/interface/PointsStorageBackendTypes.h"

namespace
{

QString storageBackendTypeLabel(const QString& backendType)
{
    const auto type = mv::pointdata::storage::getStorageBackendType(backendType);
    if (!type)
        return backendType;

    const auto capabilities = mv::pointdata::storage::getStorageBackendCapabilities(*type);
    QStringList accessModes;

    if (mv::pointdata::storage::hasStorageBackendCapability(capabilities, mv::pointdata::storage::StorageBackendCapability::RandomAccess))
        accessModes.append("read");
    if (mv::pointdata::storage::hasStorageBackendCapability(capabilities, mv::pointdata::storage::StorageBackendCapability::Writable))
        accessModes.append("write");

    return accessModes.isEmpty() ? backendType : QString("%1 (%2)").arg(backendType, accessModes.join(" | "));
}

QStringList storageBackendTypeLabels(const QStringList& backendTypes)
{
    QStringList labels;
    for (const auto& backendType : backendTypes)
        labels.append(storageBackendTypeLabel(backendType));

    return labels;
}

}

StorageBackendConfiguratorAction::StorageBackendConfiguratorAction(QObject* parent, const QStringList& backendTypes, const QString& defaultBackendType, const QString& title) :
    mv::gui::HorizontalGroupAction(parent, title),
    _backendTypes(backendTypes),
    _backendTypeAction(this, "Type", storageBackendTypeLabels(_backendTypes), storageBackendTypeLabel(defaultBackendType)),
    _configureAction(this, "Configure...")
{
    setShowLabels(false);
    setDescription(
        "<p>Choose the physical representation that best matches the dataset and its use. The selector shows whether each option supports reading and writing.</p>"
        "<p><b>Dense</b> keeps the complete matrix materialized in memory. It supports reading and writing and is a good choice when the dataset fits comfortably in RAM and you need frequent, general-purpose access. Its main trade-off is high memory use, which grows with every matrix value.</p>"
        "<p><b>Tiled</b> stores rectangular tiles and loads them on demand. It suits large datasets that should not be fully resident in memory, especially when access is localized to rows, columns, or ranges. Tile size and cache settings affect performance; tile-based access adds bookkeeping and is read-only.</p>"
        "<p><b>Sparse</b> stores only non-zero values in a compressed representation. It is most effective when the matrix contains many zeros; row-compressed (CSR) and column-compressed (CSC) layouts favor row and column access respectively. It can save substantial memory for sparse data, but offers little benefit for dense matrices and is read-only. In HDF5 import, Sparse is available only for supported sparse input.</p>"
        "<p>Changing the type converts the dataset's storage representation. Use Configure to adjust settings without changing the selected type.</p>");

    _backendTypeAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
    _backendTypeAction.setToolTip("Select the storage backend; the suffix shows whether it supports reading and writing.");
    _configureAction.setIconByName("ellipsis");
    _configureAction.setToolTip("Configure the selected storage backend");

    addAction(&_backendTypeAction);
    addAction(&_configureAction, mv::gui::TriggerAction::Icon);

    connect(&_backendTypeAction, &mv::gui::OptionAction::currentIndexChanged, this, [this](const std::int32_t index) {
        if (index >= 0 && index < _backendTypes.size())
            emit backendTypeChanged(_backendTypes[index]);
    });
    connect(&_configureAction, &mv::gui::TriggerAction::triggered, this, &StorageBackendConfiguratorAction::configureRequested);
}

mv::gui::OptionAction& StorageBackendConfiguratorAction::getBackendTypeAction()
{
    return _backendTypeAction;
}

QString StorageBackendConfiguratorAction::getBackendType() const
{
    const auto index = _backendTypeAction.getCurrentIndex();
    return index >= 0 && index < _backendTypes.size() ? _backendTypes[index] : QString();
}

void StorageBackendConfiguratorAction::setBackendType(const QString& backendType)
{
    const auto index = _backendTypes.indexOf(backendType);
    _backendTypeAction.setCurrentIndex(index);
}

mv::gui::TriggerAction& StorageBackendConfiguratorAction::getConfigureAction()
{
    return _configureAction;
}

void StorageBackendConfiguratorAction::setConfigureEnabled(const bool enabled)
{
    _configureAction.setEnabled(enabled);
}
