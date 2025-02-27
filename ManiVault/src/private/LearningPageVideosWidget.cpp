// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideosWidget.h"
#include "LearningPageContentWidget.h"
#include "LearningPageVideoWidget.h"
#include "LearningPageVideoStyledItemDelegate.h"

#include <Application.h>

#include <CoreInterface.h>

#ifdef _DEBUG
    #define LEARNING_PAGE_VIDEOS_WIDGET_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;

LearningPageVideosWidget::LearningPageVideosWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _settingsAction(this, "Settings")
{
    _mainLayout.setContentsMargins(0, 0, 0, 0);
    _mainLayout.setSpacing(20);
    _mainLayout.addWidget(PageContentWidget::createHeaderLabel("Videos", "Videos"));
    _mainLayout.addWidget(_settingsAction.createWidget(this));
    _mainLayout.addWidget(&_videosListView, 1);

    _settingsAction.setShowLabels(false);
    _settingsAction.addAction(&_videosFilterModel.getFilterGroupAction());
    _settingsAction.addAction(&_videosFilterModel.getTagsFilterAction());

    _videosFilterModel.setSourceModel(&(const_cast<LearningCenterVideosModel&>(mv::help().getVideosModel())));
    _videosFilterModel.getTextFilterColumnAction().setCurrentText("Title");

    _videosListView.setObjectName("Videos");
    _videosListView.setViewMode(QListView::IconMode);
    _videosListView.setFlow(QListView::LeftToRight);
    _videosListView.setWrapping(true);
    _videosListView.setIconSize(QSize(64, 64));
    _videosListView.setResizeMode(QListView::Adjust);
    _videosListView.setModel(&_videosFilterModel);
    _videosListView.setModelColumn(static_cast<int>(LearningCenterVideosModel::Column::Delegate));
    _videosListView.setItemDelegateForColumn(static_cast<int>(LearningCenterVideosModel::Column::Delegate), new LearningPageVideoStyledItemDelegate(this));
    _videosListView.setSpacing(10);

    setLayout(&_mainLayout);

    const auto openPersistentEditors = [this]() -> void {
        for (int rowIndex = 0; rowIndex <= _videosFilterModel.rowCount(); rowIndex++) {
            const auto index = _videosFilterModel.index(rowIndex, static_cast<int>(LearningCenterVideosModel::Column::Delegate));

            //if (_videosListView.isPersistentEditorOpen(index))
            //    return;

            if (index.isValid())
                _videosListView.openPersistentEditor(index);
        }
    };

    openPersistentEditors();

    connect(&_videosFilterModel, &QSortFilterProxyModel::layoutChanged, this, openPersistentEditors);

    connect(&_videosFilterModel, &QSortFilterProxyModel::rowsAboutToBeRemoved, this, [this](const QModelIndex& parent, int first, int last) -> void {
        for (int rowIndex = first; rowIndex <= last; rowIndex++)
            _videosListView.closePersistentEditor(_videosFilterModel.index(rowIndex, static_cast<int>(LearningCenterVideosModel::Column::Delegate)));
    });

    updateCustomStyle();

    connect(&mv::theme(), &AbstractThemeManager::colorSchemeChanged, this, &LearningPageVideosWidget::updateCustomStyle);

    _videosFilterModel.invalidate();
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
        }").arg(QApplication::palette().color(QPalette::Normal, QPalette::Window).name()));
}
