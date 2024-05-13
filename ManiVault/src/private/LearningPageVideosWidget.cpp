// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideosWidget.h"
#include "LearningPageContentWidget.h"
#include "LearningPageVideoWidget.h"

#include <Application.h>

#include <QDebug>
#include <QStyledItemDelegate>

using namespace mv::gui;

LearningPageVideosWidget::LearningPageVideosWidget(LearningPageContentWidget* learningPageContentWidget) :
    QWidget(learningPageContentWidget),
    _learningPageContentWidget(learningPageContentWidget),
    _mainLayout(),
    _settingsAction(this, "Settings"),
    _videosScrollArea(),
    _videosWidget(),
    _videosLayout(),
    _model(),
    _filterModel()
{
    _mainLayout.setContentsMargins(0, 0, 0, 0);
    _mainLayout.setSpacing(20);
    _mainLayout.addWidget(PageContentWidget::createHeaderLabel("Videos", "Videos"));
    //_mainLayout.addWidget(_settingsAction.createWidget(this));
    _mainLayout.addWidget(_filterModel.getTagsFilterAction().createWidget(this, OptionsAction::Tags));
    _mainLayout.addWidget(&_videosScrollArea, 1);

    _settingsAction.setShowLabels(false);

    _settingsAction.addAction(&_filterModel.getTitleFilterAction());
    _settingsAction.addAction(&_filterModel.getTagsFilterAction(), OptionsAction::Tags);

    _videosScrollArea.setObjectName("VideosScrollArea");
    _videosScrollArea.setWidgetResizable(true);
    _videosScrollArea.setWidget(&_videosWidget);
    _videosScrollArea.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _videosScrollArea.setStyleSheet("QScrollArea#VideosScrollArea { background-color: transparent; border: none; }");
    
    _videosWidget.setObjectName("VideosWidget");
    _videosWidget.setLayout(&_videosLayout);

    _videosLayout.setContentsMargins(0, 0, 0, 0);
    _videosLayout.setVerticalSpacing(10);
    _videosLayout.setAlignment(Qt::AlignTop);

    _filterModel.setSourceModel(&_model);
    _filterModel.setFilterKeyColumn(static_cast<int>(LearningPageVideosModel::Column::Title));

    _filterModel.getTagsFilterAction().setStretch(2);

    setLayout(&_mainLayout);

    //connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, &LearningPageVideosWidget::updateVideos);
    //connect(&_filterModel, &QSortFilterProxyModel::rowsRemoved, this, &LearningPageVideosWidget::updateVideos);

    connect(qApp, &QApplication::paletteChanged, this, &LearningPageVideosWidget::updateCustomStyle);

    updateCustomStyle();
    updateVideos();
}

void LearningPageVideosWidget::updateCustomStyle()
{
    _videosWidget.setStyleSheet(QString("QWidget#VideosWidget { background-color: %1; border: none;}").arg(QApplication::palette().color(QPalette::Normal, QPalette::Midlight).name()));
}

void LearningPageVideosWidget::updateVideos()
{
    qDebug() << __FUNCTION__;

    QLayoutItem* layoutItem;

    while ((layoutItem = _videosLayout.takeAt(0)) != nullptr) {
        delete layoutItem->widget();
        delete layoutItem;
    }

    const auto numberOfColumns = 4;

    for (std::int32_t rowIndex = 0; rowIndex < _filterModel.rowCount(); ++rowIndex) {
        const auto filterModelIndex = _filterModel.index(rowIndex, 0);
        const auto sourceModelIndex = _filterModel.mapToSource(filterModelIndex);

        if (!sourceModelIndex.isValid())
            continue;

        _videosLayout.addWidget(new LearningPageVideoWidget(sourceModelIndex), std::int32_t(floorf(rowIndex / static_cast<float>(numberOfColumns))), rowIndex % numberOfColumns);
    }

    _videosLayout.setRowStretch(_videosLayout.rowCount(), 1);
}
