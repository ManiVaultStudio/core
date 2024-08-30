// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <models/SortFilterProxyModel.h>

#include <actions/OptionsAction.h>
#include <actions/HorizontalGroupAction.h>

class HelpManagerVideosModel;

/**
 * Help manager videos filter model class
 *
 * Sorting and filtering model for the help manager videos model
 *
 * @author Thomas Kroes
 */
class HelpManagerVideosFilterModel : public mv::SortFilterProxyModel
{
public:

    /** 
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
    */
    HelpManagerVideosFilterModel(QObject* parent = nullptr);

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

public: // Action getters

    mv::gui::OptionsAction& getTagsFilterAction() { return _tagsFilterAction; }
    mv::gui::HorizontalGroupAction& getFilterGroupAction() { return _filterGroupAction; }

private:
    HelpManagerVideosModel*         _videosModel;           /** Pointer to source model */
    mv::gui::OptionsAction          _tagsFilterAction;      /** Filter based on tag(s) */
    mv::gui::HorizontalGroupAction  _filterGroupAction;     /** Groups the filter text and the the filter settings */
};
