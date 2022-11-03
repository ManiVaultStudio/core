#include "WriterPlugin.h"

namespace hdps
{
namespace plugin
{

WriterPlugin::WriterPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _input()
{

}

void WriterPlugin::setInputDataset(Dataset<DatasetImpl>& inputDataset)
{
    _input = inputDataset;
}

WriterPluginFactory::WriterPluginFactory() :
    PluginFactory(Type::WRITER)
{

}

QIcon WriterPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("file-export", color);
}

}

}
