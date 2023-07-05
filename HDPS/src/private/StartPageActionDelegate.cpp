// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageActionDelegate.h"
#include "StartPageActionDelegateEditorWidget.h"
#include "StartPageAction.h"

#include <QCoreApplication>

#ifdef _DEBUG
    #define START_PAGE_ACTION_DELEGATE_VERBOSE
#endif

StartPageActionDelegate::StartPageActionDelegate(QObject* parent /*= nullptr*/) :
    QStyledItemDelegate(parent)
{
}

QSize StartPageActionDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    /*
    QScopedPointer<StartPageActionDelegateEditorWidget> widget(new StartPageActionDelegateEditorWidget);

    widget->setAttribute(Qt::WA_DontShowOnScreen);

    QCoreApplication::processEvents();

    qDebug() << QSize(option.rect.width(), widget->sizeHint().height());
    */

    // Hard-coded for optimization purposes
    return StartPageAction::isCompactView() ? QSize(0, 26) : QSize(0, 43); // 43
}

QWidget* StartPageActionDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    return new StartPageActionDelegateEditorWidget(parent);
}

void StartPageActionDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    qobject_cast<StartPageActionDelegateEditorWidget*>(editor)->setEditorData(index);
}

void StartPageActionDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index)

    editor->setGeometry(option.rect);
}
