// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningCenterTutorialsFilterModel.h"
#include "LearningCenterTutorialsModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define LEARNING_CENTER_TUTORIALS_FILTER_MODEL_VERBOSE
#endif

using namespace mv::gui;

namespace mv {

LearningCenterTutorialsFilterModel::LearningCenterTutorialsFilterModel(QObject* parent /*= nullptr*/) :
    SortFilterProxyModel(parent),
    _learningCenterTutorialsModel(nullptr),
    _tagsFilterAction(this, "Tags filter"),
    _filterGroupAction(this, "Filter group")
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);
    setRowTypeName("Video");

    _tagsFilterAction.setIconByName("tag");
    _tagsFilterAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _tagsFilterAction.setDefaultWidgetFlags(OptionsAction::Tags | OptionsAction::Selection);

    connect(&_tagsFilterAction, &OptionsAction::selectedOptionsChanged, this, [this]() -> void {
        invalidate();
    });

    _filterGroupAction.setShowLabels(false);
    _filterGroupAction.addAction(&getTextFilterAction());
    _filterGroupAction.addAction(&getTextFilterSettingsAction());
}

bool LearningCenterTutorialsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    const auto tagsList         = index.siblingAtColumn(static_cast<int>(LearningCenterTutorialsModel::Column::Tags)).data(Qt::EditRole).toStringList();
    const auto filterTagsList   = _tagsFilterAction.getSelectedOptions();

    if (_tagsFilterAction.hasOptions()) {
        auto matchTags = false;

        for (const auto& tag : tagsList) {
            if (!filterTagsList.contains(tag))
                continue;

            matchTags = true;

            break;
        }

        if (!matchTags)
            return false;
    }

    return true;
}

void LearningCenterTutorialsFilterModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    SortFilterProxyModel::setSourceModel(sourceModel);

    _learningCenterTutorialsModel = dynamic_cast<LearningCenterTutorialsModel*>(sourceModel);

    connect(_learningCenterTutorialsModel, &LearningCenterTutorialsModel::tagsChanged, this, [this](const QSet<QString>& tags) -> void {
        const auto options = QStringList(_learningCenterTutorialsModel->getTagsSet().begin(), _learningCenterTutorialsModel->getTagsSet().end());

        _tagsFilterAction.initialize(options, options);
        _tagsFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox);
    });
}

bool LearningCenterTutorialsFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}

}
