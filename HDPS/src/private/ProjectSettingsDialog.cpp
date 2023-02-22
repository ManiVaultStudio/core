#include "ProjectSettingsDialog.h"

#include <AbstractProjectManager.h>

#include <Application.h>
#include <CoreInterface.h>

#include <QVBoxLayout>
#include <QVector>

#ifdef _DEBUG
    #define PROJECT_SETTINGS_DIALOG_VERBOSE
#endif

using namespace hdps;
using namespace hdps::gui;

ProjectSettingsDialog::ProjectSettingsDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _groupAction(this),
    _okAction(this, "Ok")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setModal(true);
    setWindowTitle("Project Settings");

    _groupAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

    const auto project = projects().getCurrentProject();

    _groupAction << project->getTitleAction();
    _groupAction << project->getDescriptionAction();
    _groupAction << project->getProjectVersionAction();
    _groupAction << project->getTagsAction();
    _groupAction << project->getCommentsAction();
    _groupAction << project->getContributorsAction();
    _groupAction << project->getSplashScreenAction();
    _groupAction << workspaces().getLockingAction().getLockedAction();

    auto layout = new QVBoxLayout();

    auto groupActionWidget = _groupAction.createWidget(this);

    groupActionWidget->layout()->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(groupActionWidget);

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_okAction.createWidget(this));

    layout->addLayout(bottomLayout);

    setLayout(layout);

    connect(&_okAction, &TriggerAction::triggered, this, &ProjectSettingsDialog::accept);
}
