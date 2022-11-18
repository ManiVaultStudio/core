#include "ViewPlugin.h"

#include "widgets/ProjectEditorDialog.h"
#include "actions/ViewPluginTriggerAction.h"
#include "Application.h"
#include "AbstractLayoutManager.h"

#include <QWidget>

namespace hdps::plugin
{

ViewPlugin::ViewPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _widget(),
    _editActionsAction(&_widget, "Edit view plugin actions"),
    _mayCloseAction(this, "May close", true, true),
    _mayFloatAction(this, "May float", true, true),
    _mayMoveAction(this, "May move", true, true),
    _visibleAction(this, "Visible", true, true),
    _triggerHelpAction(this, "Trigger help")
{
    setText(getGuiName());

    _widget.setAutoFillBackground(true);

    _widget.addAction(&_editActionsAction);
    _widget.addAction(&_triggerHelpAction);

    _editActionsAction.setShortcut(tr("F12"));
    _editActionsAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _editActionsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _editActionsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    _mayCloseAction.setToolTip("Determines whether this view plugin may be closed or not");
    _mayCloseAction.setConnectionPermissionsToNone();
    _mayCloseAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _mayCloseAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    _mayFloatAction.setToolTip("Determines whether this view plugin may float or not");
    _mayFloatAction.setConnectionPermissionsToNone();
    _mayFloatAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _mayFloatAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    _mayMoveAction.setToolTip("Determines whether this view plugin may be moved or not");
    _mayMoveAction.setConnectionPermissionsToNone();
    _mayMoveAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _mayMoveAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    _visibleAction.setToolTip("Determines whether the view plugin is visible in the user interface or not");
    _visibleAction.setConnectionPermissionsToNone();
    _visibleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _visibleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    _triggerHelpAction.setShortcut(tr("F1"));
    _triggerHelpAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _triggerHelpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu, false);
    _triggerHelpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly, false);
    
    connect(&_editActionsAction, &TriggerAction::triggered, this, [this]() -> void {
        ProjectEditorDialog viewPluginEditorDialog(nullptr, this);
        viewPluginEditorDialog.exec();
    });

    connect(&_triggerHelpAction, &TriggerAction::triggered, this, [this]() -> void {
        getTriggerHelpAction().trigger();
    });

    connect(&getGuiNameAction(), &StringAction::stringChanged, this, [this](const QString& guiName) -> void {
        _widget.setWindowTitle(guiName);
    });

    const auto updateVisibleAction = [this]() -> void {
        _visibleAction.setEnabled(_mayCloseAction.isChecked());
    };

    connect(&_mayCloseAction, &ToggleAction::toggled, this, updateVisibleAction);

    _visibleAction.setText(getGuiName());

    updateVisibleAction();
}

void ViewPlugin::init()
{
}

void ViewPlugin::setObjectName(const QString& name)
{
    QObject::setObjectName("Plugins/View/" + name);
}

void ViewPlugin::loadData(const Datasets& datasets)
{
    qDebug() << "Load function not implemented in view plugin implementation";
}

QWidget& ViewPlugin::getWidget()
{
    return _widget;
}

bool ViewPlugin::isSystemViewPlugin() const
{
    return dynamic_cast<const ViewPluginFactory*>(_factory)->producesSystemViewPlugins();
}

ViewPluginFactory::ViewPluginFactory(bool producesSystemViewPlugins /*= false*/) :
    PluginFactory(Type::VIEW),
    _producesSystemViewPlugins(producesSystemViewPlugins),
    _viewPluginTriggerAction(new ViewPluginTriggerAction(this, this))
{
    if (_producesSystemViewPlugins)
        setMaximumNumberOfInstances(1);
}

QIcon ViewPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("eye", color);
}

bool ViewPluginFactory::producesSystemViewPlugins() const
{
    return _producesSystemViewPlugins;
}

hdps::gui::PluginTriggerAction& ViewPluginFactory::getPluginTriggerAction()
{
    return *_viewPluginTriggerAction;
}

}
