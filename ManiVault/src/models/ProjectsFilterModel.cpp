// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsFilterModel.h"
#include "AbstractProjectsModel.h"

#include "util/Version.h"

#include "Application.h"

#include <QDebug>

#ifdef _DEBUG
    #define PROJECTS_FILTER_MODEL_VERBOSE
#endif

using namespace mv::gui;
using namespace mv::util;

namespace mv {

ProjectsFilterModel::ProjectsFilterModel(QObject* parent /*= nullptr*/) :
    SortFilterProxyModel(parent, "ProjectsFilterModel"),
    _projectDatabaseModel(nullptr),
    _tagsFilterAction(this, "Tags filter"),
    _excludeTagsFilterAction(this, "Exclude tags filter"),
    _targetAppVersionAction(this, "App version"),
    _filterLoadableOnlyAction(this, "Loadable only", true),
    _filterGroupAction(this, "Filter group")
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);
    setRowTypeName("Project");

    _tagsFilterAction.setIconByName("tag");
    _tagsFilterAction.setDefaultWidgetFlags(OptionsAction::WidgetFlag::Tags | OptionsAction::WidgetFlag::Selection);
    _tagsFilterAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _tagsFilterAction.setPopupSizeHint(QSize(400, 100));

    const auto applicationVersion = Application::current()->getVersion();

    _targetAppVersionAction.setToolTip("Minimum ManiVault Studio version");
    _targetAppVersionAction.getSuffixAction().setVisible(false);

    _filterGroupAction.setIconByName("filter");
    _filterGroupAction.setPopupSizeHint({ 400, 0 });

    connect(&_tagsFilterAction, &OptionsAction::selectedOptionsChanged, this, &ProjectsFilterModel::invalidate);
    connect(&_excludeTagsFilterAction, &OptionsAction::selectedOptionsChanged, this, &ProjectsFilterModel::invalidate);
    connect(&_targetAppVersionAction, &VersionAction::versionChanged, this, &ProjectsFilterModel::invalidate);
    connect(&_filterLoadableOnlyAction, &ToggleAction::toggled, this, &ProjectsFilterModel::invalidate);

    _filterGroupAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    _filterGroupAction.addAction(&getFilterLoadableOnlyAction());
    _filterGroupAction.addAction(&getTextFilterCaseSensitiveAction());
    _filterGroupAction.addAction(&getTextFilterRegularExpressionAction());
    _filterGroupAction.addAction(&getTargetAppVersionAction());
}

bool ProjectsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    const auto tagsList         = index.siblingAtColumn(static_cast<int>(AbstractProjectsModel::Column::Tags)).data(Qt::EditRole).toStringList();
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

    const auto projectMinimumCoreVersion  = index.siblingAtColumn(static_cast<int>(AbstractProjectsModel::Column::MinimumCoreVersion)).data(Qt::EditRole).value<Version>();

    const Version targetAppVersion(_targetAppVersionAction.getMajor(), _targetAppVersionAction.getMinor(), 0) ;

    if (targetAppVersion > projectMinimumCoreVersion)
        return false;

    const auto missingPlugins = index.siblingAtColumn(static_cast<int>(AbstractProjectsModel::Column::MissingPlugins)).data(Qt::EditRole).toStringList();

    if (_filterLoadableOnlyAction.isChecked() && !missingPlugins.isEmpty())
        return false;

    return true;
}

void ProjectsFilterModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    SortFilterProxyModel::setSourceModel(sourceModel);

    _projectDatabaseModel = dynamic_cast<AbstractProjectsModel*>(sourceModel);

    const auto updateTags = [this]() -> void {
        const auto uniqueTags = QStringList(_projectDatabaseModel->getTagsSet().begin(), _projectDatabaseModel->getTagsSet().end());

        if (uniqueTags == _tagsFilterAction.getOptions())
            return;

        _tagsFilterAction.setOptions(uniqueTags);
    	_tagsFilterAction.setSelectedOptions(_tagsFilterAction.hasSelectedOptions() ? _tagsFilterAction.getSelectedOptions() : uniqueTags);
    };

    connect(_projectDatabaseModel, &AbstractProjectsModel::tagsChanged, this, updateTags);

    updateTags();
}

bool ProjectsFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}

void ProjectsFilterModel::fromVariantMap(const QVariantMap& variantMap)
{
	SortFilterProxyModel::fromVariantMap(variantMap);

    _tagsFilterAction.fromParentVariantMap(variantMap);
    _excludeTagsFilterAction.fromParentVariantMap(variantMap);
    _targetAppVersionAction.fromParentVariantMap(variantMap);
    _filterLoadableOnlyAction.fromParentVariantMap(variantMap);
}

QVariantMap ProjectsFilterModel::toVariantMap() const
{
	auto variantMap = SortFilterProxyModel::toVariantMap();

    _tagsFilterAction.insertIntoVariantMap(variantMap);
    _excludeTagsFilterAction.insertIntoVariantMap(variantMap);
    _targetAppVersionAction.insertIntoVariantMap(variantMap);
    _filterLoadableOnlyAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
