#include "OverwriteClustersConfirmationDialog.h"

#include <Application.h>

#include <QVBoxLayout>

using namespace hdps;

OverwriteClustersConfirmationDialog::OverwriteClustersConfirmationDialog(QWidget* parent, std::uint32_t numberOfUserClusters, std::uint32_t numberOfUserModifiedClusters) :
    QDialog(parent),
    _alwaysAskPermissionAction(this, "Always ask permission"),
    _discardAction(this, "Discard"),
    _cancelAction(this, "Cancel")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("random"));
    setWindowTitle("About to overwrite all cluster(s)");
    setModal(true);

    // Check whether to show the dialog again or not
    _alwaysAskPermissionAction.setChecked(true);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    // Ask for confirmation question
    const auto confirmationQuestion = QString("You made changes to %1 cluster%2, would you like to discard these changes?").arg(QString::number(numberOfUserModifiedClusters), numberOfUserModifiedClusters > 1 ? "s" : "");

    layout->addWidget(new QLabel(confirmationQuestion));

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addWidget(_alwaysAskPermissionAction.createWidget(this));
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_discardAction.createWidget(this));
    bottomLayout->addWidget(_cancelAction.createWidget(this));

    layout->addStretch(1);
    layout->addLayout(bottomLayout);

    connect(&_alwaysAskPermissionAction, &ToggleAction::toggled, this, [this](bool toggled) {
        Application::current()->setSetting("OverwriteClustersAskConfirmation", toggled);
    });

    connect(&_discardAction, &TriggerAction::triggered, this, &OverwriteClustersConfirmationDialog::accept);
    connect(&_cancelAction, &TriggerAction::triggered, this, &OverwriteClustersConfirmationDialog::reject);
}