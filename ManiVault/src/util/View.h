// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QStyledItemDelegate>

namespace mv::util
{

/**
 * Middle elide delegate class
 *
 * To set middle elision (i.e. "MyVery...ongText.txt") in a QTreeView, you need to apply it
 * via the item delegate, because Qt's item views (like QTreeView) do not support
 * mid-elision natively — they only do end-elision by default.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT MiddleElideDelegate : public QStyledItemDelegate
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
};


}
