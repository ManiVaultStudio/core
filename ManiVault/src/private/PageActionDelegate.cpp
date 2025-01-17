// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageActionDelegate.h"
#include "PageActionDelegateEditorWidget.h"
#include "PageAction.h"

#ifdef _DEBUG
    #define PAGE_ACTION_DELEGATE_VERBOSE
#endif

PageActionDelegate::PageActionDelegate(QObject* parent /*= nullptr*/) :
    QStyledItemDelegate(parent)
{
}

QSize PageActionDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return PageAction::isCompactView() ? QSize(0, 26) : QSize(0, 43);
}

QWidget* PageActionDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    return new PageActionDelegateEditorWidget(parent);
}

void PageActionDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    qobject_cast<PageActionDelegateEditorWidget*>(editor)->setEditorData(index);
}

void PageActionDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index)

    editor->setGeometry(option.rect);
}
