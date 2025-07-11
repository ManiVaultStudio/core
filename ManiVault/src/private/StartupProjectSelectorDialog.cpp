// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartupProjectSelectorDialog.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QTextBrowser>
#include <QToolTip>
#include <QTreeView>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    #define STARTUP_PROJECT_SELECTOR_DIALOG_VERBOSE
#endif

StartupProjectSelectorDialog::StartupProjectSelectorDialog(mv::ProjectsTreeModel& projectsTreeModel, mv::ProjectsFilterModel& projectsFilterModel, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _projectsTreeModel(projectsTreeModel),
    _projectsFilterModel(projectsFilterModel),
    _hierarchyWidget(this, "Startup project", _projectsTreeModel, &_projectsFilterModel, true),
    _loadAction(this, "Load"),
    _quitAction(this, "Quit")
{
    const auto windowIcon = StyledIcon("file-import");

    setWindowIcon(windowIcon);
    setModal(true);
    setWindowTitle("Load project");

    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget, 1);

	auto bottomLayout = new QHBoxLayout();

    bottomLayout->addWidget(_loadAction.createWidget(this));
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_quitAction.createWidget(this));

    layout->addLayout(bottomLayout);

    setLayout(layout);

    _hierarchyWidget.getFilterGroupAction().addAction(&_projectsFilterModel.getFilterStartupOnlyAction());

	_hierarchyWidget.setWindowIcon(windowIcon);
    _hierarchyWidget.getTreeView().setRootIsDecorated(true);

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    treeView.setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

    connect(&treeView, &QTreeView::doubleClicked, this, [this](const QModelIndex& index) {
        if (getSelectedStartupProject()->isGroup())
            return; // Do not load group items

        _loadAction.trigger();
    });

    const auto updateLoadAction = [this, &treeView]() -> void {
        const auto canLoad = treeView.selectionModel()->selectedRows().isEmpty();

        _loadAction.setText(canLoad ? "Start ManiVault" : "Load Project");
        _loadAction.setToolTip(canLoad ? "Start ManiVault" : "Load the selected project");
    };

    updateLoadAction();

    connect(treeView.selectionModel(), &QItemSelectionModel::selectionChanged, this, updateLoadAction);
    
    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(true);

    treeViewHeader->resizeSection(static_cast<int>(ProjectsTreeModel::Column::Title), 250);

    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::Downloaded), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::Group), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::IsGroup), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::IconName), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::Tags), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::Date), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::Summary), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::Url), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::IsStartup), true);
    //treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::RecommendedHardwareSpec), true);

#if QT_NO_DEBUG
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::MinimumCoreVersion), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::RequiredPlugins), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::MissingPlugins), true);
#endif

    connect(&_loadAction, &TriggerAction::triggered, this, [this]() -> void {
        if (auto selectedStartupProject = getSelectedStartupProject()) {
            const auto downloadedProjectFilePath = mv::projects().downloadProject(selectedStartupProject->getUrl());

	        if (!downloadedProjectFilePath.isEmpty()) {
                Application::current()->setStartupProjectUrl(QUrl(QString("file:///%1").arg(downloadedProjectFilePath)));

	        	accept();
			}
        }
    });

    connect(&_quitAction, &TriggerAction::triggered, this, &StartupProjectSelectorDialog::reject);
}

std::int32_t StartupProjectSelectorDialog::getSelectedStartupProjectIndex()
{
    auto selectedRows = _hierarchyWidget.getTreeView().selectionModel()->selectedRows();

    if (selectedRows.isEmpty())
        return -1;

    return selectedRows.first().row();
}

ProjectsModelProject* StartupProjectSelectorDialog::getSelectedStartupProject() const
{
    const auto selectedRows = _hierarchyWidget.getSelectedRows();

    if (selectedRows.isEmpty())
        return {};

    const auto& firstSelectedFilterRow = selectedRows.first();

	if (auto projectsModelProject = _projectsTreeModel.getProject(firstSelectedFilterRow))
		return const_cast<mv::util::ProjectsModelProject*>(projectsModelProject);

	return {};
}
