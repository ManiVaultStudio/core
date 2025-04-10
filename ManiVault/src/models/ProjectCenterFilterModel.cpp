// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectCenterFilterModel.h"
#include "ProjectCenterModel.h"

#include "util/Version.h"

#include "Application.h"

#include <QDebug>

#ifdef _DEBUG
    #define PROJECT_CENTER_FILTER_MODEL_VERBOSE
#endif

using namespace mv::gui;
using namespace mv::util;

namespace mv {

ProjectCenterFilterModel::ProjectCenterFilterModel(QObject* parent /*= nullptr*/) :
    SortFilterProxyModel(parent),
    _projectCenterModel(nullptr),
    _tagsFilterAction(this, "Tags filter"),
    _excludeTagsFilterAction(this, "Exclude tags filter"),
    _targetAppVersionAction(this, "App version"),
    _filterGroupAction(this, "Filter group")
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);
    setRowTypeName("Project");

    _tagsFilterAction.setIconByName("tag");
    _tagsFilterAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _tagsFilterAction.setDefaultWidgetFlags(OptionsAction::Tags | OptionsAction::Selection);
    _tagsFilterAction.setPopupSizeHint(QSize(500, 200));

    const auto applicationVersion = Application::current()->getVersion();

    _targetAppVersionAction.setToolTip("Minimum ManiVault Studio version");
    _targetAppVersionAction.getSuffixAction().setVisible(false);

    _filterGroupAction.setIconByName("filter");
    _filterGroupAction.setPopupSizeHint({ 400, 0 });

    connect(&_tagsFilterAction, &OptionsAction::selectedOptionsChanged, this, &ProjectCenterFilterModel::invalidate);
    connect(&_excludeTagsFilterAction, &OptionsAction::selectedOptionsChanged, this, &ProjectCenterFilterModel::invalidate);
    connect(&_targetAppVersionAction, &VersionAction::versionChanged, this, &ProjectCenterFilterModel::invalidate);

    _filterGroupAction.addAction(&getTextFilterCaseSensitiveAction());
    _filterGroupAction.addAction(&getTextFilterRegularExpressionAction());
    _filterGroupAction.addAction(&getTargetAppVersionAction());
}

bool ProjectCenterFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    /*
    const auto tagsList                 = index.siblingAtColumn(static_cast<int>(ProjectCenterModel::Column::Tags)).data(Qt::EditRole).toStringList();
    const auto filterTagsList           = _tagsFilterAction.getSelectedOptions();
    const auto filterExcludeTagsList    = _excludeTagsFilterAction.getSelectedOptions();

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

    if (_excludeTagsFilterAction.hasOptions()) {
        for (const auto& tag : tagsList) {
            if (filterExcludeTagsList.contains(tag))
                return false;
        }
    }

    const auto minimumVersionMajor  = index.siblingAtColumn(static_cast<int>(ProjectCenterModel::Column::MinimumVersionMajor)).data(Qt::EditRole).toInt();
    const auto minimumVersionMinor  = index.siblingAtColumn(static_cast<int>(ProjectCenterModel::Column::MinimumVersionMinor)).data(Qt::EditRole).toInt();

    const Version tutorialMinimumAppVersion(minimumVersionMajor, minimumVersionMinor, 0) ;
    const Version targetAppVersion(_targetAppVersionAction.getMajor(), _targetAppVersionAction.getMinor(), 0) ;

    if (targetAppVersion > tutorialMinimumAppVersion)
        return false;
    */

    return true;
}

void ProjectCenterFilterModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    SortFilterProxyModel::setSourceModel(sourceModel);

    _projectCenterModel = dynamic_cast<ProjectCenterModel*>(sourceModel);

    const auto updateTags = [this]() -> void {
        const auto uniqueTags = QStringList(_projectCenterModel->getTagsSet().begin(), _projectCenterModel->getTagsSet().end());

        if (uniqueTags == _tagsFilterAction.getOptions())
            return;

        _tagsFilterAction.setOptions(uniqueTags);
    	_tagsFilterAction.setSelectedOptions(_tagsFilterAction.hasSelectedOptions() ? _tagsFilterAction.getSelectedOptions() : uniqueTags);
    };

    connect(_projectCenterModel, &ProjectCenterModel::tagsChanged, this, updateTags);

    updateTags();
}

bool ProjectCenterFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}

void ProjectCenterFilterModel::fromVariantMap(const QVariantMap& variantMap)
{
	SortFilterProxyModel::fromVariantMap(variantMap);

    _tagsFilterAction.fromParentVariantMap(variantMap);
    _excludeTagsFilterAction.fromParentVariantMap(variantMap);
    _targetAppVersionAction.fromParentVariantMap(variantMap);
}

QVariantMap ProjectCenterFilterModel::toVariantMap() const
{
	auto variantMap = SortFilterProxyModel::toVariantMap();

    _tagsFilterAction.insertIntoVariantMap(variantMap);
    _excludeTagsFilterAction.insertIntoVariantMap(variantMap);
    _targetAppVersionAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
