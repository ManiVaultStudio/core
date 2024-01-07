// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginFactory.h"

#include "Set.h"

namespace mv
{

using namespace gui;

namespace plugin
{

PluginFactory::PluginFactory(Type type) :
    _kind(),
    _type(type),
    _guiName(),
    _version(),
    _numberOfInstances(0),
    _maximumNumberOfInstances(-1),
    _pluginTriggerAction(this, this, "Plugin trigger", "A plugin trigger action creates a new plugin when triggered", QIcon()),
    _triggerHelpAction(nullptr, "Trigger plugin help")
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

bool PluginFactory::hasHelp()
{
    return false;
}

mv::gui::TriggerAction& PluginFactory::getTriggerHelpAction()
{
    return _triggerHelpAction;
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

std::uint32_t PluginFactory::getNumberOfInstances() const
{
    return _numberOfInstances;
}

void PluginFactory::setNumberOfInstances(std::uint32_t numberOfInstances)
{
    _numberOfInstances = numberOfInstances;

    _pluginTriggerAction.setEnabled(mayProduce());
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

}

}
