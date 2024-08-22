// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginFactory.h"
#include "widgets/MarkdownDialog.h"

#include "Set.h"

#include "actions/PluginTriggerAction.h"

#include <QDesktopServices>

using namespace mv::gui;
using namespace mv::util;

namespace mv::plugin
{

PluginFactory::PluginFactory(Type type) :
    _kind(),
    _type(type),
    _guiName(),
    _version(),
    _numberOfInstances(0),
    _maximumNumberOfInstances(-1),
    _pluginTriggerAction(this, this, "Plugin trigger", "A plugin trigger action creates a new plugin when triggered", QIcon()),
    _triggerHelpAction(nullptr, "Trigger plugin help"),
    _triggerReadmeAction(nullptr, "Readme"),
    _visitRepositoryAction(nullptr, "Go to repository"),
    _pluginGlobalSettingsGroupAction(nullptr),
    _statusBarAction(nullptr),
    _shortcutMap()
{
    _triggerReadmeAction.setIconByName("book");

    connect(&_triggerReadmeAction, &TriggerAction::triggered, this, [this]() -> void {
        if (!getReadmeMarkdownUrl().isValid())
            return;

        MarkdownDialog markdownDialog(getReadmeMarkdownUrl());

        markdownDialog.setWindowTitle(QString("%1").arg(_kind));
        markdownDialog.exec();
    });

    _visitRepositoryAction.setIconByName("github");

    connect(&_visitRepositoryAction, &TriggerAction::triggered, this, [this]() -> void {
        if (!getRespositoryUrl().isValid())
            return;

        QDesktopServices::openUrl(getRespositoryUrl());
    });
}

QString PluginFactory::getKind() const
{
    return _kind;
}

void PluginFactory::setKind(const QString& kind)
{
    _kind = kind;

    _pluginTriggerAction.setText(_kind);
    _shortcutMap.setTitle(_kind);
}

mv::plugin::Type PluginFactory::getType() const
{
    return _type;
}

void PluginFactory::initialize()
{
    getPluginTriggerAction().initialize();

    _triggerHelpAction.setText(_kind);
    _triggerHelpAction.setIcon(getIcon());
}

QString PluginFactory::getGlobalSettingsPrefix() const
{
    return QString("%1/").arg(getKind());
}

PluginGlobalSettingsGroupAction* PluginFactory::getGlobalSettingsGroupAction() const
{
    return _pluginGlobalSettingsGroupAction;
}

void PluginFactory::setGlobalSettingsGroupAction(PluginGlobalSettingsGroupAction* pluginGlobalSettingsGroupAction)
{
    if (pluginGlobalSettingsGroupAction == _pluginGlobalSettingsGroupAction)
        return;

    _pluginGlobalSettingsGroupAction = pluginGlobalSettingsGroupAction;

    emit pluginGlobalSettingsGroupActionChanged(_pluginGlobalSettingsGroupAction);
}

PluginStatusBarAction* PluginFactory::getStatusBarAction() const
{
    return _statusBarAction;
}

void PluginFactory::setStatusBarAction(PluginStatusBarAction* statusBarAction)
{
    if (statusBarAction == _statusBarAction)
        return;

    _statusBarAction = statusBarAction;

    emit statusBarActionChanged(_statusBarAction);
}

bool PluginFactory::hasHelp()
{
    return false;
}

TriggerAction& PluginFactory::getTriggerHelpAction()
{
    return _triggerHelpAction;
}

TriggerAction& PluginFactory::getTriggerReadmeAction()
{
    return _triggerReadmeAction;
}

TriggerAction& PluginFactory::getVisitRepositoryAction()
{
    return _visitRepositoryAction;
}

QString PluginFactory::getGuiName() const
{
    return _guiName;
}

void PluginFactory::setGuiName(const QString& guiName)
{
    _guiName = guiName;
}

QString PluginFactory::getVersion() const
{
    return _version;
}

void PluginFactory::setVersion(const QString& version)
{
    _version = version;
}

QIcon PluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return QIcon();
}

bool PluginFactory::mayProduce() const
{
    return _numberOfInstances < _maximumNumberOfInstances;
}

mv::gui::PluginTriggerAction& PluginFactory::getPluginTriggerAction()
{
    return _pluginTriggerAction;
}

util::ShortcutMap& PluginFactory::getShortcutMap()
{
    return _shortcutMap;
}

const util::ShortcutMap& PluginFactory::getShortcutMap() const
{
    return _shortcutMap;
}

std::uint32_t PluginFactory::getNumberOfInstances() const
{
    return _numberOfInstances;
}

void PluginFactory::setNumberOfInstances(std::uint32_t numberOfInstances)
{
    if (numberOfInstances == _numberOfInstances)
        return;

    _numberOfInstances = numberOfInstances;

    _pluginTriggerAction.setEnabled(mayProduce());

    emit numberOfInstancesChanged(_numberOfInstances);
}

std::uint32_t PluginFactory::getMaximumNumberOfInstances() const
{
    return _maximumNumberOfInstances;
}

void PluginFactory::setMaximumNumberOfInstances(std::uint32_t maximumNumberOfInstances)
{
    _maximumNumberOfInstances = maximumNumberOfInstances;
}

QStringList PluginFactory::getDatasetTypesAsStringList(const Datasets& datasets)
{
    QStringList datasetTypes;

    for (const auto& dataset : datasets)
        datasetTypes << dataset->getDataType().getTypeString();

    return datasetTypes;
}

bool PluginFactory::areAllDatasetsOfTheSameType(const Datasets& datasets, const DataType& dataType)
{
    for (const auto& dataset : datasets)
        if (dataset->getDataType() != dataType)
            return false;

    return true;
}

std::uint16_t PluginFactory::getNumberOfDatasetsForType(const Datasets& datasets, const DataType& dataType)
{
    std::uint16_t numberOfDatasetsForType = 0;

    for (const auto& dataset : datasets)
        if (dataset->getDataType() == dataType)
            numberOfDatasetsForType++;

    return numberOfDatasetsForType;
}

QUrl PluginFactory::getReadmeMarkdownUrl() const
{
    const auto githubRepositoryUrl = getRespositoryUrl();

    if (!githubRepositoryUrl.isValid())
        return {};

    auto readmeMarkdownUrl = QUrl(QString("https://raw.githubusercontent.com%1/master/README.md").arg(githubRepositoryUrl.path()));

    if (readmeMarkdownUrl.isValid())
        return readmeMarkdownUrl;

    return {};
}

QUrl PluginFactory::getRespositoryUrl() const
{
    return {};
}

}
