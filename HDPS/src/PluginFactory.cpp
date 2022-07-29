#include "PluginFactory.h"
#include "Set.h"
#include "actions/TriggerAction.h"

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

TriggerAction* PluginFactory::createProducerAction(const QString& title, const QString& description) const
{
    auto action = new TriggerAction(nullptr, title);

    action->setToolTip(description);
    action->setIcon(getIcon());

    return action;
}

TriggerAction* PluginFactory::createProducerAction(const QString& title, const QString& description, const QString& iconName) const
{
    auto action = new TriggerAction(nullptr, title);

    action->setToolTip(description);
    action->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon(iconName));

    return action;
}

TriggerAction* PluginFactory::createProducerAction(const QString& title, const QString& description, const QIcon& icon) const
{
    auto action = new TriggerAction(nullptr, title);

    action->setToolTip(description);
    action->setIcon(icon);

    return action;
}

}

}