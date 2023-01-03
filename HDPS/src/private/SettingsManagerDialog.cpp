#include "SettingsManagerDialog.h"

#include <Application.h>

#include <QVBoxLayout>

using namespace hdps;

#ifdef _DEBUG
    #define SETTINGS_MANAGER_DIALOG_VERBOSE
#endif

namespace hdps::gui {

SettingsManagerDialog::SettingsManagerDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _groupsAction(this),
    _globalPathsGroupAction(this, true)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    setModal(true);
    setWindowTitle("Settings");

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->addWidget(_groupsAction.createWidget(this));
    
    auto& settingsManager = Application::core()->getSettingsManager();

    _globalPathsGroupAction.setText("Paths");
    _globalPathsGroupAction.setLabelWidthPercentage(20);

    _globalPathsGroupAction << settingsManager.getGlobalProjectsPathAction();
    _globalPathsGroupAction << settingsManager.getGlobalWorkspacesPathAction();
    _globalPathsGroupAction << settingsManager.getGlobalDataPathAction();

    _groupsAction.addGroupAction(&_globalPathsGroupAction);
}

}
