#include "ViewPlugin.h"
#include "CoreInterface.h"
#include "widgets/ProjectEditorDialog.h"
#include "Application.h"
#include "AbstractWorkspaceManager.h"

#include <QWidget>
#include <QFileDialog>
#include <QInputDialog>

#ifdef _DEBUG
    #define VIEW_PLUGIN_VERBOSE
#endif

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps::plugin
{

ViewPlugin::ViewPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _widget(),
    _editActionsAction(&_widget, "Edit..."),
    _screenshotAction(&_widget, "Screenshot..."),
    _isolateAction(&_widget, "Isolate"),
    _mayCloseAction(this, "May close", true, true),
    _mayFloatAction(this, "May float", true, true),
    _mayMoveAction(this, "May move", true, true),
    _lockingAction(this),
    _visibleAction(this, "Visible", true, true),
    _helpAction(this, "Trigger help"),
    _triggerShortcut()
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

    _screenshotAction.setIcon(Application::getIconFont("FontAwesome").getIcon("camera"));
    _screenshotAction.setShortcut(tr("F2"));
    _screenshotAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _screenshotAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _screenshotAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    _isolateAction.setIcon(Application::getIconFont("FontAwesome").getIcon("crosshairs"));
    _isolateAction.setShortcut(tr("F3"));
    _isolateAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _isolateAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);

    _mayCloseAction.setToolTip("Determines whether this view plugin may be closed or not");
    _mayCloseAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _mayCloseAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    _mayFloatAction.setToolTip("Determines whether this view plugin may float or not");
    _mayFloatAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _mayFloatAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    _mayMoveAction.setToolTip("Determines whether this view plugin may be moved or not");
    _mayMoveAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _mayMoveAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    _lockingAction.setWhat("Layout");

    _visibleAction.setToolTip("Determines whether the view plugin is visible or not");
    _visibleAction.setIcon(getIcon());
    _visibleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu);
    _visibleAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly);

    _helpAction.setToolTip(QString("Shows %1 documentation").arg(factory->getKind()));
    _helpAction.setShortcut(tr("F1"));
    _helpAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    _helpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::VisibleInMenu, false);
    _helpAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::InternalUseOnly, false);

    connect(&_editActionsAction, &TriggerAction::triggered, this, [this]() -> void {
        ProjectEditorDialog viewPluginEditorDialog(nullptr, this);
        viewPluginEditorDialog.exec();
    });

    connect(&_screenshotAction, &TriggerAction::triggered, this, [this]() -> void {
        createScreenshot();
    });

    connect(&_isolateAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
        workspaces().isolateViewPlugin(this, toggled);
    });

    const auto updateDockWidgetPermissionsReadOnly = [this]() -> void {
        const auto enabled = !_lockingAction.getLockedAction().isChecked();

        _mayCloseAction.setEnabled(enabled);
        _mayFloatAction.setEnabled(enabled);
        _mayMoveAction.setEnabled(enabled);
    };

    connect(&_lockingAction.getLockedAction(), &ToggleAction::toggled, this, [this, updateDockWidgetPermissionsReadOnly](bool toggled) -> void {
        _mayCloseAction.setChecked(!toggled);
        _mayFloatAction.setChecked(!toggled);
        _mayMoveAction.setChecked(!toggled);
        _isolateAction.setEnabled(!toggled);

        getDestroyAction().setEnabled(!toggled);

        updateDockWidgetPermissionsReadOnly();
    });

    updateDockWidgetPermissionsReadOnly();

    connect(&_helpAction, &TriggerAction::triggered, this, [this]() -> void {
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

void ViewPlugin::createScreenshot()
{
    QFileDialog fileDialog;

    fileDialog.setWindowTitle("Save Screenshot");
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

QKeySequence ViewPlugin::getTriggerShortcut() const
{
    return _triggerShortcut;
}

void ViewPlugin::setTriggerShortcut(const QKeySequence& keySequence)
{
    _triggerShortcut = keySequence;
}

void ViewPlugin::fromVariantMap(const QVariantMap& variantMap)
{
    Plugin::fromVariantMap(variantMap);

    Serializable::fromVariantMap(_mayCloseAction, variantMap, "MayClose");
    Serializable::fromVariantMap(_mayFloatAction, variantMap, "MayFloat");
    Serializable::fromVariantMap(_mayMoveAction, variantMap, "MayMove");
    Serializable::fromVariantMap(_lockingAction, variantMap, "Locking");
    Serializable::fromVariantMap(_visibleAction, variantMap, "Visible");
}

QVariantMap ViewPlugin::toVariantMap() const
{
    auto variantMap = Plugin::toVariantMap();

    Serializable::insertIntoVariantMap(_mayCloseAction, variantMap, "MayClose");
    Serializable::insertIntoVariantMap(_mayFloatAction, variantMap, "MayFloat");
    Serializable::insertIntoVariantMap(_mayMoveAction, variantMap, "MayMove");
    Serializable::insertIntoVariantMap(_lockingAction, variantMap, "Locking");
    Serializable::insertIntoVariantMap(_visibleAction, variantMap, "Visible");

    return variantMap;
}

void ViewPlugin::loadPreset(const QString& name)
{
#ifdef VIEW_PLUGIN_VERBOSE
    qDebug() << __FUNCTION__ << name;
#endif

    if (name.isEmpty())
        return;

    auto presets = getSetting("Presets", QVariantMap()).toMap();

    if (!presets.contains(name))
        return;

    fromVariantMap(presets[name].toMap());
}

void ViewPlugin::savePreset(const QString& name)
{
#ifdef VIEW_PLUGIN_VERBOSE
    qDebug() << __FUNCTION__ << name;
#endif

    if (name.isEmpty())
        return;

    auto presets = getSetting("Presets", QVariantMap()).toMap();

    presets[name] = toVariantMap();

    setSetting("Presets", presets);
}

void ViewPlugin::importPreset()
{
#ifdef VIEW_PLUGIN_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QFileDialog fileDialog;

    fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    fileDialog.setWindowTitle("Import Preset");
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setNameFilters({ "HDPS View Preset (*.hvp)" });
    fileDialog.setDefaultSuffix(".hvp");
    fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

    if (fileDialog.exec() == 0)
        return;

    if (fileDialog.selectedFiles().count() != 1)
        throw std::runtime_error("Only one file may be selected");

    const auto presetFilePath = fileDialog.selectedFiles().first();

    fromJsonFile(presetFilePath);
    savePreset(QFileInfo(presetFilePath).baseName());
}

void ViewPlugin::exportPreset()
{
#ifdef VIEW_PLUGIN_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QFileDialog fileDialog;

    fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("file-export"));
    fileDialog.setWindowTitle("Export Preset");
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setNameFilters({ "HDPS View Preset (*.hvp)" });
    fileDialog.setDefaultSuffix(".hvp");
    fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

    if (fileDialog.exec() == 0)
        return;

    if (fileDialog.selectedFiles().count() != 1)
        throw std::runtime_error("Only one file may be selected");

    toJsonFile(fileDialog.selectedFiles().first());
}

QMenu* ViewPlugin::getPresetsMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu("Presets", parent);

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    const auto presetIcon = fontAwesome.getIcon("prescription");

    menu->setIcon(presetIcon);

    auto savePresetAction = new QAction("Save");

    savePresetAction->setIcon(fontAwesome.getIcon("save"));

    connect(savePresetAction, &TriggerAction::triggered, this, [this, &fontAwesome]() -> void {
        QInputDialog inputDialog;

        inputDialog.setWindowIcon(fontAwesome.getIcon("list-alt"));
        inputDialog.setWindowTitle("Choose preset name");
        inputDialog.setInputMode(QInputDialog::TextInput);
        inputDialog.setLabelText("Preset name:");

        if (inputDialog.exec())
            savePreset(inputDialog.textValue());
    });

    menu->addAction(savePresetAction);

    auto saveDefaultPresetAction = new QAction("Save As Default");

    saveDefaultPresetAction->setIcon(fontAwesome.getIcon("save"));

    connect(saveDefaultPresetAction, &TriggerAction::triggered, this, &ViewPlugin::saveDefaultPreset);

    menu->addAction(saveDefaultPresetAction);

    auto presetNames = getSetting("Presets").toMap().keys();

    if (!presetNames.isEmpty())
        menu->addSeparator();

    if (presetNames.contains("Default"))
        presetNames.removeOne("Default");

    presetNames.insert(0, "Default");

    for (const auto& presetName : presetNames) {
        auto loadPresetAction = new QAction(presetName);

        loadPresetAction->setIcon(presetIcon);

        connect(loadPresetAction, &TriggerAction::triggered, this, [this, presetName]() -> void {
            loadPreset(presetName);
        });

        menu->addAction(loadPresetAction);

        if (presetName == "Default")
            menu->addSeparator();
    }

    menu->addSeparator();

    auto importPresetAction = new QAction("Import...");
    auto exportPresetAction = new QAction("Export...");

    importPresetAction->setIcon(fontAwesome.getIcon("file-import"));
    exportPresetAction->setIcon(fontAwesome.getIcon("file-export"));

    menu->addAction(importPresetAction);
    menu->addAction(exportPresetAction);

    connect(importPresetAction, &TriggerAction::triggered, this, &ViewPlugin::importPreset);
    connect(exportPresetAction, &TriggerAction::triggered, this, &ViewPlugin::exportPreset);

    /*
    menu->addSeparator();

    auto editPresetsAction = new QAction("Edit...");

    editPresetsAction->setIcon(fontAwesome.getIcon("cog"));

    menu->addAction(editPresetsAction);

    connect(editPresetsAction, &TriggerAction::triggered, this, [this, presetNames, &fontAwesome]() -> void {
    });
    */

    return menu;
}

void ViewPlugin::loadDefaultPreset()
{
    loadPreset("Default");
}

void ViewPlugin::saveDefaultPreset()
{
    savePreset("Default");
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
