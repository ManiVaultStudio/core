#include "Plugin.h"
#include "actions/WidgetAction.h"
#include "Application.h"

namespace hdps
{

namespace plugin
{

QMap<QString, std::int32_t> hdps::plugin::Plugin::noInstances = QMap<QString, std::int32_t>();

Plugin::Plugin(const PluginFactory* factory) :
    WidgetAction(nullptr),
    _factory(factory),
    _name(getKind() + QUuid::createUuid().toString(QUuid::WithoutBraces)),
    _properties(),
    _eventListener(),
    _guiNameAction(this, "Plugin title", QString("%1 %2").arg(getKind(), QString::number(noInstances[getKind()] + 1)))
{
    noInstances[getKind()]++;

    _guiNameAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu, false);
}

bool Plugin::hasHelp()
{
    return const_cast<PluginFactory*>(_factory)->hasHelp();
}

hdps::gui::TriggerAction& Plugin::getTriggerHelpAction()
{
    return const_cast<PluginFactory*>(_factory)->getTriggerHelpAction();
}

QVariant Plugin::getSetting(const QString& path, const QVariant& defaultValue /*= QVariant()*/) const
{
    return Application::current()->getSetting(QString("%1/%2").arg(getKind(), path), defaultValue);
}

void Plugin::setSetting(const QString& path, const QVariant& value)
{
    Application::current()->setSetting(QString("%1/%2").arg(getKind(), path), value);
}

void Plugin::fromVariantMap(const QVariantMap& variantMap)
{
}

QVariantMap Plugin::toVariantMap() const
{
    QVariantMap plugin {
        { "Kind", _factory->getKind() },
        { "Type", static_cast<std::uint32_t>(_factory->getType()) },
        { "Version", _factory->getVersion() }
    };

    return {
        { "Name", _name },
        { "GuiName", _guiNameAction.getString() },
        { "Plugin", plugin }
    };
}

}
}
