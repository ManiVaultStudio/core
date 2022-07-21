#include "PluginProducerMetaData.h"

namespace hdps
{
namespace plugin
{

PluginProducerMetaData::PluginProducerMetaData(const Type& pluginType, const QString& pluginKind, const QStringList& datasetTypes, const QString& title, const QString& description) :
	_pluginType(pluginType),
	_pluginKind(pluginKind),
	_datasetTypes(datasetTypes),
	_title(title),
	_description(description)
{

}

}
}