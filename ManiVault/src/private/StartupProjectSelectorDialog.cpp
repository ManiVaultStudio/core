// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartupProjectSelectorDialog.h"

#include <models/HardwareSpecTreeModel.h>

#include <actions/TaskAction.h>

#include <CoreInterface.h>

#include <QVBoxLayout>
#include <QHeaderView>
#include <QTextBrowser>
#include <QToolTip>
#include <QTreeView>
#include <QDialogButtonBox>

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
    _projectDownloadTask(this, "Project download"),
    _loadAction(this, "Load"),
    _quitAction(this, "Quit")
{
    const auto windowIcon = StyledIcon("file-import");

    setWindowIcon(windowIcon);
    setModal(true);
    setWindowTitle("Select a project to load");

    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget, 1);

    auto taskAction = new TaskAction(this, "Loading project...");

    taskAction->setTask(&_projectDownloadTask);

    auto downloadTaskWidget = taskAction->createWidget(this);

    //auto sizePolicy = downloadTaskWidget->sizePolicy();

    //sizePolicy.setRetainSizeWhenHidden(true);

    //downloadTaskWidget->setSizePolicy(sizePolicy);

	const auto updateDownloadTaskWidgetVisibility = [this, downloadTaskWidget]() -> void {
        downloadTaskWidget->setVisible(_projectDownloadTask.isRunning());
    };

    updateDownloadTaskWidgetVisibility();

    connect(&_projectDownloadTask, &Task::statusChanged, this, updateDownloadTaskWidgetVisibility);

    layout->addWidget(downloadTaskWidget);

	auto bottomLayout = new QHBoxLayout();

    bottomLayout->addWidget(_loadAction.createWidget(this));
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_quitAction.createWidget(this));

    layout->addLayout(bottomLayout);

    setLayout(layout);

    _hierarchyWidget.getFilterGroupAction().addAction(&_projectsFilterModel.getFilterStartupOnlyAction());
    _hierarchyWidget.getFilterGroupAction().addAction(&_projectsFilterModel.getFilterLoadableOnlyAction());

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
        auto mayLoad = false;

        if (const auto selectedStartupProject   = getSelectedStartupProject()) {
            mayLoad = !selectedStartupProject->isGroup();
        }
        _loadAction.setEnabled(mayLoad);
        _loadAction.setText(mayLoad ? "Load Project" : "Select Project");
        _loadAction.setToolTip(mayLoad ? "Click to load the project" : "Select a project to load");
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
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::Sha), true);

#if QT_NO_DEBUG
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::LastModified), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::MinimumCoreVersion), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::RequiredPlugins), true);
    treeViewHeader->setSectionHidden(static_cast<int>(ProjectsTreeModel::Column::MissingPlugins), true);
#endif

    connect(&_loadAction, &TriggerAction::triggered, this, [this]() -> void {
        if (auto selectedStartupProject = getSelectedStartupProject()) {
            bool loadStartupProject = true;

            _projectDownloadTask.setName(QString("Downloading %1").arg(selectedStartupProject->getTitle()));

            const auto downloadedProjectFilePath = mv::projects().downloadProject(selectedStartupProject->getUrl(), "", &_projectDownloadTask);

            const auto systemCompatibility  = HardwareSpec::getSystemCompatibility(selectedStartupProject->getMinimumHardwareSpec(), selectedStartupProject->getRecommendedHardwareSpec());
            const auto notCompatible        = systemCompatibility._compatibility == HardwareSpec::SystemCompatibility::Incompatible;
            const auto notRecommended       = systemCompatibility._compatibility == HardwareSpec::SystemCompatibility::Minimum;

            if (notCompatible || notRecommended) {

                QDialog projectIncompatibleWithSystemDialog;

                projectIncompatibleWithSystemDialog.setWindowIcon(StyledIcon("triangle-exclamation"));
                projectIncompatibleWithSystemDialog.setWindowTitle("Incompatible System");
                projectIncompatibleWithSystemDialog.setMinimumWidth(500);
                projectIncompatibleWithSystemDialog.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

                auto layout     = new QVBoxLayout(&projectIncompatibleWithSystemDialog);
                auto message    = new QLabel();

                layout->setSpacing(10);

                message->setWordWrap(true);

                if (notCompatible)
                    message->setText("<p>Your system does not meet the minimum requirements for this project, there might be problems with opening it, its stability and performance!</p>");

                if (notRecommended)
                    message->setText("<p>Your system does not meet the recommended requirements for this project, the interactivity might not be optimal!</p>");

                layout->addWidget(message);

                auto requirementsLayout     = new QVBoxLayout();
                auto models                 = QList<HardwareSpecTreeModel*>({ new HardwareSpecTreeModel(&projectIncompatibleWithSystemDialog), new HardwareSpecTreeModel(&projectIncompatibleWithSystemDialog) });
                auto treeViews              = QList<QTreeView*>({ new QTreeView(), new QTreeView() });
                auto recommendedCheckBox    = new QCheckBox("Show recommended");

                recommendedCheckBox->setChecked(notRecommended);

                requirementsLayout->setSpacing(5);

                models.first()->setHardwareSpec(selectedStartupProject->getMinimumHardwareSpec());
                models.first()->setHorizontalHeaderLabels({ "Component", "System", "Minimum" });

                models.last()->setHardwareSpec(selectedStartupProject->getRecommendedHardwareSpec());
                models.last()->setHorizontalHeaderLabels({ "Component", "System", "Recommended" });

                for (auto treeView : treeViews) {
                    treeView->setIconSize(QSize(13, 13));
                    treeView->setModel(models[treeViews.indexOf(treeView)]);
                    treeView->expandAll();

                    treeView->header()->setStretchLastSection(false);
                    treeView->header()->setSectionResizeMode(QHeaderView::Interactive);
                    treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
                    treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeMode::Fixed);
                    treeView->header()->setSectionResizeMode(2, QHeaderView::ResizeMode::Fixed);

                    treeView->header()->resizeSection(1, 100);
                    treeView->header()->resizeSection(2, 100);

                    requirementsLayout->addWidget(treeView);
                }

                const auto updateTreeViewVisibility = [treeViews, recommendedCheckBox]() -> void {
                    treeViews.first()->setVisible(!recommendedCheckBox->isChecked());
                    treeViews.last()->setVisible(recommendedCheckBox->isChecked());
                };

                updateTreeViewVisibility();

                connect(recommendedCheckBox, &QCheckBox::toggled, this, updateTreeViewVisibility);

                requirementsLayout->addWidget(recommendedCheckBox);

                layout->addLayout(requirementsLayout, 1);
                layout->addWidget(new QLabel("<p>Do you want to continue anyway?</p>"));

            	auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::Abort);

                connect(dialogButtonBox->button(QDialogButtonBox::StandardButton::Yes), &QPushButton::clicked, &projectIncompatibleWithSystemDialog, &QDialog::accept);
                connect(dialogButtonBox->button(QDialogButtonBox::StandardButton::Abort), &QPushButton::clicked, &projectIncompatibleWithSystemDialog, &QDialog::reject);

                layout->addWidget(dialogButtonBox);

                projectIncompatibleWithSystemDialog.setLayout(layout);

                if (projectIncompatibleWithSystemDialog.exec() == QDialog::Rejected)
                    loadStartupProject = false;
            }

	        if (loadStartupProject && !downloadedProjectFilePath.isEmpty()) {
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
