#include "DataPropertiesPlugin.h"

Q_PLUGIN_METADATA(IID "nl.BioVault.DataPropertiesPlugin")

using namespace hdps;

DataPropertiesPlugin::DataPropertiesPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dataPropertiesWidget(nullptr)
{
    const_cast<PluginFactory*>(factory)->setMaximumNumberOfInstances(1);

    /*
    * connect(&_core->getDataHierarchyManager(), &DataHierarchyManager::itemAdded, this, &MainWindow::updateCentralWidget);
    connect(&_core->getDataHierarchyManager(), &DataHierarchyManager::selectedItemsChanged, this, [this](DataHierarchyItems selectedItems) -> void {
        if (selectedItems.isEmpty())
            _dataPropertiesDockWidget->setWindowTitle("Data properties");
        else
            _dataPropertiesDockWidget->setWindowTitle("Data properties: " + selectedItems.first()->getFullPathName());
    });

    _dataPropertiesDockWidget->setWindowTitle("Data properties");
    */
}

void DataPropertiesPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(&_dataPropertiesWidget);

    getWidget().setLayout(layout);
}

QIcon DataPropertiesPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("sliders-h", color);
}

ViewPlugin* DataPropertiesPluginFactory::produce()
{
    return new DataPropertiesPlugin(this);
}
