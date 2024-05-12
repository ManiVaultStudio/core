// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageExamplesWidget.h"
#include "LearningPageContentWidget.h"

#include <QDebug>
#include <QStyledItemDelegate>

using namespace mv::gui;

LearningPageExamplesWidget::LearningPageExamplesWidget(LearningPageContentWidget* learningPageContentWidget) :
    QWidget(learningPageContentWidget),
    _learningPageContentWidget(learningPageContentWidget),
    _mainLayout()
{
    _mainLayout.setSpacing(20);
    _mainLayout.addWidget(PageContentWidget::createHeaderLabel("Examples", "Examples"));

    setLayout(&_mainLayout);

    connect(qApp, &QApplication::paletteChanged, this, &LearningPageExamplesWidget::updateCustomStyle);

    updateCustomStyle();
}

void LearningPageExamplesWidget::openPersistentEditor(int rowIndex)
{
    //const auto index = _filterModel.index(rowIndex, static_cast<int>(LearningPageVideosModel::Column::Delegate));

    //if (_hierarchyWidget.getTreeView().isPersistentEditorOpen(index))
    //    return;

    //if (index.isValid())
    //    _hierarchyWidget.getTreeView().openPersistentEditor(index);
}

void LearningPageExamplesWidget::closePersistentEditor(int rowIndex)
{
    //const auto index = _filterModel.index(rowIndex, static_cast<int>(LearningPageVideosModel::Column::Delegate));

    //if (!_hierarchyWidget.getTreeView().isPersistentEditorOpen(index))
    //    return;

    //if (index.isValid())
    //    _hierarchyWidget.getTreeView().closePersistentEditor(index);
}

void LearningPageExamplesWidget::updateCustomStyle()
{
    //auto styleSheet = QString(" \
    //    QLabel { \
    //        background-color: rgba(0, 0, 0, 0); \
    //    } \
    //    QTreeView::item:hover:!selected { \
    //        background-color: rgba(0, 0, 0, 50); \
    //    } \
    //    QTreeView::item:selected { \
    //        background-color: rgba(0, 0, 0, 100); \
    //    } \
    //");

    //auto& treeView = _hierarchyWidget.getTreeView();
    //
    //styleSheet += QString(" \
    //    QTreeView { \
    //        border: none; \
    //    } \
    //");

    //auto color = QApplication::palette().color(QPalette::Normal, QPalette::Midlight).name();

    //styleSheet += QString("QTreeView { background-color: %1;}").arg(color);
    //
    //treeView.setStyleSheet(styleSheet);
}
