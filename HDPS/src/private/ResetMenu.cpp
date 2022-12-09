#include "ResetMenu.h"

#include <Application.h>

using namespace hdps;
using namespace hdps::gui;

ResetMenu::ResetMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _resetDataAction(this, "Data"),
    _resetLayoutAction(this, "Layout"),
    _resetAllAction(this, "All")
{
    setTitle("Reset");
    setToolTip("Reset the project (or parts of it)");
    setIcon(Application::getIconFont("FontAwesome").getIcon("redo-alt"));

    _resetDataAction.setToolTip("Reset the data hierarchy");
    _resetLayoutAction.setToolTip("Reset the layout");
    _resetAllAction.setToolTip("Reset the data and the layout");

    connect(&_resetDataAction, &TriggerAction::triggered, this, []() -> void {
        Application::core()->getDataManager().reset();
    });

    connect(&_resetLayoutAction, &TriggerAction::triggered, this, []() -> void {
        Application::core()->getLayoutManager().reset();
    });

    connect(&_resetAllAction, &TriggerAction::triggered, this, []() -> void {
        Application::core()->getDataManager().reset();
        Application::core()->getLayoutManager().reset();
        Application::core()->getPluginManager().reset();
    });

    addAction(&_resetDataAction);
    addAction(&_resetLayoutAction);

    addSeparator();

    addAction(&_resetAllAction);
}
