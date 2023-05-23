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
    _globalPathsGroupAction(this, "Paths", true),
    _ioGroupAction(this, "IO", true),
    _parametersGroupAction(this, "Parameters", true)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    setModal(true);
    setWindowTitle("Settings");

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->addWidget(_groupsAction.createWidget(this));

    _globalPathsGroupAction.setLabelWidthPercentage(20);

    _globalPathsGroupAction << settings().getGlobalProjectsPathAction();
    _globalPathsGroupAction << settings().getGlobalWorkspacesPathAction();
    _globalPathsGroupAction << settings().getGlobalDataPathAction();

    _ioGroupAction.setShowLabels(false);

    _ioGroupAction << settings().getIgnoreLoadingErrorsAction();

    _parametersGroupAction.setShowLabels(false);

    _parametersGroupAction << settings().getAskForSharedParameterNameAction();

    _groupsAction.addGroupAction(&_ioGroupAction);
    _groupsAction.addGroupAction(&_parametersGroupAction);
}

QSize SettingsManagerDialog::sizeHint() const
{
    return QSize(400, 500);
}

}
