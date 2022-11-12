#include "ViewPlugin.h"

#include "widgets/ProjectEditorDialog.h"

#include "Application.h"
#include "AbstractLayoutManager.h"

#include <QWidget>

namespace hdps
{

namespace plugin
{

QMap<QString, std::uint32_t> ViewPlugin::dockWidgetAreaMap({
    { "Left", 0x01 },
    { "Right", 0x02 },
    { "Top", 0x04 },
    { "Bottom", 0x08 },
    { "Center", 0x10 }
});

std::int32_t ViewPlugin::getDockWidgetAreas(const QStringList& dockWidgetAreas)
{
    std::int32_t dockAreas = 0;

    for (const auto& dockWidgetArea : dockWidgetAreas) {
        if (dockWidgetAreaMap.contains(dockWidgetArea))
            dockAreas |= dockWidgetAreaMap[dockWidgetArea];
    }

    return dockAreas;
}

ViewPlugin::ViewPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _widget(),
    _editActionsAction(&_widget, "Edit view plugin actions"),
    _mayCloseAction(this, "May close", true, true),
    _mayFloatAction(this, "May float", true, true),
    _mayMoveAction(this, "May move", true, true),
    _visibleAction(this, "Visible", true, true),
    _allowedDockingAreasAction(this, "Allowed docking areas", ViewPlugin::dockWidgetAreaMap.keys(), ViewPlugin::dockWidgetAreaMap.keys()),
    _preferredDockingAreaAction(this, "Preferred docking area", ViewPlugin::dockWidgetAreaMap.keys(), "Bottom"),
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

    _allowedDockingAreasAction.setToolTip("Determines the allowed docking areas for the view plugin");
    _allowedDockingAreasAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _allowedDockingAreasAction.setConnectionPermissionsToNone();
    _allowedDockingAreasAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu, false, true);
    _allowedDockingAreasAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly, false, true);

    _preferredDockingAreaAction.setToolTip("Determines the preferred docking area for the view plugin");
    _preferredDockingAreaAction.setConnectionPermissionsToNone();
    _preferredDockingAreaAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu, false, true);
    _preferredDockingAreaAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly, false, true);

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

bool ViewPlugin::isStandardView() const
{
    return dynamic_cast<const ViewPluginFactory*>(_factory)->isStandardView();
}

ViewPluginFactory::ViewPluginFactory(bool isStandardView /*= false*/) :
    PluginFactory(Type::VIEW),
    _isStandardView(isStandardView)
{
    if (_isStandardView)
        setMaximumNumberOfInstances(1);
}

QIcon ViewPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("eye", color);
}

bool ViewPluginFactory::isStandardView() const
{
    return _isStandardView;
}

}

}
