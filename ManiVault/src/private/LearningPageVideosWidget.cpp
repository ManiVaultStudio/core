// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideosWidget.h"
#include "LearningPageContentWidget.h"
#include "LearningPageVideoWidget.h"
#include "LearningPageVideoStyledItemDelegate.h"

#include <Application.h>

#include <QDebug>
#include <QStyledItemDelegate>

#ifdef _DEBUG
    #define LEARNING_PAGE_VIDEOS_WIDGET_VERBOSE
#endif

using namespace mv::gui;

LearningPageVideosWidget::LearningPageVideosWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _mainLayout(),
    _settingsAction(this, "Settings"),
    _videosListView(),
    _model(),
    _filterModel()
{
    _mainLayout.setContentsMargins(0, 0, 0, 0);
    _mainLayout.setSpacing(20);
    _mainLayout.addWidget(PageContentWidget::createHeaderLabel("Videos", "Videos"));
    _mainLayout.addWidget(_filterModel.getTagsFilterAction().createWidget(this));
    _mainLayout.addWidget(_filterModel.getFilterGroupAction().createWidget(this));
    _mainLayout.addWidget(&_videosListView, 1);

    _filterModel.setSourceModel(&_model);
    _filterModel.getTagsFilterAction().setStretch(2);

    _videosListView.setObjectName("Videos");
    _videosListView.setViewMode(QListView::IconMode);
    _videosListView.setModel(&_filterModel);
    _videosListView.setModelColumn(static_cast<int>(LearningPageVideosModel::Column::Delegate));
    _videosListView.setItemDelegateForColumn(static_cast<int>(LearningPageVideosModel::Column::Delegate), new LearningPageVideoStyledItemDelegate(this));

    setLayout(&_mainLayout);

    const auto openPersistentEditors = [this]() -> void {
        for (int rowIndex = 0; rowIndex <= _filterModel.rowCount(); rowIndex++) {
            const auto index = _filterModel.index(rowIndex, static_cast<int>(LearningPageVideosModel::Column::Delegate));

            //if (_videosListView.isPersistentEditorOpen(index))
            //    return;

            if (index.isValid())
                _videosListView.openPersistentEditor(index);
        }
    };

    openPersistentEditors();

    connect(&_filterModel, &QSortFilterProxyModel::layoutChanged, this, openPersistentEditors);

    connect(&_filterModel, &QSortFilterProxyModel::rowsAboutToBeRemoved, this, [this](const QModelIndex& parent, int first, int last) -> void {
        for (int rowIndex = first; rowIndex <= last; rowIndex++)
            _videosListView.closePersistentEditor(_filterModel.index(rowIndex, static_cast<int>(LearningPageVideosModel::Column::Delegate)));
    });

    connect(qApp, &QApplication::paletteChanged, this, &LearningPageVideosWidget::updateCustomStyle);

    updateCustomStyle();
}

void LearningPageVideosWidget::showEvent(QShowEvent* showEvent)
{
    _model.populateFromServer();
}

void LearningPageVideosWidget::updateCustomStyle()
{
    _videosListView.setStyleSheet(QString(" \
        QListView#Videos { \
            background-color: %1; \
            border: none; \
        } \
        QListView#Videos::item:hover { \
            background-color: transparent; \
        }").arg(QApplication::palette().color(QPalette::Normal, QPalette::Midlight).name()));
}
