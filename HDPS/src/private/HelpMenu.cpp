#include "HelpMenu.h"
#include "PluginManager.h"
#include "PluginHelpMenu.h"

using namespace hdps;

HelpMenu::HelpMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Help");
    setToolTip("HDPS help");

    addMenu(new PluginHelpMenu());
}

