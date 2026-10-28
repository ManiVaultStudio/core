// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendConfiguratorAction.h"

StorageBackendConfiguratorAction::StorageBackendConfiguratorAction(QObject* parent, const QStringList& backendTypes, const QString& defaultBackendType, const QString& title) :
    mv::gui::HorizontalGroupAction(parent, title),
    _backendTypeAction(this, "Type", backendTypes, defaultBackendType),
    _configureAction(this, "Configure...")
{
    setShowLabels(false);

    _backendTypeAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
    _backendTypeAction.setToolTip("Select the storage backend; the suffix shows whether it supports reading and writing.");

    _configureAction.setIconByName("ellipsis");
    _configureAction.setToolTip("Configure the selected storage backend");

    addAction(&_backendTypeAction);
    addAction(&_configureAction, mv::gui::TriggerAction::Icon);

    connect(&_backendTypeAction, &mv::gui::OptionAction::currentTextChanged, this, &StorageBackendConfiguratorAction::backendTypeChanged);
    connect(&_configureAction, &mv::gui::TriggerAction::triggered, this, &StorageBackendConfiguratorAction::configureRequested);
}

mv::gui::OptionAction& StorageBackendConfiguratorAction::getBackendTypeAction()
{
    return _backendTypeAction;
}

mv::gui::TriggerAction& StorageBackendConfiguratorAction::getConfigureAction()
{
    return _configureAction;
}

void StorageBackendConfiguratorAction::setConfigureEnabled(const bool enabled)
{
    _configureAction.setEnabled(enabled);
}
