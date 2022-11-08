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

    _eventListener.setEventCore(Application::core());

    _guiNameAction.setConnectionPermissionsToNone();
    _guiNameAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu, false);
    _guiNameAction.setPlaceHolderString("Enter plugin name here...");
}

Plugin::~Plugin()
{
}

QString Plugin::getName() const
{
    return _name;
}

QString Plugin::getGuiName() const
{
    return _guiNameAction.getString();
}

QIcon Plugin::getIcon() const
{
    return _factory->getIcon();
}

QString Plugin::getKind() const
{
    return _factory->getKind();
}

hdps::plugin::Type Plugin::getType() const
{
    return _factory->getType();
}

QString Plugin::getVersion() const
{
    return "No Version";
}

bool Plugin::hasHelp()
{
    return const_cast<PluginFactory*>(_factory)->hasHelp();
}

hdps::gui::TriggerAction& Plugin::getTriggerHelpAction()
{
    return const_cast<PluginFactory*>(_factory)->getTriggerHelpAction();
}

QVariant Plugin::getProperty(const QString& name, const QVariant& defaultValue /*= QVariant()*/) const
{
    if (!hasProperty(name))
        return defaultValue;

    return _properties[name];
}

void Plugin::setProperty(const QString& name, const QVariant& value)
{
    _properties[name] = value;
}

bool Plugin::hasProperty(const QString& name) const
{
    return _properties.contains(name);
}

QStringList Plugin::propertyNames() const
{
    return _properties.keys();
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

std::uint32_t Plugin::getNumberOfInstances(const QString& pluginKind)
{
    return Plugin::noInstances[pluginKind];
}

hdps::gui::StringAction& Plugin::getGuiNameAction()
{
    return _guiNameAction;
}

}
}
