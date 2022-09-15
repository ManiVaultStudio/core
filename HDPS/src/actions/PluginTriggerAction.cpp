#include "PluginTriggerAction.h"

#include <QCryptographicHash>

namespace hdps {

namespace gui {

PluginTriggerAction::PluginTriggerAction(QObject* parent, const QString& title, const plugin::Type& pluginType, const QString& pluginKind, const Datasets& datasets) :
    TriggerAction(parent),
    _title(title),
    _sha(QString(QCryptographicHash::hash(QString("%1_%2").arg(pluginKind, title).toUtf8(), QCryptographicHash::Sha1).toHex())),
    _pluginType(pluginType),
    _pluginKind(pluginKind),
    _datasets(datasets),
    _configurationAction(nullptr)
{
    //switch (_pluginType)
    //{
    //    case plugin::Type::ANALYSIS:
    //        _title.insert(0, "Analyze/");
    //	    break;

    //    case plugin::Type::DATA:
    //        _title.insert(0, "Data/");
    //        break;

    //    case plugin::Type::LOADER:
    //        _title.insert(0, "Import/");
    //        break;

    //    case plugin::Type::TRANSFORMATION:
    //        _title.insert(0, "Transform/");
    //        break;

    //    case plugin::Type::VIEW:
    //        _title.insert(0, "View/");
    //        break;

    //    case plugin::Type::WRITER:
    //        _title.insert(0, "Export/");
    //        break;

    //    default:
    //        break;
    //}

    setText(_title.split("/").last());
}

QString PluginTriggerAction::getTitle() const
{
    return _title;
}

QString PluginTriggerAction::getSha() const
{
    return _sha;
}

plugin::Type PluginTriggerAction::getPluginType() const
{
    return _pluginType;
}

QString PluginTriggerAction::getPluginKind() const
{
    return _pluginKind;
}

Datasets PluginTriggerAction::getDatasets() const
{
    return _datasets;
}

void PluginTriggerAction::setDatasets(const Datasets& datasets)
{
    _datasets = datasets;
}

WidgetAction* PluginTriggerAction::getConfigurationAction()
{
    return _configurationAction;
}

void PluginTriggerAction::setConfigurationAction(WidgetAction* configurationAction)
{
    _configurationAction = configurationAction;
}

}
}