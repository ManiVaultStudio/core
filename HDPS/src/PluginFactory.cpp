#include "PluginFactory.h"
#include "Set.h"
#include "actions/PluginTriggerAction.h"

namespace hdps
{
namespace plugin
{

PluginFactory::PluginFactory(Type type) :
    _kind(),
    _type(type),
    _guiName(),
    _version(),
    _numberOfInstances(0),
    _maximumNumberOfInstances(-1),
    _producePluginTriggerAction(this, "")
{
}

QString PluginFactory::getKind() const
{
    return _kind;
}

void PluginFactory::setKind(const QString& kind)
{
    _kind = kind;
}

hdps::plugin::Type PluginFactory::getType() const
{
    return _type;
}

QString PluginFactory::getGuiName() const
{
    return _guiName;
}

void PluginFactory::setGuiName(const QString& guiName)
{
    _guiName = guiName;

    _producePluginTriggerAction.setText(_guiName);
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

hdps::gui::TriggerAction& PluginFactory::getProducePluginTriggerAction()
{
    return _producePluginTriggerAction;
}

std::uint32_t PluginFactory::getNumberOfInstances() const
{
    return _numberOfInstances;
}

void PluginFactory::setNumberOfInstances(std::uint32_t numberOfInstances)
{
    _numberOfInstances = numberOfInstances;

    _producePluginTriggerAction.setEnabled(mayProduce());
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

    for (auto dataset : datasets)
        datasetTypes << dataset->getDataType().getTypeString();

    return datasetTypes;
}

bool PluginFactory::areAllDatasetsOfTheSameType(const Datasets& datasets, const DataType& dataType)
{
    for (auto dataset : datasets)
        if (dataset->getDataType() != dataType)
            return false;

    return true;
}

std::uint16_t PluginFactory::getNumberOfDatasetsForType(const Datasets& datasets, const DataType& dataType)
{
    std::uint16_t numberOfDatasetsForType = 0;

    for (auto dataset : datasets)
        if (dataset->getDataType() == dataType)
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