// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowMessageDetailsDelegate.h"

#include "util/Serialization.h"
#include "util/Miscellaneous.h"

#include <QAbstractItemView>
#include <QDialog>
#include <QFontDatabase>
#include <QStandardItemModel>
#include <QToolTip>
#include <QTreeView>
#include <QVBoxLayout>
#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QMessageBox>
#include <QMouseEvent>
#include <QMainWindow>
#include <QPushButton>

#ifdef _DEBUG
	#define WORKFLOW_MESSAGE_DETAILS_DELEGATE_VERBOSE
#endif

using namespace mv;
using namespace mv::util;

namespace mv::workflow
{

namespace
{

    void copyDetailsToClipboard(const QVariantMap& details)
    {
        const auto json = QJsonDocument::fromVariant(details).toJson(QJsonDocument::Indented);
        QApplication::clipboard()->setText(QString::fromUtf8(json));

        mv::help().addNotification("Copied to clipboard", "The workflow message details have been copied to the clipboard.", StyledIcon("clipboard"));
    }

    void saveDetailsToJsonFile(const QVariantMap& details, QWidget* parent)
    {
        const auto fileName = QFileDialog::getSaveFileName(parent, QStringLiteral("Save workflow message details"), QStringLiteral("workflow-message-details.json"), QStringLiteral("JSON files (*.json);;All files (*)"));

        if (fileName.isEmpty())
            return;

        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            QMessageBox::warning(parent, QStringLiteral("Save failed"), QStringLiteral("Could not write to:\n%1").arg(fileName));
            return;
        }

        const auto json = QJsonDocument::fromVariant(details).toJson(QJsonDocument::Indented);
        file.write(json);

        mv::help().addNotification("Saved to file", QString("The workflow message details have been saved to %1.").arg(fileName), StyledIcon("save"));
    }

} // namespace

bool WorkflowMessageDetailsDelegate::editorEvent(QEvent* event, QAbstractItemModel* abstractItemModel, const QStyleOptionViewItem& styleOptionViewItem, const QModelIndex& index)
{
    Q_UNUSED(abstractItemModel);
    Q_UNUSED(styleOptionViewItem);

	if (event->type() != QEvent::MouseButtonRelease)
		return false;

	const auto* mouseEvent = static_cast<QMouseEvent*>(event);
	if (mouseEvent->button() != Qt::LeftButton)
		return false;
	const auto details = index.data(Qt::EditRole).toMap();

	if (details.isEmpty())
		return false;

	showDetailsBrowser(details);

	return true;
}

void WorkflowMessageDetailsDelegate::showDetailsBrowser(const QVariantMap& details)
{
	auto parentWindow   = qobject_cast<QWidget*>(parent());
	auto dialog         = new QDialog(parentWindow);

	dialog->setWindowModality(Qt::NonModal);
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	dialog->setWindowTitle(QStringLiteral("Workflow message details"));
	dialog->resize(900, 650);

	auto layout = new QVBoxLayout(dialog);
	auto model  = new QStandardItemModel(dialog);

	model->setHorizontalHeaderLabels({
		QStringLiteral("Key"),
		QStringLiteral("Value")
	});

	for (auto it = details.begin(); it != details.end(); ++it)
		populateModel(model->invisibleRootItem(), it.key(), it.value());

	auto treeView = new QTreeView(dialog);

	treeView->setModel(model);
	treeView->setAlternatingRowColors(true);
	treeView->setUniformRowHeights(false);
	treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	treeView->collapseAll();
	treeView->resizeColumnToContents(0);

	layout->addWidget(treeView);

	auto buttons    = new QDialogButtonBox(dialog);
	auto copyButton = buttons->addButton(QStringLiteral("Copy JSON"), QDialogButtonBox::ActionRole);
	auto saveButton = buttons->addButton(QStringLiteral("Save JSON..."), QDialogButtonBox::ActionRole);

	buttons->addButton(QDialogButtonBox::Close);

	connect(copyButton, &QPushButton::clicked, dialog, [details] {
		copyDetailsToClipboard(details);
	});

	connect(saveButton, &QPushButton::clicked, dialog, [details, dialog] {
		saveDetailsToJsonFile(details, dialog);
	});

	connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::close);

	// WA_DeleteOnClose already set above.
	dialog->show();
	dialog->raise();
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
