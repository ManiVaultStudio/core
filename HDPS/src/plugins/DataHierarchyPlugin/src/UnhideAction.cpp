// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "UnhideAction.h"

#include <Application.h>

#include <widgets/HierarchyWidget.h>

#include <QHeaderView>

using namespace mv;
using namespace mv::gui;

UnhideAction::UnhideAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _listModel(this),
    _filterModel(this),
    _treeAction(this, "List")
{
    setIconByName("eye-slash");
    setShowLabels(false);
    setPopupSizeHint(QSize(300, 300));

    _filterModel.getVisibilityFilterAction().setSelectedOptions({ "Hidden" });

    _treeAction.initialize(&_listModel, &_filterModel, "Hidden dataset");

    addAction(&_treeAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget != nullptr);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->setWindowIcon(Application::getIconFont("FontAwesome").getIcon("database"));

        auto treeView = widget->findChild<QTreeView*>("TreeView");

        Q_ASSERT(treeView != nullptr);

        if (treeView == nullptr)
            return;

        treeView->setRootIsDecorated(false);
        treeView->expandAll();

        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::DatasetId), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::SourceDatasetId), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::Progress), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::GroupIndex), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::IsGroup), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::IsLocked), true);
        treeView->setColumnHidden(static_cast<int>(AbstractDataHierarchyModel::Column::IsDerived), true);

        auto treeViewHeader = treeView->header();

        treeViewHeader->setStretchLastSection(false);

        treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractDataHierarchyModel::Column::Name), QHeaderView::Stretch);
        treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractDataHierarchyModel::Column::IsVisible), QHeaderView::ResizeToContents);

        const auto resizeSections = [this, treeViewHeader]() -> void {
            if (_filterModel.rowCount() >= 1)
                treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
        };

        resizeSections();

        connect(&_filterModel, &QAbstractItemModel::rowsInserted, widget, resizeSections);
        connect(&_filterModel, &QAbstractItemModel::rowsRemoved, widget, resizeSections);
        connect(&_filterModel, &QAbstractItemModel::layoutChanged, widget, resizeSections);
    });
}
