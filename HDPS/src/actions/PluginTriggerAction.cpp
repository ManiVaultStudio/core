#include "PluginTriggerAction.h"

#include <QCryptographicHash>

namespace hdps {

namespace gui {

PluginTriggerAction::PluginTriggerAction(QObject* parent, const QString& title, const plugin::Type& pluginType, const QString& pluginKind, const Datasets& datasets) :
    TriggerAction(parent),
    _hash(QString(QCryptographicHash::hash(QString("%1_%2").arg(pluginKind, title).toUtf8(), QCryptographicHash::Sha1).toHex())),
    _pluginType(pluginType),
    _pluginKind(pluginKind),
    _datasets(datasets),
    _configurationAction(nullptr)
{
    setText(title);
}

QString PluginTriggerAction::getHash() const
{
    return _hash;
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

WidgetAction* PluginTriggerAction::getConfigurationAction()
{
    return _configurationAction;
}

void PluginTriggerAction::setConfigurationAction(WidgetAction* configurationAction)
{
    Q_ASSERT(configurationAction != nullptr);

    _configurationAction = configurationAction;
}

}
}