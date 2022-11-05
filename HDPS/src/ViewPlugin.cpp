#include "ViewPlugin.h"

#include <QWidget>

namespace hdps
{

namespace plugin
{

ViewPlugin::ViewPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _widget(),
    _editActionsAction(&_widget, "Edit view plugin actions"),
    _mayCloseAction(this, "May close", true, true),
    _mayFloatAction(this, "May float", true, true),
    _visibleAction(this, "Visible", true, true),
    _triggerHelpAction(this, "Trigger help")
{
    setText(getGuiName());

    _widget.setAutoFillBackground(true);

    _widget.addAction(&_editActionsAction);
    _widget.addAction(&_triggerHelpAction);

    _editActionsAction.setShortcut(tr("F12"));
    _editActionsAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _editActionsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu, false);
    _editActionsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInHierarchy, false);

    _mayCloseAction.setToolTip("Determines whether this view plugin may be closed or not");
    _mayCloseAction.setConnectionPermissionsToNone();
    _mayCloseAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu, false);
    _mayCloseAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInHierarchy, false);

    _mayFloatAction.setToolTip("Determines whether this view plugin may float or not");
    _mayFloatAction.setConnectionPermissionsToNone();
    _mayFloatAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu, false);
    _mayFloatAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInHierarchy, false);

    _visibleAction.setToolTip("Determines whether the view plugin is visible in the user interface or not");
    _visibleAction.setConnectionPermissionsToNone();
    _visibleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu, false);
    _visibleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInHierarchy, false);

    _triggerHelpAction.setShortcut(tr("F1"));
    _triggerHelpAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _triggerHelpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu, false);
    _triggerHelpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInHierarchy, false);
    
    connect(&_editActionsAction, &TriggerAction::triggered, this, [this]() -> void {
        ViewPluginEditorDialog viewPluginEditorDialog(nullptr, this);
        viewPluginEditorDialog.exec();
    });

    connect(&_triggerHelpAction, &TriggerAction::triggered, this, [this]() -> void {
        getTriggerHelpAction().trigger();
    });

    const auto updateVisibleAction = [this]() -> void {
        _visibleAction.setEnabled(_mayCloseAction.isChecked());
    };

    connect(&_mayCloseAction, &ToggleAction::toggled, this, updateVisibleAction);

    _visibleAction.setText(getGuiName());

    updateVisibleAction();
}

void ViewPlugin::setObjectName(const QString& name)
{
    QObject::setObjectName("Plugins/View/" + name);
}

void ViewPlugin::loadData(const Datasets& datasets)
{
    qDebug() << "Load function not implemented in view plugin implementation";
}

ViewPluginFactory::ViewPluginFactory() :
    PluginFactory(Type::VIEW)
{
}

QIcon ViewPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("eye", color);
}

}

}
