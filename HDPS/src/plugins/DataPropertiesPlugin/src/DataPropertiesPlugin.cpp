#include "DataPropertiesPlugin.h"

#include <AbstractDataHierarchyManager.h>

Q_PLUGIN_METADATA(IID "nl.BioVault.DataPropertiesPlugin")

using namespace hdps;

DataPropertiesPlugin::DataPropertiesPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dataPropertiesWidget(nullptr)
{
    const_cast<PluginFactory*>(factory)->setMaximumNumberOfInstances(1);

    getAllowedDockingAreasAction().initialize({ "Left", "Right" }, { "Left", "Right" });
    getPreferredDockingAreaAction().setCurrentText("Bottom");
}

void DataPropertiesPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_dataPropertiesWidget);

    getWidget().setLayout(layout);

    connect(&_core->getDataHierarchyManager(), &AbstractDataHierarchyManager::selectedItemsChanged, this, [this](DataHierarchyItems selectedItems) -> void {
        if (selectedItems.isEmpty())
            getWidget().setWindowTitle("Data properties");
        else
            getWidget().setWindowTitle("Data properties: " + selectedItems.first()->getFullPathName());
    });

    getWidget().setWindowTitle("Data properties");
}

QIcon DataPropertiesPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("sliders-h", color);
}

ViewPlugin* DataPropertiesPluginFactory::produce()
{
    return new DataPropertiesPlugin(this);
}
