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
    _type(type),
    _pluginTriggerAction(this, this, "Plugin trigger", "A plugin trigger action creates a new plugin when triggered", QIcon()),
    _numberOfInstances(0),
    _numberOfInstancesProduced(0),
    _maximumNumberOfInstances(-1),
    _triggerHelpAction(nullptr, "Trigger plugin help"),
    _triggerReadmeAction(nullptr, "Readme"),
    _visitRepositoryAction(nullptr, "Go to repository"),
    _pluginGlobalSettingsGroupAction(nullptr),
    _statusBarAction(nullptr),
    _allowPluginCreationFromStandardGui(true)
{
    _triggerReadmeAction.setIconByName("book");

    connect(&_triggerReadmeAction, &TriggerAction::triggered, this, [this]() -> void {
        if (!getReadmeMarkdownUrl().isValid())
            return;

        MarkdownDialog markdownDialog(getReadmeMarkdownUrl());

        markdownDialog.setWindowTitle(QString("%1").arg(_kind));
        markdownDialog.exec();
    });

    _visitRepositoryAction.setToolTip("Browse to the Github repository");
    _visitRepositoryAction.setIcon(Application::getIconFont("FontAwesomeBrands").getIcon("code-branch"));

    connect(&_visitRepositoryAction, &TriggerAction::triggered, this, [this]() -> void {
        if (getRepositoryUrl().isValid())
            QDesktopServices::openUrl(getRepositoryUrl());
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

bool PluginFactory::hasHelp() const
{
    return false;
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

util::ShortcutMap& PluginFactory::getShortcutMap()
{
    return _shortcutMap;
}

const util::ShortcutMap& PluginFactory::getShortcutMap() const
{
    return _shortcutMap;
}

QString PluginFactory::getShortDescription() const
{
    return _shortDescription;
}

void PluginFactory::setShortDescription(const QString& shortDescription)
{
    if (shortDescription == _shortDescription)
        return;

    const auto previousShortDescription = _shortDescription;

    _shortDescription = shortDescription;

    emit shortDescriptionChanged(previousShortDescription, _shortDescription);
}

QString PluginFactory::getLongDescription() const
{
    return _shortDescription;
}

QString PluginFactory::getLongDescriptionMarkdown() const
{
    return _longDescriptionMarkdown;
}

void PluginFactory::setLongDescription(const QString& longDescription)
{
    if (longDescription == _longDescription)
        return;

    const auto previousLongDescription = _longDescription;

    _longDescription = longDescription;

    emit longDescriptionChanged(previousLongDescription, _longDescription);
}

void PluginFactory::setLongDescriptionMarkdown(const QString& longDescriptionMarkdown)
{
    if (longDescriptionMarkdown == _longDescriptionMarkdown)
        return;

    const auto previousLongDescriptionMarkdown = _longDescriptionMarkdown;

    _longDescriptionMarkdown = longDescriptionMarkdown;

    emit longDescriptionMarkdownChanged(previousLongDescriptionMarkdown, _longDescriptionMarkdown);
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
