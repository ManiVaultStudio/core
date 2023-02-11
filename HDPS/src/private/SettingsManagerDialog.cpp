#include "SettingsManagerDialog.h"

#include <Application.h>
#include <CoreInterface.h>
#include <QVBoxLayout>

using namespace hdps;

#ifdef _DEBUG
    #define SETTINGS_MANAGER_DIALOG_VERBOSE
#endif

namespace hdps::gui {

SettingsManagerDialog::SettingsManagerDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _groupsAction(this, "Groups"),
    _globalPathsGroupAction(this, "Global Paths", true),
    _ioAction(this, "IO", true)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    setModal(true);
    setWindowTitle("Settings");

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->addWidget(_groupsAction.createWidget(this));

    _globalPathsGroupAction.setText("Paths");
    _globalPathsGroupAction.setLabelWidthPercentage(20);

    _globalPathsGroupAction << settings().getGlobalProjectsPathAction();
    _globalPathsGroupAction << settings().getGlobalWorkspacesPathAction();
    _globalPathsGroupAction << settings().getGlobalDataPathAction();

    _ioAction.setText("IO");
    _ioAction.setShowLabels(false);
    
    _ioAction << settings().getIgnoreLoadingErrorsAction();
    //_ioAction << *toggleAction;

    //_groupsAction.addGroupAction(&_globalPathsGroupAction);
    _groupsAction.addGroupAction(&_ioAction);
}

QSize SettingsManagerDialog::sizeHint() const
{
    return QSize(400, 500);
}

}
