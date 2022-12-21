#include "ViewPlugin.h"
#include "CoreInterface.h"
#include "widgets/ProjectEditorDialog.h"
#include "Application.h"
#include "AbstractWorkspaceManager.h"

#include <QWidget>
#include <QFileDialog>

using namespace hdps::gui;

namespace hdps::plugin
{

ViewPlugin::ViewPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _widget(),
    _editActionsAction(nullptr, "Edit..."),
    _screenshotAction(nullptr, "Screenshot..."),
    _isolateAction(nullptr, "Isolate"),
    _mayCloseAction(nullptr, "May close", true, true),
    _mayFloatAction(nullptr, "May float", true, true),
    _mayMoveAction(nullptr, "May move", true, true),
    _visibleAction(nullptr, "Visible", true, true),
    _helpAction(nullptr, "Trigger help"),
    _removeAction(nullptr, "Remove")
{
    setText(getGuiName());

    _widget.setAutoFillBackground(true);

    _widget.addAction(&_editActionsAction);
    _widget.addAction(&_screenshotAction);
    _widget.addAction(&_isolateAction);
    _widget.addAction(&_helpAction);

    _editActionsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    _editActionsAction.setShortcut(tr("F12"));
    _editActionsAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _editActionsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _editActionsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _editActionsAction.setConnectionPermissionsToNone();

    _screenshotAction.setIcon(Application::getIconFont("FontAwesome").getIcon("camera"));
    _screenshotAction.setShortcut(tr("F2"));
    _screenshotAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _screenshotAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _screenshotAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _screenshotAction.setConnectionPermissionsToNone();

    _isolateAction.setIcon(Application::getIconFont("FontAwesome").getIcon("crosshairs"));
    _isolateAction.setShortcut(tr("F3"));
    _isolateAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _isolateAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _isolateAction.setConnectionPermissionsToNone();

    _mayCloseAction.setToolTip("Determines whether this view plugin may be closed or not");
    _mayCloseAction.setConnectionPermissionsToNone();
    _mayCloseAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _mayCloseAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _mayCloseAction.setConnectionPermissionsToNone();

    _mayFloatAction.setToolTip("Determines whether this view plugin may float or not");
    _mayFloatAction.setConnectionPermissionsToNone();
    _mayFloatAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _mayFloatAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _mayFloatAction.setConnectionPermissionsToNone();

    _mayMoveAction.setToolTip("Determines whether this view plugin may be moved or not");
    _mayMoveAction.setConnectionPermissionsToNone();
    _mayMoveAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _mayMoveAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _mayMoveAction.setConnectionPermissionsToNone();

    _visibleAction.setToolTip("Determines whether the view plugin is visible in the user interface or not");
    _visibleAction.setIcon(getIcon());
    _visibleAction.setConnectionPermissionsToNone();
    _visibleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _visibleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _visibleAction.setConnectionPermissionsToNone();

    _helpAction.setToolTip(QString("Shows %1 documentation").arg(factory->getKind()));
    _helpAction.setShortcut(tr("F1"));
    _helpAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _helpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu, false);
    _helpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly, false);
    _helpAction.setConnectionPermissionsToNone();
    
    _removeAction.setToolTip(QString("Remove %1").arg(getGuiName()));
    _removeAction.setIcon(Application::getIconFont("FontAwesome").getIcon("trash"));
    _removeAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _removeAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);
    _removeAction.setConnectionPermissionsToNone();

    connect(&_editActionsAction, &TriggerAction::triggered, this, [this]() -> void {
        ProjectEditorDialog viewPluginEditorDialog(nullptr, this);
        viewPluginEditorDialog.exec();
    });

    connect(&_screenshotAction, &TriggerAction::triggered, this, [this]() -> void {
        createScreenshot();
    });

    connect(&_isolateAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
        Application::core()->getWorkspaceManager().isolateViewPlugin(this, toggled);
    });

    connect(&_helpAction, &TriggerAction::triggered, this, [this]() -> void {
        getTriggerHelpAction().trigger();
    });

    connect(&_removeAction, &TriggerAction::triggered, this, [this]() -> void {
        Application::core()->getPluginManager().destroyPlugin(this);
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

void ViewPlugin::createScreenshot()
{
    QFileDialog fileDialog;

    fileDialog.setWindowTitle("Save screenshot to image file");
    fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("camera"));
    fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setNameFilters({ "Image files (*.png)" });
    fileDialog.setDefaultSuffix(".png");

    const auto cachedDirectory = QFileInfo(Application::current()->getSetting("ScreenShot/ViewPlugin/Directory", ".").toString()).dir();

    fileDialog.setDirectory(cachedDirectory);

    if (fileDialog.exec() == 0)
        return;

    if (fileDialog.selectedFiles().count() != 1)
        throw std::runtime_error("Only one file may be selected");

    Application::current()->setSetting("ScreenShot/ViewPlugin/Directory", fileDialog.selectedFiles().first());

    auto widgetPixmap = getWidget().grab();

    widgetPixmap.toImage().save(fileDialog.selectedFiles().first());
}

ViewPluginFactory::ViewPluginFactory(bool producesSystemViewPlugins /*= false*/) :
    PluginFactory(Type::VIEW),
    _producesSystemViewPlugins(producesSystemViewPlugins),
    _preferredDockArea(DockAreaFlag::Right)
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

DockAreaFlag ViewPluginFactory::getPreferredDockArea() const
{
    return _preferredDockArea;
}

void ViewPluginFactory::setPreferredDockArea(const gui::DockAreaFlag& preferredDockArea)
{
    _preferredDockArea = preferredDockArea;
}

}
