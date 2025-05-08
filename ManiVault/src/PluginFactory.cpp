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

PluginFactory::PluginFactory(Type type, const QString& title) :
    WidgetAction(nullptr, title),
    _type(type),
    _pluginTriggerAction(this, this, "Plugin trigger", "A plugin trigger action creates a new plugin when triggered", QIcon()),
    _numberOfInstances(0),
    _numberOfInstancesProduced(0),
    _maximumNumberOfInstances(-1),
    _pluginGlobalSettingsGroupAction(nullptr),
    _statusBarAction(nullptr),
    _allowPluginCreationFromStandardGui(true),
    _pluginMetadata(*this),
    _tutorialsDsnsAction(this, "Tutorials DSNs"),
    _projectsDsnsAction(this, "Projects DSNs"),
    _videosDsnsAction(this, "Videos DSNs")
{
    _tutorialsDsnsAction.setIconByName("globe");
    _tutorialsDsnsAction.setToolTip(QString("Data Source Names for %1 tutorials").arg(title));

    _projectsDsnsAction.setIconByName("globe");
    _projectsDsnsAction.setToolTip(QString("Data Source Names for %1 projects").arg(title));

    _videosDsnsAction.setIconByName("globe");
    _videosDsnsAction.setToolTip(QString("Data Source Names for %1 videos").arg(title));
}

QString PluginFactory::getKind() const
{
    return _kind;
}

void PluginFactory::setKind(const QString& kind)
{
    _kind = kind;

    _pluginTriggerAction.setText(_kind);

    getPluginMetadata().getShortcutMap().setTitle(_kind);
}

mv::plugin::Type PluginFactory::getType() const
{
    return _type;
}

void PluginFactory::initialize()
{
    getPluginTriggerAction().initialize();

    getPluginMetadata().getTriggerHelpAction().setText(_kind);
    getPluginMetadata().getTriggerHelpAction().setIcon(icon());
}

const QIcon& PluginFactory::getCategoryIcon() const
{
    return _categoryIcon;
}

void PluginFactory::setCategoryIconByName(const QString& category)
{
    _categoryIcon = StyledIcon(category);
}

QString PluginFactory::getGlobalSettingsPrefix() const
{
    return QString("Plugins/%1/").arg(getKind());
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
    return getPluginMetadata().getGuiName();
}

void PluginFactory::setGuiName(const QString& guiName)
{
    getPluginMetadata().setGuiName(guiName);
}

util::Version& PluginFactory::getVersion()
{
    return getPluginMetadata().getVersion();
}

const util::Version& PluginFactory::getVersion() const
{
    return getPluginMetadata().getVersion();
}

void PluginFactory::setVersion(const util::Version& version)
{
    getPluginMetadata().setVersion(version);
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

PluginMetadata& PluginFactory::getPluginMetadata()
{
    return _pluginMetadata;
}

const PluginMetadata& PluginFactory::getPluginMetadata() const
{
    return const_cast<PluginFactory*>(this)->getPluginMetadata();
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
