// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QStyledItemDelegate>

namespace mv::util
{

/**
 * Text elide delegate class
 *
 * To override the default right middle elision QTreeView, you need to apply it
 * via the item delegate, because Qt's item views (like QTreeView) does not support
 * other text elisions natively.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT TextElideDelegate : public QStyledItemDelegate
{
public:

    /** No need for custom constructor*/
    using QStyledItemDelegate::QStyledItemDelegate;

    /**
     * Re-implement the initStyleOption method to set the text elide mode to Qt::ElideMiddle
     * @param option Style option to initialize
     * @param index Model index
     */
    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override;

    /**
     * Get the text elide mode
     * @return Text elide mode
     */
    Qt::TextElideMode getTextElideMode() const;

    /**
     * Set the text elide mode to \p textElideMode
     * @param textElideMode Text elide mode to set
     */
    void setTextElideMode(const Qt::TextElideMode& textElideMode);

private:
    Qt::TextElideMode _textElideMode = Qt::ElideRight;  /** Text elide mode to use */
};


}
