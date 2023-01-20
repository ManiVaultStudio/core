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
    _freezeAction(this, "Freeze", QVector<TriggersAction::Trigger>({ { "Freeze workspace", "Freeze the workspace (view plugins may not be closed, moved and floated)" }, { "Unfreeze workspace", "Unfreeze the workspace (view plugins may be closed, moved and floated)" } })),
    _okAction(this, "Ok")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setModal(true);
    setWindowTitle("Project Settings");

    _groupAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

    const auto project = projects().getCurrentProject();

    _groupAction << project->getTitleAction();
    _groupAction << project->getDescriptionAction();
    _groupAction << project->getTagsAction();
    _groupAction << project->getCommentsAction();
    _groupAction << _freezeAction;

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

    const auto updateFreezeActionReadOnly = [this]() -> void {
        _freezeAction.setTriggerEnabled(0, workspaces().mayLock());
        _freezeAction.setTriggerEnabled(1, workspaces().mayUnlock());
    };

    connect(&_freezeAction, &TriggersAction::triggered, this, [updateFreezeActionReadOnly](std::int32_t triggerIndex) -> void {
        workspaces().getLockingAction().getLockedAction().setChecked(triggerIndex == 0);
        updateFreezeActionReadOnly();
    });

    updateFreezeActionReadOnly();
}
