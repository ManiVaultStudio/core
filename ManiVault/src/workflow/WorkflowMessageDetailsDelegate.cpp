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

void WorkflowMessageDetailsDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	const auto details = index.data(Qt::EditRole).toMap();

	if (details.isEmpty()) {
		QStyledItemDelegate::paint(painter, option, index);
		return;
	}

	QStyleOptionButton button;
	button.rect  = option.rect.adjusted(4, 3, -4, -3);
	button.text  = QStringLiteral("Details");
	button.state = QStyle::State_Enabled;

	if (option.state & QStyle::State_MouseOver)
		button.state |= QStyle::State_MouseOver;

	QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

bool WorkflowMessageDetailsDelegate::editorEvent(QEvent* event, QAbstractItemModel* abstractItemModel, const QStyleOptionViewItem& styleOptionViewItem, const QModelIndex& index)
{
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
    auto dialog = new QDialog(parentWindow);

	dialog->setWindowTitle(QStringLiteral("Workflow message details"));
	dialog->resize(800, 600);

	auto* layout = new QVBoxLayout(dialog);

	auto* tree = new QTreeWidget(dialog);
	tree->setColumnCount(2);
	tree->setHeaderLabels({ QStringLiteral("Key"), QStringLiteral("Value") });

	populateTree(tree->invisibleRootItem(), details);

	tree->expandAll();
	tree->resizeColumnToContents(0);

	layout->addWidget(tree);

	dialog->setAttribute(Qt::WA_DeleteOnClose);
	dialog->show();
}

void WorkflowMessageDetailsDelegate::populateTree(QTreeWidgetItem* parent, const QVariant& value)
{
	if (value.metaType().id() == QMetaType::QVariantMap) {
		const auto map = value.toMap();

		for (auto it = map.begin(); it != map.end(); ++it) {
			auto* item = new QTreeWidgetItem(parent);
			item->setText(0, it.key());

			if (it.value().canConvert<QVariantMap>() || it.value().canConvert<QVariantList>())
				populateTree(item, it.value());
			else
				item->setText(1, it.value().toString());
		}

		return;
	}

	if (value.metaType().id() == QMetaType::QVariantList) {
		const auto list = value.toList();

		for (int i = 0; i < list.size(); ++i) {
			auto* item = new QTreeWidgetItem(parent);
			item->setText(0, QStringLiteral("[%1]").arg(i));

			if (list[i].canConvert<QVariantMap>() || list[i].canConvert<QVariantList>())
				populateTree(item, list[i]);
			else
				item->setText(1, list[i].toString());
		}

		return;
	}

	parent->setText(1, value.toString());
}

}
