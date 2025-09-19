// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationConfigurationAction.h"

#include "util/StyledIcon.h"

using namespace mv::util;

namespace mv::gui {

ApplicationConfigurationAction::ApplicationConfigurationAction(QObject* parent, const QString& title) :
    GroupsAction(parent, title),
    _configureAction(this, "Configure..."),
    _baseNameAction(this, "Base name"),
    _fullNameAction(this, "Full name"),
    _editFullNameAction(this, "Edit full name"),
    _logoAction(this, "Logo"),
    _brandingAction(this, "Branding"),
    _startPageConfigurationAction(this, "Start page")
{
    _configureAction.setVisible(false);
    _configureAction.setIconByName("gear");

    _logoAction.setDefaultWidgetFlags(ImageAction::WidgetFlag::Loader);

    _editFullNameAction.setChecked(false);

    _brandingAction.addAction(&_brandingAction);
    _brandingAction.addAction(&_fullNameAction);
    _brandingAction.addAction(&_editFullNameAction);
    _brandingAction.addAction(&_logoAction);
    _brandingAction.addAction(&_startPageConfigurationAction);

    addGroupAction(&_brandingAction);
    addGroupAction(&_startPageConfigurationAction);

    connect(&_configureAction, &QAction::triggered, this, [this] {
        QDialog customizeDialog;

        customizeDialog.setWindowTitle("Customize ManiVault Studio");
        customizeDialog.setWindowIcon(StyledIcon("gear"));
        customizeDialog.setMinimumSize(QSize(500, 200));

        auto customizeDialogLayout = new QVBoxLayout();

        customizeDialogLayout->addWidget(createWidget(&customizeDialog));
        customizeDialogLayout->addStretch(1);

        QDialogButtonBox dialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

        dialogButtonBox.button(QDialogButtonBox::Ok)->setText("Save");
        dialogButtonBox.button(QDialogButtonBox::Cancel)->setText("Quit");

        customizeDialogLayout->addWidget(&dialogButtonBox);

        customizeDialog.setLayout(customizeDialogLayout);

        connect(&dialogButtonBox, &QDialogButtonBox::accepted, this, [this, &customizeDialog]() -> void {
            const auto configurationFilePath = Application::getConfigurationFilePath();

            if (!configurationFilePath.isEmpty())
				Application::current()->toJsonFile(configurationFilePath);

        	customizeDialog.accept();
        });

        connect(&dialogButtonBox, &QDialogButtonBox::rejected, &customizeDialog, &QDialog::reject);

        customizeDialog.exec();
	});

    const auto updateFullNameAction = [this]() -> void {
        _fullNameAction.setEnabled(_editFullNameAction.isChecked());

        if (!_editFullNameAction.isChecked())
            _fullNameAction.setString(QString("%1 %2").arg(_baseNameAction.getString(), QString::fromStdString(Application::current()->getVersion().getVersionString())));
	};

    updateFullNameAction();

    connect(&_editFullNameAction, &ToggleAction::toggled, this, updateFullNameAction);
    connect(&_baseNameAction, &StringAction::stringChanged, this, updateFullNameAction);
}

void ApplicationConfigurationAction::fromVariantMap(const QVariantMap& variantMap)
{
    GroupsAction::fromVariantMap(variantMap);

    _baseNameAction.fromParentVariantMap(variantMap, true);
    _fullNameAction.fromParentVariantMap(variantMap, true);
    _editFullNameAction.fromParentVariantMap(variantMap, true);
    _logoAction.fromParentVariantMap(variantMap, true);
}

QVariantMap ApplicationConfigurationAction::toVariantMap() const
{
    auto variantMap = GroupsAction::toVariantMap();

    _baseNameAction.insertIntoVariantMap(variantMap);
    _fullNameAction.insertIntoVariantMap(variantMap);
    _editFullNameAction.insertIntoVariantMap(variantMap);
    _logoAction.insertIntoVariantMap(variantMap);

    return variantMap;
}
}
