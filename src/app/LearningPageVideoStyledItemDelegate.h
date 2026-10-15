// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QStyledItemDelegate>

/**
 * Learning page video styled item delegate class
 *
 * Delegate class for showing video information
 *
 * @author Thomas Kroes
 */
class LearningPageVideoStyledItemDelegate : public QStyledItemDelegate
{
public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    LearningPageVideoStyledItemDelegate(QObject* parent = nullptr);

    /**
     * Gives the size hint for \p option and model \p index
     * @param option Style option
     * @param index Model index to compute the size hint for
     * @return Size hint
     */
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /**
     * Creates an editor widget
     * @param parent Pointer to parent widget
     * @param option Style options
     * @param index Model index
     */
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /**
     * Updates \p editor widget geometry
     * @param editor Pointer to editor widget
     * @param option Style options
     * @param index Model index
     */
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};