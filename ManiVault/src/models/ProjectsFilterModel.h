// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "models/SortFilterProxyModel.h"

#include "actions/OptionsAction.h"
#include "actions/VersionAction.h"

namespace mv {

class AbstractProjectsModel;

/**
 * Project database filter model class
 *
 * Sorting and filtering model for the project database model
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectsFilterModel : public SortFilterProxyModel
{
public:

    /** 
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
    */
    ProjectsFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether \p row with \p parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     * @return Boolean indicating whether the item is filtered in or out
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /**
     * Set source model to \p sourceModel
     * @param sourceModel Pointer to source model
     */
    void setSourceModel(QAbstractItemModel* sourceModel) override;

    /**
     * Compares two model indices plugin \p lhs with \p rhs
     * @param lhs Left-hand model index
     * @param rhs Right-hand model index
     */
    bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;

private:

    /**
     * Get whether the parent has accepted children
     * @param parent Parent model index
     * @return Boolean indicating whether the parent has accepted children
     */
    bool hasAcceptedChildren(const QModelIndex& parent) const;

public: // Serialization

    /**
     * Load from variant map
     * @param variantMap Variant map
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save to variant map
     * @return Variant map
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    gui::OptionsAction& getTagsFilterAction() { return _tagsFilterAction; }
    gui::OptionsAction& getExcludeTagsFilterAction() { return _excludeTagsFilterAction; }
    gui::VersionAction& getTargetAppVersionAction() { return _targetAppVersionAction; }
	gui::ToggleAction& getFilterLoadableOnlyAction() { return _filterLoadableOnlyAction; }
	gui::ToggleAction& getFilterStartupOnlyAction() { return _filterStartupOnlyAction; }
	gui::VerticalGroupAction& getFilterGroupAction() { return _filterGroupAction; }

private:
    AbstractProjectsModel*      _projectDatabaseModel;          /** Pointer to source model */
    gui::OptionsAction          _tagsFilterAction;              /** Filter based on tag(s) */
    gui::OptionsAction          _excludeTagsFilterAction;       /** Filter out based on tag(s) */
    gui::VersionAction          _targetAppVersionAction;        /** Target app minimum version filter action */
    gui::ToggleAction           _filterLoadableOnlyAction;      /** Filter out projects that are not loadable */
    gui::ToggleAction           _filterStartupOnlyAction;       /** Filter out projects that are not startup projects */
    gui::VerticalGroupAction    _filterGroupAction;             /** Groups the filter text, filter settings and minimum version settings */
};

}
