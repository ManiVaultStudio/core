// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartupProjectSelectorDialog.h"

#include <Application.h>

#include <QVBoxLayout>
#include <QHeaderView>

using namespace mv;
using namespace mv::gui;

#ifdef _DEBUG
    #define STARTUP_PROJECT_SELECTOR_DIALOG_VERBOSE
#endif

StartupProjectSelectorDialog::StartupProjectSelectorDialog(const QVector<QPair<QSharedPointer<mv::ProjectMetaAction>, QString>>& startupProjectsMetaActions, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Startup project", _model, &_filterModel, false),
    _loadAction(this, "Load"),
    _cancelAction(this, "Cancel")
{
    _model.initialize(startupProjectsMetaActions);

    const auto windowIcon = Application::getIconFont("FontAwesome").getIcon("file-import");

    setWindowIcon(windowIcon);
    setModal(true);
    setWindowTitle("Load project");
    
    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget, 1);

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_loadAction.createWidget(this));
    bottomLayout->addWidget(_cancelAction.createWidget(this));

    layout->addLayout(bottomLayout);

    setLayout(layout);

    _loadAction.setToolTip("Load the selected project");
    _cancelAction.setToolTip("Do not load a project");

    _hierarchyWidget.setWindowIcon(windowIcon);
    _hierarchyWidget.getTreeView().setRootIsDecorated(false);

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    treeView.setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

    const auto updateLoadAction = [this, &treeView]() -> void {
        _loadAction.setEnabled(!treeView.selectionModel()->selectedRows().isEmpty());
    };

    updateLoadAction();

    connect(treeView.selectionModel(), &QItemSelectionModel::selectionChanged, this, updateLoadAction);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    //treeViewHeader->resizeSection(static_cast<int>(StartupProjectsModel::Column::FileName), 300);
    //treeViewHeader->resizeSection(static_cast<int>(StartupProjectsModel::Column::Title), 300);
    //treeViewHeader->resizeSection(static_cast<int>(StartupProjectsModel::Column::Description), 70);

#ifdef _DEBUG
    treeViewHeader->setSectionHidden(static_cast<int>(StartupProjectsModel::Column::FileName), false);
#else
    treeViewHeader->setSectionHidden(static_cast<int>(StartupProjectsModel::Column::FileName), true);
#endif
    

    treeViewHeader->setSectionResizeMode(static_cast<int>(StartupProjectsModel::Column::FileName), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(StartupProjectsModel::Column::Title), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(StartupProjectsModel::Column::Description), QHeaderView::Stretch);

    treeViewHeader->setStretchLastSection(false);

    connect(&_loadAction, &TriggerAction::triggered, this, &StartupProjectSelectorDialog::accept);
    connect(&_cancelAction, &TriggerAction::triggered, this, &StartupProjectSelectorDialog::reject);
}

std::int32_t StartupProjectSelectorDialog::getSelectedStartupProjectIndex()
{
    auto selectedRows = _hierarchyWidget.getTreeView().selectionModel()->selectedRows();

    if (selectedRows.isEmpty())
        return -1;

    return selectedRows.first().row();
}