#include "WorkspaceSettingsDialog.h"

#include <AbstractWorkspaceManager.h>

#include <Application.h>

#include <QVBoxLayout>

#ifdef _DEBUG
    #define WORKSPACE_SETTINGS_DIALOG_VERBOSE
#endif

using namespace hdps;
using namespace hdps::gui;

WorkspaceSettingsDialog::WorkspaceSettingsDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _groupAction(this),
    _okAction(this, "Ok")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setModal(true);
    setWindowTitle("Edit Workspace Settings");

    _groupAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

    const auto workspace = Application::core()->getWorkspaceManager().getWorkspace();

    _groupAction << workspace->getDescriptionAction();
    _groupAction << workspace->getTagsAction();
    _groupAction << workspace->getCommentsAction();

    auto layout = new QVBoxLayout();

    auto groupActionWidget = _groupAction.createWidget(this);

    groupActionWidget->layout()->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(groupActionWidget);

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_okAction.createWidget(this));

    layout->addLayout(bottomLayout);

    setLayout(layout);

    connect(&_okAction, &TriggerAction::triggered, this, &WorkspaceSettingsDialog::accept);
}
