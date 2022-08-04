#include "PluginTriggerAction.h"

namespace hdps {

namespace gui {

PluginTriggerAction::PluginTriggerAction(QObject* parent, const QString& title, const plugin::Type& pluginType, const QString& pluginKind, const Datasets& datasets) :
    TriggerAction(parent),
    _pluginType(pluginType),
    _pluginKind(pluginKind),
    _datasets(datasets),
    _configurationAction(nullptr)
{
    setText(title);
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