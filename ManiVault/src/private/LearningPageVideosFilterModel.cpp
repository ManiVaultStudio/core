// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideosFilterModel.h"
#include "LearningPageVideosModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define LEARNING_PAGE_VIDEOS_FILTER_MODEL_VERBOSE
#endif

using namespace mv::gui;

LearningPageVideosFilterModel::LearningPageVideosFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _tagsFilterAction(this, "Tags")
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);

    connect(&_tagsFilterAction, &OptionsAction::selectedOptionsChanged, this, &QSortFilterProxyModel::invalidate);
}

bool LearningPageVideosFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    //const auto tagsList         = index.siblingAtColumn(static_cast<int>(LearningPageVideosModel::Column::Tags)).data(Qt::EditRole).toStringList();
    //const auto filterTagsList   = _tagsFilterAction.getSelectedOptions();

    //auto matchTags = false;

    //for (const auto& tag : tagsList) {
    //    if (!filterTagsList.contains(tag))
    //        continue;

    //    matchTags = true;

    //    break;
    //}

    //if (!matchTags) {
    //    qDebug() << "reject row";
    //    //return false;
    //}

    //qDebug() << __FUNCTION__ << "TRUE";

    return true;
}

void LearningPageVideosFilterModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);

    _learningPageVideosModel = static_cast<LearningPageVideosModel*>(sourceModel);

    connect(_learningPageVideosModel, &LearningPageVideosModel::tagsChanged, this, [this](const QSet<QString>& tags) -> void {
        const auto options = QStringList(_learningPageVideosModel->getTagsSet().begin(), _learningPageVideosModel->getTagsSet().end());

        _tagsFilterAction.setOptions(options);
        _tagsFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox);
        _tagsFilterAction.setSettingsPrefix("LearningPage/Videos/FilterModel/TagsFilter");
    });
}

