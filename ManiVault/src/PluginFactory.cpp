// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginFactory.h"

#include "Set.h"

#include "actions/PluginTriggerAction.h"

#include <QDesktopServices>

using namespace mv::gui;
using namespace mv::util;

namespace mv::plugin
{

PluginFactory::PluginFactory(Type type) :
    _type(type),
    _pluginTriggerAction(this, this, "Plugin trigger", "A plugin trigger action creates a new plugin when triggered", QIcon()),
    _numberOfInstances(0),
    _numberOfInstancesProduced(0),
    _maximumNumberOfInstances(-1),
    _pluginGlobalSettingsGroupAction(nullptr),
    _statusBarAction(nullptr),
    _allowPluginCreationFromStandardGui(true),
    _pluginMetaData(*this)
{
}

QString PluginFactory::getKind() const
{
    return _kind;
}

void PluginFactory::setKind(const QString& kind)
{
    _kind = kind;

    _pluginTriggerAction.setText(_kind);

    getPluginMetaData().getShortcutMap().setTitle(_kind);
}

mv::plugin::Type PluginFactory::getType() const
{
    return _type;
}

void PluginFactory::initialize()
{
    getPluginTriggerAction().initialize();

    getPluginMetaData().getTriggerHelpAction().setText(_kind);
    getPluginMetaData().getTriggerHelpAction().setIcon(getIcon());
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

bool PluginFactory::hasHelp() const
{
    return false;
}

QString PluginFactory::getGuiName() const
{
    return getPluginMetaData().getGuiName();
}

void PluginFactory::setGuiName(const QString& guiName)
{
    getPluginMetaData().setGuiName(guiName);
}

QString PluginFactory::getVersion() const
{
    return getPluginMetaData().getVersion();
}

void PluginFactory::setVersion(const QString& version)
{
    getPluginMetaData().setVersion(version);
}

QIcon PluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return {};
}

bool PluginFactory::mayProduce() const
{
    return _numberOfInstances < _maximumNumberOfInstances;
}

mv::gui::PluginTriggerAction& PluginFactory::getPluginTriggerAction()
{
    return _pluginTriggerAction;
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

std::uint32_t PluginFactory::getNumberOfInstancesProduced() const
{
    return _numberOfInstancesProduced;
}

void PluginFactory::setNumberOfInstancesProduced(std::uint32_t numberOfInstancesProduced)
{
    if (numberOfInstancesProduced == _numberOfInstancesProduced)
        return;

    _numberOfInstancesProduced = numberOfInstancesProduced;

    emit numberOfInstancesProducedChanged(_numberOfInstancesProduced);
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

void PluginFactory::viewShortcutMap()
{
    qDebug() << __FUNCTION__ << "not implemented yet...";
}

PluginMetaData& PluginFactory::getPluginMetaData()
{
    return _pluginMetaData;
}

const PluginMetaData& PluginFactory::getPluginMetaData() const
{
    return const_cast<PluginFactory*>(this)->getPluginMetaData();
}

QUrl PluginFactory::getReadmeMarkdownUrl() const
{
    const auto githubRepositoryUrl = getRepositoryUrl();

    if (!githubRepositoryUrl.isValid())
        return {};

    auto readmeMarkdownUrl = QUrl(QString("https://raw.githubusercontent.com%1/%2/README.md").arg(githubRepositoryUrl.path(), getDefaultBranch()));

    if (readmeMarkdownUrl.isValid())
        return readmeMarkdownUrl;

    return {};
}

QUrl PluginFactory::getRepositoryUrl() const
{
    return {};
}

QString PluginFactory::getDefaultBranch() const
{
    return "master";
}

void PluginFactory::setAllowPluginCreationFromStandardGui(bool allowPluginCreationFromStandardGui)
{
    _allowPluginCreationFromStandardGui = allowPluginCreationFromStandardGui;
}

bool PluginFactory::getAllowPluginCreationFromStandardGui() const
{
    return _allowPluginCreationFromStandardGui;
}

}
