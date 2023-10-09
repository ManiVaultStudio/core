// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageActionsWidget.h"
#include "StartPageActionDelegate.h"
#include "StartPageWidget.h"

#include <Application.h>

#include <QDebug>
#include <QHeaderView>

#ifdef _DEBUG
    //#define START_PAGE_ACTIONS_WIDGET_VERBOSE
#endif

using namespace hdps;
using namespace hdps::gui;

StartPageActionsWidget::StartPageActionsWidget(QWidget* parent, const QString& title, bool restyle /*= true*/) :
    QWidget(parent),
    _layout(),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Item", _model, &_filterModel, true, true),
    _restyle(restyle)
{
    if (!title.isEmpty())
        _layout.addWidget(StartPageContentWidget::createHeaderLabel(title, title));

    _layout.addWidget(&_hierarchyWidget, 1);

    setLayout(&_layout);

    _filterModel.setFilterKeyColumn(static_cast<int>(StartPageActionsModel::Column::Title));

    _hierarchyWidget.getFilterGroupAction().setVisible(false);
    _hierarchyWidget.getCollapseAllAction().setVisible(false);
    _hierarchyWidget.getExpandAllAction().setVisible(false);
    _hierarchyWidget.getColumnsGroupAction().setVisible(false);
    _hierarchyWidget.getSelectionGroupAction().setVisible(false);
    _hierarchyWidget.setHeaderHidden(true);
    
    _hierarchyWidget.getInfoOverlayWidget()->setBackgroundColor(Qt::transparent);
    
    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setRootIsDecorated(false);
    treeView.setItemDelegateForColumn(static_cast<int>(StartPageActionsModel::Column::SummaryDelegate), new StartPageActionDelegate());
    treeView.setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView.setSelectionMode(QAbstractItemView::SingleSelection);
    treeView.setIconSize(QSize(24, 24));
    treeView.setMouseTracking(true);
    treeView.setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Icon), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Title), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Description), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Comments), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Tags), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Subtitle), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::MetaData), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::PreviewImage), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Tooltip), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Contributors), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::ClickedCallback), true);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->setSectionResizeMode(static_cast<int>(StartPageActionsModel::Column::SummaryDelegate), QHeaderView::Stretch);
    
    updateCustomStyle();

    connect(&treeView, &QTreeView::clicked, this, [this](const QModelIndex& index) -> void {
        auto callback = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::ClickedCallback)).data(Qt::UserRole + 1).value<StartPageAction::ClickedCallback>();
        callback();

        _hierarchyWidget.getSelectionModel().clear();
    });

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, [this](const QModelIndex& parent, int first, int last) -> void {
        for (int rowIndex = first; rowIndex <= last; rowIndex++)
            openPersistentEditor(rowIndex);
    });

    connect(&_filterModel, &QSortFilterProxyModel::rowsAboutToBeRemoved, this, [this](const QModelIndex& parent, int first, int last) -> void {
        for (int rowIndex = first; rowIndex <= last; rowIndex++)
            closePersistentEditor(rowIndex);
    });
    
    connect(qApp, &QApplication::paletteChanged, this, &StartPageActionsWidget::updateCustomStyle);
}

QVBoxLayout& StartPageActionsWidget::getLayout()
{
    return _layout;
}

StartPageActionsModel& StartPageActionsWidget::getModel()
{
    return _model;
}

StartPageActionsFilterModel& StartPageActionsWidget::getFilterModel()
{
    return _filterModel;
}

HierarchyWidget& StartPageActionsWidget::getHierarchyWidget()
{
    return _hierarchyWidget;
}

void StartPageActionsWidget::openPersistentEditor(int rowIndex)
{
    const auto index = _filterModel.index(rowIndex, static_cast<int>(StartPageActionsModel::Column::SummaryDelegate));

    if (_hierarchyWidget.getTreeView().isPersistentEditorOpen(index))
        return;

    if (index.isValid()) {
#ifdef START_PAGE_ACTIONS_WIDGET_VERBOSE
        qDebug() << __FUNCTION__ << index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Title)).data().toString() << index << index.isValid();
#endif

        _hierarchyWidget.getTreeView().openPersistentEditor(index);
    }
}

void StartPageActionsWidget::closePersistentEditor(int rowIndex)
{
    const auto index = _filterModel.index(rowIndex, static_cast<int>(StartPageActionsModel::Column::SummaryDelegate));

    if (!_hierarchyWidget.getTreeView().isPersistentEditorOpen(index))
        return;

    if (index.isValid()) {
#ifdef START_PAGE_ACTIONS_WIDGET_VERBOSE
        qDebug() << __FUNCTION__ << index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Title)).data().toString() << index;
#endif

        _hierarchyWidget.getTreeView().closePersistentEditor(index);
    }
}

void StartPageActionsWidget::updateCustomStyle()
{
    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("search"));
    
    auto styleSheet = QString(" \
        QLabel { \
            background-color: rgba(0, 0, 0, 0); \
        } \
        QTreeView::item:hover:!selected { \
            background-color: rgba(0, 0, 0, 50); \
        } \
        QTreeView::item:selected { \
            background-color: rgba(0, 0, 0, 100); \
        } \
    ");
    
    auto& treeView = _hierarchyWidget.getTreeView();
    if (_restyle) {
        styleSheet += QString(" \
            QTreeView { \
                border: none; \
            } \
        ");
    }
    
    auto color = QApplication::palette().color(QPalette::Normal, QPalette::Midlight).name();
    styleSheet += QString("QTreeView { background-color: %1;}").arg(color);
    treeView.setStyleSheet(styleSheet);
}
