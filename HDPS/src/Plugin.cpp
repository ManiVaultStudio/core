// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Plugin.h"
#include "CoreInterface.h"

#ifdef _DEBUG
    #define PLUGIN_VERBOSE
#endif

using namespace mv::gui;

namespace mv::plugin
{

Plugin::Plugin(const PluginFactory* factory) :
    WidgetAction(nullptr, "Plugin"),
    _core(Application::core()),
    _factory(factory),
    _name(getKind() + QUuid::createUuid().toString(QUuid::WithoutBraces)),
    _properties(),
    _eventListener(),
    _guiNameAction(this, "Plugin title", QString("%1 %2").arg(getKind(), (factory->getMaximumNumberOfInstances() == 1 ? "" : QString::number(factory->getNumberOfInstances() + 1)))),
    _destroyAction(this, "Remove")
{
    setConnectionPermissionsFlag(WidgetAction::ConnectionPermissionFlag::ForceNone);

    _guiNameAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _guiNameAction.setPlaceHolderString("Enter plugin name here...");
    _guiNameAction.setConnectionPermissionsToForceNone();

    _destroyAction.setToolTip(QString("Remove %1").arg(getGuiName()));
    _destroyAction.setIconByName("trash");
    _destroyAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _destroyAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _destroyAction.setConnectionPermissionsToForceNone();

    connect(&_destroyAction, &TriggerAction::triggered, this, &Plugin::destroy);
}

Plugin::~Plugin()
{
#ifdef PLUGIN_VERBOSE
    qDebug() << "Removed plugin" << getGuiName();
#endif

    const_cast<PluginFactory*>(_factory)->setNumberOfInstances(_factory->getNumberOfInstances() - 1);
}

const mv::plugin::PluginFactory* Plugin::getFactory() const
{
    return _factory;
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

mv::plugin::Type Plugin::getType() const
{
    return _factory->getType();
}

QString Plugin::getVersion() const
{
    return _factory->getVersion();
}

bool Plugin::hasHelp()
{
    return const_cast<PluginFactory*>(_factory)->hasHelp();
}

mv::gui::TriggerAction& Plugin::getTriggerHelpAction()
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
    WidgetAction::fromVariantMap(variantMap);

    Serializable::fromVariantMap(_guiNameAction, variantMap, "GuiName");
}

QVariantMap Plugin::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    Serializable::insertIntoVariantMap(_guiNameAction, variantMap, "GuiName");

    variantMap.insert({
        { "Name", _name },
        { "Kind", _factory->getKind() },
        { "Type", static_cast<std::uint32_t>(_factory->getType()) },
        { "Version", _factory->getVersion() }
    });

    return variantMap;
}

void Plugin::destroy()
{
    plugins().destroyPlugin(this);
}

}
