#include "ResetMenu.h"

#include <Application.h>

using namespace hdps;

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
    _resetAllAction.setToolTip("Reset the data ánd the layout");

    addAction(&_resetDataAction);
    addAction(&_resetLayoutAction);

    addSeparator();

    addAction(&_resetAllAction);
}

