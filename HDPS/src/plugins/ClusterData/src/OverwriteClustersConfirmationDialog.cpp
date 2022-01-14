#include "OverwriteClustersConfirmationDialog.h"

#include <Application.h>

#include <QVBoxLayout>

using namespace hdps;

OverwriteClustersConfirmationDialog::OverwriteClustersConfirmationDialog(QWidget* parent, std::uint32_t numberOfUserClusters, std::uint32_t numberOfUserModifiedClusters) :
    QDialog(parent),
    _showAgainAction(this, "Ask for confirmation next time"),
    _overwriteAction(this, "Overwrite"),
    _cancelAction(this, "Cancel")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("random"));
    setWindowTitle(QString("About to overwrite %1 cluster%2").arg(QString::number(numberOfUserClusters), numberOfUserClusters > 1 ? "s" : ""));
    setModal(true);
    //setFixedWidth(400);

    // Check whether to show the dialog again or not
    _showAgainAction.setChecked(true);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    // Ask for confirmation question
    const auto confirmationQuestion = QString("You made changes to %1 cluster%2, would you like to overwrite them?").arg(QString::number(numberOfUserModifiedClusters), numberOfUserModifiedClusters > 1 ? "s" : "");

    layout->addWidget(new QLabel(confirmationQuestion));

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addWidget(_showAgainAction.createWidget(this));
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_overwriteAction.createWidget(this));
    bottomLayout->addWidget(_cancelAction.createWidget(this));

    layout->addStretch(1);
    layout->addLayout(bottomLayout);

    connect(&_showAgainAction, &ToggleAction::toggled, this, [this](bool toggled) {
        Application::current()->setSetting("OverwriteClustersAskConfirmation", toggled);
    });

    connect(&_overwriteAction, &TriggerAction::triggered, this, &OverwriteClustersConfirmationDialog::accept);
    connect(&_cancelAction, &TriggerAction::triggered, this, &OverwriteClustersConfirmationDialog::reject);
}