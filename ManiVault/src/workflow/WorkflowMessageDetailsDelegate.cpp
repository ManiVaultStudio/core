// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowMessageDetailsDelegate.h"

#include "util/Serialization.h"
#include "util/Miscellaneous.h"

#include <QFontDatabase>
#include <QToolTip>

#ifdef _DEBUG
	#define WORKFLOW_MESSAGE_DETAILS_DELEGATE_VERBOSE
#endif

using namespace mv;
using namespace mv::util;

namespace mv::workflow
{

//void WorkflowMessageDetailsDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
//{
//    const auto details = index.data(Qt::EditRole).toMap();
//
//    if (details.isEmpty()) {
//        QStyledItemDelegate::paint(painter, option, index);
//        return;
//    }
//
//    QStyledItemDelegate::paint(painter, option, index);
//
//    constexpr int margin = 0;
//    const int size = option.rect.height() - 2 * margin;
//
//    const QRect iconRect(
//        option.rect.left(),
//        option.rect.top() + margin,
//        size,
//        size
//    );
//
//    const bool hovered = option.state & QStyle::State_MouseOver;
//
//    const QColor color = hovered
//        ? QColor(80, 170, 255)
//        : option.palette.color(QPalette::Text);
//
//    QIcon(StyledIcon("circle-info").withColor(color)).paint(painter, iconRect);
//}

bool WorkflowMessageDetailsDelegate::editorEvent(QEvent* event, QAbstractItemModel* abstractItemModel, const QStyleOptionViewItem& styleOptionViewItem, const QModelIndex& index)
{
    Q_UNUSED(abstractItemModel);
    Q_UNUSED(styleOptionViewItem);

	if (event->type() != QEvent::MouseButtonRelease)
		return false;

	const auto details = index.data(Qt::EditRole).toMap();

	if (details.isEmpty())
		return false;

	showDetailsBrowser(details);

	return true;
}

void WorkflowMessageDetailsDelegate::showDetailsBrowser(const QVariantMap& details)
{
    auto* parentWindow = qobject_cast<QWidget*>(parent());
    auto* dialog = new QDialog(parentWindow);

    dialog->setWindowTitle(QStringLiteral("Workflow message details"));
    dialog->resize(900, 650);

    auto* layout = new QVBoxLayout(dialog);

    auto* model = new QStandardItemModel(dialog);
    model->setHorizontalHeaderLabels({
        QStringLiteral("Key"),
        QStringLiteral("Value")
        });

    for (auto it = details.begin(); it != details.end(); ++it)
        populateModel(model->invisibleRootItem(), it.key(), it.value());

    auto* treeView = new QTreeView(dialog);
    treeView->setModel(model);
    treeView->setAlternatingRowColors(true);
    treeView->setUniformRowHeights(false);
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView->expandAll();
    treeView->resizeColumnToContents(0);

    layout->addWidget(treeView);

    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
    dialog->raise();
    dialog->activateWindow();
}

QList<QStandardItem*> WorkflowMessageDetailsDelegate::makeRow(const QString& key, const QString& value)
{
    auto* keyItem = new QStandardItem(key);
    auto* valueItem = new QStandardItem(value);

    keyItem->setEditable(false);
    valueItem->setEditable(false);

    return { keyItem, valueItem };
}

void WorkflowMessageDetailsDelegate::populateModel(QStandardItem* parent, const QString& key, const QVariant& value)
{
    const auto type = value.metaType().id();

    if (type == QMetaType::QString) {
        parent->appendRow(makeRow(key, value.toString()));
        return;
    }

    if (type == QMetaType::QVariantMap) {
        auto row = makeRow(key);
        auto* keyItem = row.first();
        parent->appendRow(row);

        const auto map = value.toMap();

        for (auto it = map.begin(); it != map.end(); ++it)
            populateModel(keyItem, it.key(), it.value());

        return;
    }

    if (type == QMetaType::QStringList) {
        auto row = makeRow(key);
        auto* keyItem = row.first();
        parent->appendRow(row);

        const auto list = value.toStringList();

        for (int i = 0; i < list.size(); ++i)
            keyItem->appendRow(makeRow(QStringLiteral("[%1]").arg(i), list[i]));

        return;
    }

    if (type == QMetaType::QVariantList) {
        auto row = makeRow(key);
        auto* keyItem = row.first();
        parent->appendRow(row);

        const auto list = value.toList();

        for (int i = 0; i < list.size(); ++i)
            populateModel(keyItem, QStringLiteral("[%1]").arg(i), list[i]);

        return;
    }

    parent->appendRow(makeRow(key, value.toString()));
}

}
