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
    _brandingConfigurationAction(this, "Branding"),
    _startPageConfigurationAction(this, "Start page"),
    _projectsGroupAction(this, "Projects")
{
    setDefaultWidgetFlags(0);
    
    _configureAction.setVisible(false);
    _configureAction.setIconByName("gear");

    connect(Application::current(), &Application::coreInitialized, this, [this]() {
        _projectsGroupAction.addAction(&const_cast<ProjectsTreeModel&>(mv::projects().getProjectsTreeModel()).getDsnsAction());
    });

    addGroupAction(&_brandingConfigurationAction);
    addGroupAction(&_startPageConfigurationAction);
    addGroupAction(&_projectsGroupAction);

    connect(&_configureAction, &QAction::triggered, this, [this] {
        QDialog customizeDialog;

        customizeDialog.setWindowTitle("Customize ManiVault Studio");
        customizeDialog.setWindowIcon(StyledIcon("gear"));
        customizeDialog.setMinimumSize(QSize(600, 300));

        auto customizeDialogLayout = new QVBoxLayout();

        customizeDialogLayout->addWidget(createWidget(&customizeDialog));

        QDialogButtonBox dialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

        dialogButtonBox.button(QDialogButtonBox::Ok)->setText("Save");
        dialogButtonBox.button(QDialogButtonBox::Cancel)->setText("Quit");

        customizeDialogLayout->addWidget(&dialogButtonBox);

        customizeDialog.setLayout(customizeDialogLayout);

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

    return variantMap;
}

}
