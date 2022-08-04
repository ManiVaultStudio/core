#include "PluginFactory.h"
#include "Set.h"
#include "actions/PluginTriggerAction.h"

namespace hdps
{
namespace plugin
{

QStringList PluginFactory::getDatasetTypesAsStringList(const Datasets& datasets)
{
    QStringList datasetTypes;

    for (auto dataset : datasets)
        datasetTypes << dataset->getDataType().getTypeString();

    return datasetTypes;
}

bool PluginFactory::areAllDatasetsOfTheSameType(const Datasets& datasets, const QString& datasetType)
{
    for (auto dataset : datasets)
        if (dataset->getDataType().getTypeString() != datasetType)
            return false;

    return true;
}

std::uint16_t PluginFactory::getNumberOfDatasetsForType(const Datasets& datasets, const QString& datasetType)
{
    std::uint16_t numberOfDatasetsForType = 0;

    for (auto dataset : datasets)
        if (dataset->getDataType().getTypeString() == datasetType)
            numberOfDatasetsForType++;

    return numberOfDatasetsForType;
}

PluginTriggerAction* PluginFactory::createPluginTriggerAction(const QString& title, const QString& description, const Datasets& datasets) const
{
    auto action = new PluginTriggerAction(nullptr, title, _type, _kind, datasets);

    action->setToolTip(description);
    action->setIcon(getIcon());

    return action;
}

PluginTriggerAction* PluginFactory::createPluginTriggerAction(const QString& title, const QString& description, const Datasets& datasets, const QString& iconName) const
{
    auto action = new PluginTriggerAction(nullptr, title, _type, _kind, datasets);

    action->setToolTip(description);
    action->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon(iconName));

    return action;
}

PluginTriggerAction* PluginFactory::createPluginTriggerAction(const QString& title, const QString& description, const Datasets& datasets, const QIcon& icon) const
{
    auto action = new PluginTriggerAction(nullptr, title, _type, _kind, datasets);

    action->setToolTip(description);
    action->setIcon(icon);

    return action;
}

}

}