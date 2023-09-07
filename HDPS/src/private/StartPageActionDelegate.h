// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QStyledItemDelegate>

/**
 * Workspace delegate class
 *
 * Delegate class for custom start page action display in a view.
 *
 * @author Thomas Kroes
 */
class StartPageActionDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    
    /**
     * Construct delegate with \p parent object
     * @param parent Pointer to parent object
     */
    StartPageActionDelegate(QObject* parent = nullptr);

    /**
     * Gives the size hint for \p option and model \p index
     * @param option Style option
     * @param index Model index to compute the size hint for
     * @return Size hint
     */
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /**
     * 
     */
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    /**
     * 
     */
    void setEditorData(QWidget* editor, const QModelIndex& index) const;

    /**
     * 
     */
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    static std::int32_t height;
};
