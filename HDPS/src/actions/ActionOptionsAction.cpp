#include "ActionOptionsAction.h"
#include "Application.h"

#include <QMenu>

namespace hdps {

namespace gui {

ActionOptionsAction::ActionOptionsAction(QObject* parent, WidgetAction* widgetAction) :
    WidgetAction(parent),
    _widgetAction(widgetAction),
    _loadDefaultAction(this, "Load default"),
    _saveDefaultAction(this, "Save default"),
    _factoryDefaultAction(this, "Factory default"),
    _ignoreResettableSignals(false)
{
    setText("Options");
    setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setSerializable(false, false);

    // Show icons only
    _loadDefaultAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _saveDefaultAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _factoryDefaultAction.setDefaultWidgetFlags(TriggerAction::Icon);

    // Set action icons
    _loadDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("undo"));
    _saveDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _factoryDefaultAction.setIcon(Application::getIconFont("FontAwesome").getIcon("industry"));

    // Set action tooltips
    _loadDefaultAction.setToolTip("Load default properties");
    _saveDefaultAction.setToolTip("Save default properties");
    _factoryDefaultAction.setToolTip("Restore factory default properties");

    // Load default when triggered
    connect(&_loadDefaultAction, &TriggerAction::triggered, this, [this]() -> void {

        // Prevent unnecessary callbacks
        _ignoreResettableSignals = true;

        // Load default recursively
        _widgetAction->loadDefault();

        // Update the read-only state of the actions
        updateActions();
    });

    // Save default when triggered
    connect(&_saveDefaultAction, &TriggerAction::triggered, this, [this]() -> void {
        _widgetAction->saveDefault();

        // Update the read-only state of the actions
        updateActions();
    });

    // Load factory default when triggered
    connect(&_factoryDefaultAction, &TriggerAction::triggered, this, [this]() -> void {
        _widgetAction->reset();

        // Update the read-only state of the actions
        updateActions();
    });

    // Update actions when the resettable-ness of the widget action changed
    connect(_widgetAction, &WidgetAction::resettableChanged, this, [this]() -> void {
        if (!_ignoreResettableSignals)
            updateActions();
    });

    // Update actions when the factory resettable-ness of the widget action changed
    connect(_widgetAction, &WidgetAction::factoryResettableChanged, this, [this]() -> void {
        if (!_ignoreResettableSignals)
            updateActions();
    });

    // Perform initial update of the actions
    updateActions();
}

QMenu* ActionOptionsAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    // Create main context menu
    auto contextMenu = new QMenu();

    // Add load/save default actions
    contextMenu->addAction(&_loadDefaultAction);
    contextMenu->addAction(&_saveDefaultAction);

    contextMenu->addSeparator();

    // Create context menu for additional options
    auto optionsMenu = new QMenu("Options");

    // Configure the context menu and add reset to factory default action
    //optionsMenu->setEnabled(_widgetAction->isFactoryResettable());
    optionsMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    optionsMenu->addAction(&_factoryDefaultAction);

    // Add the options menu to the main context menu
    contextMenu->addMenu(optionsMenu);

    return contextMenu;
}

void ActionOptionsAction::updateActions()
{
#ifdef _DEBUG
    qDebug() << "Update actions";
#endif

    _loadDefaultAction.setEnabled(_widgetAction->isResettable());
    _saveDefaultAction.setEnabled(_widgetAction->canSaveDefault());
    _factoryDefaultAction.setEnabled(_widgetAction->isFactoryResettable());
}

}
}