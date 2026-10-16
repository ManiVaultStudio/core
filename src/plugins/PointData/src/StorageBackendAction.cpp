// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendAction.h"

#include "PointData.h"

using namespace mv;
using namespace mv::gui;

StorageBackendAction::StorageBackendAction(QObject* parent, const Dataset<Points>& points) :
    HorizontalGroupAction(parent, "Storage backend"),
    _points(points),
    _backendTypeAction(this, "Type", { "Eager", "Tiled", "Mixed", "Unknown" }, "Eager"),
    _configureAction(this, "Configure...")
{
    setShowLabels(true);

    _backendTypeAction.setDefaultWidgetFlags(OptionAction::ComboBox);
    _backendTypeAction.setEnabled(false);
    _backendTypeAction.setToolTip("The storage backend currently used by this points dataset");

    _configureAction.setIconByName("ellipsis");
    _configureAction.setEnabled(false);
    _configureAction.setToolTip("Storage backend configuration is not available yet");

    addAction(&_backendTypeAction);
    addAction(&_configureAction, TriggerAction::Icon);

    if (_points.isValid())
        connect(&_points, &Dataset<Points>::dataChanged, this, &StorageBackendAction::updateBackendType);

    updateBackendType();
}

OptionAction& StorageBackendAction::getBackendTypeAction()
{
    return _backendTypeAction;
}

TriggerAction& StorageBackendAction::getConfigureAction()
{
    return _configureAction;
}

void StorageBackendAction::updateBackendType()
{
    if (!_points.isValid())
    {
        _backendTypeAction.setCurrentText("Unknown");
        return;
    }

    switch (_points->getStorageBackendType())
    {
        case Points::StorageBackendType::Eager:
            _backendTypeAction.setCurrentText("Eager");
            break;

        case Points::StorageBackendType::Tiled:
            _backendTypeAction.setCurrentText("Tiled");
            break;

        case Points::StorageBackendType::Mixed:
            _backendTypeAction.setCurrentText("Mixed");
            break;

        case Points::StorageBackendType::Unknown:
            _backendTypeAction.setCurrentText("Unknown");
            break;
    }
}
