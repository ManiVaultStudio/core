#include "TransformationPlugin.h"

namespace hdps
{

namespace plugin
{

TransformationPlugin::TransformationPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _inputDatasets()
{

}

hdps::Datasets TransformationPlugin::getInputDatasets() const
{
    return _inputDatasets;
}

void TransformationPlugin::setInputDatasets(const Datasets& inputDatasets)
{
    _inputDatasets = inputDatasets;
}

TransformationPluginFactory::TransformationPluginFactory() :
    PluginFactory(Type::TRANSFORMATION)
{

}

QIcon TransformationPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("random", color);
}

}
}
