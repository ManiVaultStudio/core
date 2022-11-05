#include "DataHierarchyPlugin.h"

Q_PLUGIN_METADATA(IID "nl.BioVault.DataHierarchyPlugin")

using namespace hdps;

DataHierarchyPlugin::DataHierarchyPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dataHierarchyWidget(nullptr)
{
    const_cast<PluginFactory*>(factory)->setMaximumNumberOfInstances(1);
}

void DataHierarchyPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(&_dataHierarchyWidget);

    getWidget().setLayout(layout);
}

QIcon DataHierarchyPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("sitemap", color);
}

ViewPlugin* DataHierarchyPluginFactory::produce()
{
    return new DataHierarchyPlugin(this);
}
