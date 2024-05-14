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
    _titleFilterAction(this, "Title"),
    _tagsFilterAction(this, "Tags")
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);

    _tagsFilterAction.setIconByName("tag");
    _tagsFilterAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _tagsFilterAction.setDefaultWidgetFlags(OptionsAction::Tags | OptionsAction::Selection);

    //connect(&_tagsFilterAction, &OptionsAction::selectedOptionsChanged, this, [this]() -> void {
    //    invalidate();
    //});
}

bool LearningPageVideosFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    qDebug() << __FUNCTION__;
    const auto index = sourceModel()->index(row, 0, parent);

    return true;

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    const auto tagsList         = index.siblingAtColumn(static_cast<int>(LearningPageVideosModel::Column::Tags)).data(Qt::EditRole).toStringList();
    const auto filterTagsList   = _tagsFilterAction.getSelectedOptions();

    if (_tagsFilterAction.hasOptions()) {
        qDebug() << filterTagsList;

        auto matchTags = false;

        //for (const auto& tag : tagsList) {
        //    //qDebug() << tag << filterTagsList;
        //    if (!filterTagsList.contains(tag))
        //        continue;

        //    matchTags = true;

        //    break;
        //}

        ////qDebug() << "matchTags" << matchTags;
        //if (!matchTags) {

        //    //return false;
        //}
    }
    

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
    });
}

bool LearningPageVideosFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}

