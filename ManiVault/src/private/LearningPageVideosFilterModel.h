// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QSortFilterProxyModel>

#include <actions/StringAction.h>
#include <actions/OptionsAction.h>

class LearningPageVideosModel;

/**
 * Learning page videos filter model class
 *
 * Sorting and filtering model for the learning page videos model
 *
 * @author Thomas Kroes
 */
class LearningPageVideosFilterModel : public QSortFilterProxyModel
{
public:

    /** 
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
    */
    LearningPageVideosFilterModel(QObject* parent = nullptr);

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

    mv::gui::StringAction& getTitleFilterAction() { return _titleFilterAction; }
    mv::gui::OptionsAction& getTagsFilterAction() { return _tagsFilterAction; }

private:
    LearningPageVideosModel*    _learningPageVideosModel;   /** Pointer to source model */
    mv::gui::StringAction       _titleFilterAction;         /** Filter based on title */
    mv::gui::OptionsAction      _tagsFilterAction;          /** Filter based on tag(s) */
};
