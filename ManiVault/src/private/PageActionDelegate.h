// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QStyledItemDelegate>

/**
 * Page delegate class
 *
 * For custom page action display in a view.
 *
 * @author Thomas Kroes
 */
class PageActionDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    
    /**
     * Construct delegate with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    PageActionDelegate(QObject* parent = nullptr);

    /**
     * Gives the size hint for \p option and model \p index
     * @param option Style option
     * @param index Model index to compute the size hint for
     * @return Size hint
     */
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /**
     * Create custom delegate widget
     * @param parent Pointer to parent widget
     * @param option Style view options
     * @param index Model index
     * @return Pointer to widget
     */
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /**
     * Set \p editor data for model \p index
     * @param editor Pointer to editor
     * @param index Model index
     */
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    /**
     * Override update editor geometry
     * @param editor Pointer to editor widget
     * @param option Style view options
     * @param index Model index
     */
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    static std::int32_t height; /** Height of the delegate widget */
};
