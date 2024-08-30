// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Plugin.h"
#include "CoreInterface.h"

#ifdef _DEBUG
    //#define PLUGIN_VERBOSE
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
    _guiNameAction(this, "Plugin title", QString("%1 %2").arg(getKind(), (factory->getMaximumNumberOfInstances() == 1 ? "1" : QString::number(factory->getNumberOfInstances() + 1)))),
    _destroyAction(this, "Remove"),
    _viewShortcutMapAction(this, "Shortcut map")
{
    setConnectionPermissionsFlag(WidgetAction::ConnectionPermissionFlag::ForceNone);

    _guiNameAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _guiNameAction.setPlaceHolderString("Enter plugin name here...");
    _guiNameAction.setConnectionPermissionsToForceNone();

    _destroyAction.setToolTip(QString("Remove %1").arg(getGuiName()));
    _destroyAction.setIconByName("trash");
    _destroyAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _destroyAction.setConnectionPermissionsToForceNone();

    _viewShortcutMapAction.setToolTip("View shortcut map");
    _viewShortcutMapAction.setIconByName("keyboard");
    _viewShortcutMapAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _viewShortcutMapAction.setConnectionPermissionsToForceNone();

    connect(&_destroyAction, &TriggerAction::triggered, this, &Plugin::destroy);
    connect(&_viewShortcutMapAction, &TriggerAction::triggered, this, &Plugin::viewShortcutMap);
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

util::ShortcutMap& Plugin::getShortcutMap()
{
    return const_cast<PluginFactory*>(_factory)->getShortcutMap();
}

const util::ShortcutMap& Plugin::getShortcutMap() const
{
    return const_cast<Plugin*>(this)->getShortcutMap();
}

void Plugin::addShortcut(const util::ShortcutMap::Shortcut& shortcut)
{
    getShortcutMap().addShortcut(shortcut);
}

void Plugin::removeShortcut(const util::ShortcutMap::Shortcut& shortcut)
{
    getShortcutMap().removeShortcut(shortcut);
}

util::ShortcutMap::Shortcuts Plugin::getShortcuts(const QStringList& categories) const
{
    return getShortcutMap().getShortcuts(categories);
}

bool Plugin::hasShortcuts(const QStringList& categories) const
{
    return getShortcutMap().hasShortcuts(categories);
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

QString Plugin::getShortDescription() const
{
    return _shortDescription;
}

void Plugin::setShortDescription(const QString& shortDescription)
{
    if (shortDescription == _shortDescription)
        return;

    const auto previousShortDescription = _shortDescription;

    _shortDescription = shortDescription;

    emit shortDescriptionChanged(previousShortDescription, _shortDescription);
}

QString Plugin::getLongDescription() const
{
    return _shortDescription;
}

void Plugin::setLongDescription(const QString& longDescription)
{
    if (longDescription == _longDescription)
        return;

    const auto previouslongDescription = _longDescription;

    _longDescription = longDescription;

    emit longDescriptionChanged(previouslongDescription, _longDescription);
}

QVariant Plugin::getSetting(const QString& path, const QVariant& defaultValue /*= QVariant()*/) const
{
    return Application::current()->getSetting(QString("%1%2").arg(getGlobalSettingsPrefix(), path), defaultValue);
}

void Plugin::setSetting(const QString& path, const QVariant& value)
{
    Application::current()->setSetting(QString("%1%2").arg(getGlobalSettingsPrefix(), path), value);
}

QString Plugin::getGlobalSettingsPrefix() const
{
    return _factory->getGlobalSettingsPrefix();
}

PluginGlobalSettingsGroupAction* Plugin::getGlobalSettingsAction() const
{
    return _factory->getGlobalSettingsGroupAction();
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

void Plugin::viewShortcutMap()
{
    qDebug() << __FUNCTION__ << "not implemented yet...";
}

void Plugin::destroy()
{
    plugins().destroyPlugin(this);
}

}
