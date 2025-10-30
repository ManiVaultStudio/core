// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationConfigurationAction.h"
#include "Application.h"
#include "CoreInterface.h"

#include "util/StyledIcon.h"

#include "models/ProjectsTreeModel.h"

#include <QDialog>
#include <QDialogButtonBox>

using namespace mv::util;

namespace mv::gui {

ApplicationConfigurationAction::ApplicationConfigurationAction(QObject* parent, const QString& title) :
    GroupsAction(parent, title),
    _configureAction(this, "Configure..."),
    _brandingConfigurationAction(this, "Branding"),
    _startPageConfigurationAction(this, "Start page"),
    _projectsConfigurationAction(this, "Projects")
{
    setDefaultWidgetFlags(0);
    
    _configureAction.setVisible(false);
    _configureAction.setIconByName("gear");

    connect(Application::current(), &Application::coreInitialized, this, [this]() {
        _projectsConfigurationAction.addAction(&const_cast<ProjectsTreeModel&>(mv::projects().getProjectsTreeModel()).getDsnsAction());
    });

    addGroupAction(&_brandingConfigurationAction);
    addGroupAction(&_startPageConfigurationAction);
    addGroupAction(&_projectsConfigurationAction);

    connect(&_brandingConfigurationAction.getOrganizationNameAction(), &StringAction::stringChanged, this, [this]() -> void { emit mayConfigureChanged(mayConfigure()); });
    connect(&_brandingConfigurationAction.getOrganizationDomainAction(), &StringAction::stringChanged, this, [this]() -> void { emit mayConfigureChanged(mayConfigure()); });
    connect(&_brandingConfigurationAction.getBaseNameAction(), &StringAction::stringChanged, this, [this]() -> void { emit mayConfigureChanged(mayConfigure()); });

    connect(&_configureAction, &QAction::triggered, this, [this] {
        QDialog customizeDialog;

        customizeDialog.setWindowTitle("Customize ManiVault Studio");
        customizeDialog.setWindowIcon(StyledIcon("gear"));
        customizeDialog.setMinimumSize(QSize(640, 480));

        auto customizeDialogLayout = new QVBoxLayout();

        customizeDialogLayout->addWidget(createWidget(&customizeDialog));

        QDialogButtonBox dialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

        dialogButtonBox.button(QDialogButtonBox::Ok)->setText("Save");
        dialogButtonBox.button(QDialogButtonBox::Cancel)->setText("Quit");

        customizeDialogLayout->addWidget(&dialogButtonBox);

        customizeDialog.setLayout(customizeDialogLayout);

        const auto updateSaveButtonReadOnly = [this, &dialogButtonBox]() -> void {
            dialogButtonBox.button(QDialogButtonBox::Ok)->setEnabled(mayConfigure());
        };

        updateSaveButtonReadOnly();

        connect(this, &ApplicationConfigurationAction::mayConfigureChanged, this, updateSaveButtonReadOnly);

        connect(&dialogButtonBox, &QDialogButtonBox::accepted, this, [this, &customizeDialog]() -> void {
            const auto configurationFilePath = Application::getConfigurationFilePath();

            if (!configurationFilePath.isEmpty())
				Application::current()->toJsonFile(configurationFilePath);

        	//customizeDialog.accept();
        });

        connect(&dialogButtonBox, &QDialogButtonBox::rejected, &customizeDialog, &QDialog::reject);

        customizeDialog.exec();
	});
}

void ApplicationConfigurationAction::fromVariantMap(const QVariantMap& variantMap)
{
    GroupsAction::fromVariantMap(variantMap);

    _brandingConfigurationAction.fromParentVariantMap(variantMap, true);
    _startPageConfigurationAction.fromParentVariantMap(variantMap, true);
}

QVariantMap ApplicationConfigurationAction::toVariantMap() const
{
    auto variantMap = GroupsAction::toVariantMap();

    _brandingConfigurationAction.insertIntoVariantMap(variantMap);
    _startPageConfigurationAction.insertIntoVariantMap(variantMap);

    mv::projects().getProjectsTreeModel().getDsnsAction().insertIntoVariantMap(variantMap);

    return variantMap;
}

bool ApplicationConfigurationAction::mayConfigure()
{
    if (_brandingConfigurationAction.getOrganizationNameAction().getString().isEmpty())
        return false;

    if (_brandingConfigurationAction.getOrganizationDomainAction().getString().isEmpty())
        return false;

    if (_brandingConfigurationAction.getBaseNameAction().getString().isEmpty())
        return false;

    return true;
}

}
