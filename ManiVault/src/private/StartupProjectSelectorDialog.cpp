// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartupProjectSelectorDialog.h"

#include <models/HardwareSpecTreeModel.h>

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QTreeView>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    #define STARTUP_PROJECT_SELECTOR_DIALOG_VERBOSE
#endif

class CustomTooltipPopup : public QFrame {
public:
    explicit CustomTooltipPopup(QWidget* parent = nullptr)
        : QFrame(parent, Qt::ToolTip)
    {
        setWindowFlags(Qt::ToolTip);
        setFrameShape(QFrame::Box);

        _hardwareSpecTreeModel.setHeaderData(0, Qt::Horizontal, "Name");
        _hardwareSpecTreeModel.setHeaderData(1, Qt::Horizontal, "System");
        _hardwareSpecTreeModel.setHeaderData(2, Qt::Horizontal, "Minimum");

        _hardwareSpecTreeView.setModel(&_hardwareSpecTreeModel);
        //_hardwareSpecTreeView.setHeaderHidden(true);
        _hardwareSpecTreeView.setRootIsDecorated(false);
        _hardwareSpecTreeView.setIconSize(QSize(12, 12));
        _hardwareSpecTreeView.setMinimumWidth(400);

    	auto header = _hardwareSpecTreeView.header();

        header->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);

        auto layout = new QVBoxLayout(this);
        
        layout->addWidget(&_hardwareSpecTreeView);

        setLayout(layout);
    }

    void setHardwareSpec(const HardwareSpec& hardwareSpec) {
        _hardwareSpecTreeModel.setHardwareSpec(hardwareSpec);
        _hardwareSpecTreeView.expandAll();
    }

    //QSize sizeHint() const override {
    //    return _hardwareSpecTreeView.sizeHint();
    //}

private:
    HardwareSpecTreeModel   _hardwareSpecTreeModel;     /** Model for the hardware specification */
    QTreeView               _hardwareSpecTreeView;      /** Visualization of the hardware specification */
};

class ItemViewTooltipInterceptor : public QObject {
public:
    ItemViewTooltipInterceptor(ProjectsTreeModel* projectsTreeModel, ProjectsFilterModel* projectsFilterModel, QAbstractItemView* view) :
		QObject(view),
        _projectsTreeModel(projectsTreeModel),
        _projectsFilterModel(projectsFilterModel),
		_projectsView(view),
		_popup(new CustomTooltipPopup(view))
    {
        _projectsView->viewport()->installEventFilter(this);
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override {
        if (event->type() == QEvent::ToolTip) {
            const auto helpEvent        = dynamic_cast<QHelpEvent*>(event);
            const auto index            = _projectsFilterModel->mapToSource(_projectsView->indexAt(helpEvent->pos()));
            const auto column           = static_cast<mv::ProjectsTreeModel::Column>(index.column());
            const auto projectItem      = dynamic_cast<ProjectsTreeModel::Item*>(_projectsTreeModel->itemFromIndex(index));

        	if (index.isValid() && projectItem) {
                if (column == ProjectsTreeModel::Column::MinimumHardwareSpec)
					_popup->setHardwareSpec(projectItem->getProject()->getMinimumHardwareSpec());

                if (column == ProjectsTreeModel::Column::RecommendedHardwareSpec)
                    _popup->setHardwareSpec(projectItem->getProject()->getRecommendedHardwareSpec());

				_popup->move(helpEvent->globalPos() + QPoint(10, 20));
                _popup->show();

                return true; // block default tooltip
            }
        }
        else if (event->type() == QEvent::Leave) {
            _popup->hide();
        }
        return QObject::eventFilter(watched, event);
    }

private:
    ProjectsTreeModel*      _projectsTreeModel;     /** Pointer to the projects tree model */
    ProjectsFilterModel*    _projectsFilterModel;   /** Pointer to the projects filter model */
    QAbstractItemView*      _projectsView;          /** Pointer to the projects view */
    CustomTooltipPopup*     _popup;                 /** Custom tooltip popup for displaying hardware specifications */
};

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

    new ItemViewTooltipInterceptor(&_projectsTreeModel, &_projectsFilterModel, &_hierarchyWidget.getTreeView());

    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget, 1);

    //auto detailsGroupbox = new QGroupBox("Project details", this);
    //auto detailsLayout = new QVBoxLayout(detailsGroupbox);
    //auto detailsLabel = new QLabel("Select a project to see its details here.", detailsGroupbox);

    //detailsLayout->addWidget(detailsLabel);

    //detailsGroupbox->setLayout(detailsLayout);

    //layout->addWidget(detailsGroupbox);

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
        _loadAction.setText(treeView.selectionModel()->selectedRows().isEmpty() ? "Start ManiVault" : "Load Project");
        _loadAction.setToolTip(treeView.selectionModel()->selectedRows().isEmpty() ? "Start ManiVault" : "Load the selected project");
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

    connect(&_loadAction, &TriggerAction::triggered, this, &StartupProjectSelectorDialog::accept);
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
