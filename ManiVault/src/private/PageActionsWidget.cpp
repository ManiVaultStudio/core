// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageActionsWidget.h"

#include "PageAction.h"
#include "PageContentWidget.h"
#include "PageActionsModel.h"
#include "PageActionDelegate.h"

#include <Application.h>

#include <QDebug>
#include <QHeaderView>

#ifdef _DEBUG
    //#define PAGE_ACTIONS_WIDGET_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

PageActionsWidget::PageActionsWidget(QWidget* parent, const QString& title, bool restyle /*= true*/) :
    QWidget(parent),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Item", _model, &_filterModel, true, true),
    _restyle(restyle)
{
    if (!title.isEmpty())
        _layout.addWidget(PageContentWidget::createHeaderLabel(title, title));

    _layout.addWidget(&_hierarchyWidget, 1);
    _layout.setContentsMargins(0, 0, 0, 0);

    setLayout(&_layout);

    _filterModel.setFilterKeyColumn(static_cast<int>(PageActionsModel::Column::Title));

    _hierarchyWidget.getFilterGroupAction().setVisible(false);
    _hierarchyWidget.getCollapseAllAction().setVisible(false);
    _hierarchyWidget.getExpandAllAction().setVisible(false);
    _hierarchyWidget.getColumnsGroupAction().setVisible(false);
    _hierarchyWidget.getSelectionGroupAction().setVisible(false);
    _hierarchyWidget.setHeaderHidden(true);
    
    _hierarchyWidget.getInfoOverlayWidget()->setBackgroundColor(Qt::transparent);
    
    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setRootIsDecorated(false);
    treeView.setItemDelegateForColumn(static_cast<int>(PageActionsModel::Column::SummaryDelegate), new PageActionDelegate());
    treeView.setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView.setSelectionMode(QAbstractItemView::SingleSelection);
    treeView.setIconSize(QSize(24, 24));
    treeView.setMouseTracking(true);
    treeView.setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    treeView.setColumnHidden(static_cast<int>(PageActionsModel::Column::Icon), true);
    treeView.setColumnHidden(static_cast<int>(PageActionsModel::Column::Title), true);
    treeView.setColumnHidden(static_cast<int>(PageActionsModel::Column::Description), true);
    treeView.setColumnHidden(static_cast<int>(PageActionsModel::Column::Comments), true);
    treeView.setColumnHidden(static_cast<int>(PageActionsModel::Column::Tags), true);
    treeView.setColumnHidden(static_cast<int>(PageActionsModel::Column::Subtitle), true);
    treeView.setColumnHidden(static_cast<int>(PageActionsModel::Column::MetaData), true);
    treeView.setColumnHidden(static_cast<int>(PageActionsModel::Column::PreviewImage), true);
    treeView.setColumnHidden(static_cast<int>(PageActionsModel::Column::Tooltip), true);
    treeView.setColumnHidden(static_cast<int>(PageActionsModel::Column::DownloadUrls), true);
    treeView.setColumnHidden(static_cast<int>(PageActionsModel::Column::Contributors), true);
    treeView.setColumnHidden(static_cast<int>(PageActionsModel::Column::ClickedCallback), true);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->setSectionResizeMode(static_cast<int>(PageActionsModel::Column::SummaryDelegate), QHeaderView::Stretch);

    updateCustomStyle();

    //connect(&mv::theme(), &mv::AbstractThemeManager::colorSchemeChanged, this, &PageActionsWidget::updateCustomStyle);

    connect(&treeView, &QTreeView::clicked, this, [this](const QModelIndex& index) -> void {
        auto callback = index.siblingAtColumn(static_cast<int>(PageActionsModel::Column::ClickedCallback)).data(Qt::UserRole + 1).value<PageAction::ClickedCallback>();
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

    treeView.setBackgroundRole(QPalette::Window);
}

QVBoxLayout& PageActionsWidget::getLayout()
{
    return _layout;
}

PageActionsModel& PageActionsWidget::getModel()
{
    return _model;
}

PageActionsFilterModel& PageActionsWidget::getFilterModel()
{
    return _filterModel;
}

HierarchyWidget& PageActionsWidget::getHierarchyWidget()
{
    return _hierarchyWidget;
}

void PageActionsWidget::openPersistentEditor(int rowIndex)
{
    const auto index = _filterModel.index(rowIndex, static_cast<int>(PageActionsModel::Column::SummaryDelegate));

    if (_hierarchyWidget.getTreeView().isPersistentEditorOpen(index))
        return;

    if (index.isValid()) {
#ifdef PAGE_ACTIONS_WIDGET_VERBOSE
        qDebug() << __FUNCTION__ << index.siblingAtColumn(static_cast<int>(PageActionsModel::Column::Title)).data().toString() << index << index.isValid();
#endif

        _hierarchyWidget.getTreeView().openPersistentEditor(index);
    }
}

void PageActionsWidget::closePersistentEditor(int rowIndex)
{
    const auto index = _filterModel.index(rowIndex, static_cast<int>(PageActionsModel::Column::SummaryDelegate));

    if (!_hierarchyWidget.getTreeView().isPersistentEditorOpen(index))
        return;

    if (index.isValid()) {
#ifdef PAGE_ACTIONS_WIDGET_VERBOSE
        qDebug() << __FUNCTION__ << index.siblingAtColumn(static_cast<int>(PageActionsModel::Column::Title)).data().toString() << index;
#endif

        _hierarchyWidget.getTreeView().closePersistentEditor(index);
    }
}

void PageActionsWidget::updateCustomStyle()
{
    _hierarchyWidget.setWindowIcon(StyledIcon("magnifying-glass"));

    auto rowHoverColor = qApp->palette().color(QPalette::Normal, QPalette::Text);

    rowHoverColor.setAlpha(50);

    auto styleSheet = QString(
        "QLabel {"
            "background-color: rgba(0, 0, 0, 0);"
        "}"
        "QTreeView::item {"
			"border-radius: 5px;"
        "}"
        "QTreeView::item:hover:!selected {"
            "background-color: %1;"
        "}"
        "QTreeView::item:selected {"
			"background-color: rgba(0, 0, 0, 100);"
        "}"
    ).arg(rowHoverColor.name(QColor::HexArgb));
    
    auto& treeView = _hierarchyWidget.getTreeView();

    if (_restyle) {
        styleSheet += QString(" \
            QTreeView { \
                border: none; \
            } \
        ");
    }
    
    auto color = qApp->palette().color(QPalette::Normal, QPalette::Window).name();

	styleSheet += QString("QTreeView { background-color: %1;}").arg(color);

	treeView.setStyleSheet(styleSheet);
}
