// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationConfigurationAction.h"

#include "util/StyledIcon.h"

using namespace mv::util;

namespace mv::gui {

ApplicationConfigurationAction::ApplicationConfigurationAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _configureAction(this, "Configure..."),
    _nameAction(this, "Name", "ManiVault Studio")
{
    _configureAction.setVisible(false);
    _configureAction.setIconByName("gear");

    addAction(&_nameAction);

    connect(&_configureAction, &QAction::triggered, this, [this] {
        QDialog customizeDialog;

        customizeDialog.setWindowTitle("Customize ManiVault Studio");
        customizeDialog.setWindowIcon(StyledIcon("gear"));
        customizeDialog.setMinimumSize(QSize(600, 400));

        auto customizeGroup = new GroupAction(&customizeDialog, "Settings");

        customizeGroup->addAction(&_nameAction);

        auto customizeDialogLayout = new QVBoxLayout();

        customizeDialogLayout->addWidget(customizeGroup->createWidget(&customizeDialog));

        QDialogButtonBox dialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

        dialogButtonBox.button(QDialogButtonBox::Ok)->setText("Save");
        dialogButtonBox.button(QDialogButtonBox::Cancel)->setText("Quit");

        customizeDialogLayout->addWidget(&dialogButtonBox);

        customizeDialog.setLayout(customizeDialogLayout);

        connect(&dialogButtonBox, &QDialogButtonBox::accepted, this, [this, &customizeDialog]() -> void {
            Application::current()->toJsonFile();

        	customizeDialog.accept();
        });

        connect(&dialogButtonBox, &QDialogButtonBox::rejected, &customizeDialog, &QDialog::reject);

        customizeDialog.exec();
	});
}

}
