// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginTriggerAction.h"

#include "CoreInterface.h"
#include "PluginFactory.h"

#include <QCryptographicHash>

namespace mv::gui {

PluginTriggerAction::PluginTriggerAction(QObject* parent, const plugin::PluginFactory* pluginFactory, const QString& title, const QString& tooltip, const QIcon& icon) :
    TriggerAction(parent, title),
    _pluginFactory(pluginFactory),
    _menuLocation(),
    _sha(),
    _datasets(),
    _configurationAction(nullptr),
    _requestPluginCallback()
{
    setText(title);
    setToolTip(tooltip);
    setIcon(icon);

    connect(this, &TriggerAction::triggered, this, &PluginTriggerAction::requestPlugin);

    setRequestPluginCallback([this](PluginTriggerAction& pluginTriggerAction) -> void {
        plugins().requestPlugin(_pluginFactory->getKind());
    });
}

PluginTriggerAction::PluginTriggerAction(QObject* parent, const plugin::PluginFactory* pluginFactory, const QString& title, const QString& tooltip, const QIcon& icon, RequestPluginCallback requestPluginCallback) :
    TriggerAction(parent, title),
    _pluginFactory(pluginFactory),
    _menuLocation(),
    _sha(),
    _configurationAction(nullptr),
    _requestPluginCallback(requestPluginCallback)
{
    setText(title);
    setToolTip(tooltip);
    setIcon(icon);

    connect(this, &TriggerAction::triggered, this, &PluginTriggerAction::requestPlugin);
}

PluginTriggerAction::PluginTriggerAction(const PluginTriggerAction& pluginTriggerAction, const QString& title) :
    TriggerAction(pluginTriggerAction.parent(), title),
    _pluginFactory(pluginTriggerAction.getPluginFactory()),
    _menuLocation(),
    _sha(),
    _configurationAction(nullptr),
    _requestPluginCallback()
{
    setText(pluginTriggerAction.text());
    setToolTip(pluginTriggerAction.toolTip());
    setIcon(pluginTriggerAction.icon());

    connect(this, &TriggerAction::triggered, this, &PluginTriggerAction::requestPlugin);
}

const mv::plugin::PluginFactory* PluginTriggerAction::getPluginFactory() const
{
    return _pluginFactory;
}

QString PluginTriggerAction::getMenuLocation() const
{
    return _menuLocation;
}

void PluginTriggerAction::setMenuLocation(const QString& menuLocation)
{
    _menuLocation = menuLocation;
}

QString PluginTriggerAction::getSha() const
{
    return _sha;
}

 mv::Datasets PluginTriggerAction::getDatasets() const
{
    return _datasets;
}

WidgetAction* PluginTriggerAction::getConfigurationAction()
{
    return _configurationAction;
}

void PluginTriggerAction::setConfigurationAction(WidgetAction* configurationAction)
{
    _configurationAction = configurationAction;
}

void PluginTriggerAction::setDatasets(const mv::Datasets& datasets)
{
    _datasets = datasets;
}

void PluginTriggerAction::initialize()
{
    _sha = QString(QCryptographicHash::hash(QString("%1_%2").arg(_pluginFactory->getKind(), getLocation()).toUtf8(), QCryptographicHash::Sha1).toHex());

    setIcon(_pluginFactory->icon());
}

void PluginTriggerAction::setText(const QString& text)
{
    QAction::setText(text);

    switch (_pluginFactory->getType())
    {
        case plugin::Type::ANALYSIS:
            _menuLocation = "Analyze";
            break;

        case plugin::Type::DATA:
            _menuLocation = "Data";
            break;

        case plugin::Type::LOADER:
            _menuLocation = "Import";
            break;

        case plugin::Type::TRANSFORMATION:
            _menuLocation = "Transform";
            break;

        case plugin::Type::VIEW:
            _menuLocation = "View";
            break;

        case plugin::Type::WRITER:
            _menuLocation = "Export";
            break;

        default:
            break;
    }

    _menuLocation.append("/");
    _menuLocation.append(this->text());
}

void PluginTriggerAction::setRequestPluginCallback(RequestPluginCallback requestPluginCallback)
{
    _requestPluginCallback = requestPluginCallback;
}

void PluginTriggerAction::requestPlugin()
{
    if (_requestPluginCallback)
        _requestPluginCallback(*this);
}

}
