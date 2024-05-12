// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideosWidget.h"
#include "LearningPageContentWidget.h"
#include "LearningPageVideoStyledItemDelegate.h"

#include <Application.h>

#include <QDebug>
#include <QStyledItemDelegate>

using namespace mv::gui;

LearningPageVideosWidget::LearningPageVideosWidget(LearningPageContentWidget* learningPageContentWidget) :
    QWidget(learningPageContentWidget),
    _learningPageContentWidget(learningPageContentWidget),
    _mainLayout(),
    _model(),
    _filterModel(),
    _hierarchyWidget(this, "Video", _model, &_filterModel)
{
    _mainLayout.setSpacing(20);
    _mainLayout.addWidget(PageContentWidget::createHeaderLabel("Videos", "Videos"));
    _mainLayout.addWidget(_filterModel.getTagsFilterAction().createWidget(this, OptionsAction::Tags));
    _mainLayout.addWidget(&_hierarchyWidget, 1);

    _filterModel.setFilterKeyColumn(static_cast<int>(LearningPageVideosModel::Column::Title));

    //_hierarchyWidget.getToolbarAction().setVisible(false);
    _hierarchyWidget.setWindowIcon(mv::Application::getIconFont("FontAwesome").getIcon("video"));
    _hierarchyWidget.setHeaderHidden(true);
    _hierarchyWidget.getColumnsGroupAction().setVisible(false);
    
    _hierarchyWidget.getFilterNameAction().setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    _hierarchyWidget.getFilterGroupAction().setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    _filterModel.getTagsFilterAction().setStretch(2);

    setLayout(&_mainLayout);

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setRootIsDecorated(false);
    treeView.setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView.setSelectionMode(QAbstractItemView::SingleSelection);
    treeView.setIconSize(QSize(24, 24));
    treeView.setMouseTracking(true);
    treeView.setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    treeView.setItemDelegateForColumn(static_cast<int>(LearningPageVideosModel::Column::Delegate), new LearningPageVideoStyledItemDelegate(this));

    treeView.setColumnHidden(static_cast<int>(LearningPageVideosModel::Column::Title), true);
    treeView.setColumnHidden(static_cast<int>(LearningPageVideosModel::Column::Tags), true);
    treeView.setColumnHidden(static_cast<int>(LearningPageVideosModel::Column::Date), true);
    treeView.setColumnHidden(static_cast<int>(LearningPageVideosModel::Column::Summary), true);
    treeView.setColumnHidden(static_cast<int>(LearningPageVideosModel::Column::YouTubeId), true);
    treeView.setColumnHidden(static_cast<int>(LearningPageVideosModel::Column::YouTubeUrl), true);

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, [this](const QModelIndex& parent, int first, int last) -> void {
        qDebug() << __FUNCTION__;
        for (int rowIndex = first; rowIndex <= last; rowIndex++)
            openPersistentEditor(rowIndex);
    });

    connect(&_filterModel, &QSortFilterProxyModel::rowsAboutToBeRemoved, this, [this](const QModelIndex& parent, int first, int last) -> void {
        qDebug() << __FUNCTION__;
        for (int rowIndex = first; rowIndex <= last; rowIndex++)
            closePersistentEditor(rowIndex);
    });

    connect(qApp, &QApplication::paletteChanged, this, &LearningPageVideosWidget::updateCustomStyle);

    updateCustomStyle();
}

void LearningPageVideosWidget::openPersistentEditor(int rowIndex)
{
    const auto index = _filterModel.index(rowIndex, static_cast<int>(LearningPageVideosModel::Column::Delegate));

    if (_hierarchyWidget.getTreeView().isPersistentEditorOpen(index))
        return;

    if (index.isValid())
        _hierarchyWidget.getTreeView().openPersistentEditor(index);
}

void LearningPageVideosWidget::closePersistentEditor(int rowIndex)
{
    const auto index = _filterModel.index(rowIndex, static_cast<int>(LearningPageVideosModel::Column::Delegate));

    if (!_hierarchyWidget.getTreeView().isPersistentEditorOpen(index))
        return;

    if (index.isValid())
        _hierarchyWidget.getTreeView().closePersistentEditor(index);
}

void LearningPageVideosWidget::updateCustomStyle()
{
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
    
    styleSheet += QString(" \
        QTreeView { \
            border: none; \
        } \
    ");

    auto color = QApplication::palette().color(QPalette::Normal, QPalette::Midlight).name();

    styleSheet += QString("QTreeView { background-color: %1;}").arg(color);
    
    treeView.setStyleSheet(styleSheet);
}
