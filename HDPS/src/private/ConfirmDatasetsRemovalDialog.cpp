// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ConfirmDatasetsRemovalDialog.h"

#include <Application.h>

#include <QVBoxLayout>
#include <QLabel>

using namespace mv;

ConfirmDatasetsRemovalDialog::ConfirmDatasetsRemovalDialog(QWidget* parent) :
    QDialog(parent),
    _removeAction(this, "Remove"),
    _cancelAction(this, "Cancel"),
    _buttonsGroupAction(this, "Buttons group")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("trash"));
    setWindowTitle("About to remove dataset(s)");
    setModal(true);

    //// Check whether to show the dialog again or not
    //_alwaysAskPermissionAction.setChecked(true);

    _buttonsGroupAction.addStretch();
    _buttonsGroupAction.addAction(&_removeAction);
    _buttonsGroupAction.addAction(&_cancelAction);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    //// Ask for confirmation question
    //const auto confirmationQuestion = QString("You made changes to %1 cluster%2, would you like to discard these changes?").arg(QString::number(numberOfUserModifiedClusters), numberOfUserModifiedClusters > 1 ? "s" : "");

    //layout->addWidget(new QLabel(confirmationQuestion));

    layout->addStretch(1);
    layout->addWidget(_buttonsGroupAction.createWidget(this));

    //connect(&_alwaysAskPermissionAction, &ToggleAction::toggled, this, [this](bool toggled) {
    //    Application::current()->setSetting("OverwriteClustersAskConfirmation", toggled);
    //});

    //connect(&_discardAction, &TriggerAction::triggered, this, &OverwriteClustersConfirmationDialog::accept);
    //connect(&_cancelAction, &TriggerAction::triggered, this, &OverwriteClustersConfirmationDialog::reject);
}