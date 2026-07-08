// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Exception.h"

#include "CoreInterface.h"
#include "exception/ManiVaultException.h"

#include <QApplication>
#include <QClipboard>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QTreeWidget>

namespace mv::util {

namespace
{
    QString stackFrameToString(const StackFrame& frame)
    {
	    if (!frame.raw.isEmpty())
	    	return frame.raw;

	    QString line;

	    if (!frame.address.isEmpty())
	    	line += frame.address + " ";

	    if (!frame.function.isEmpty())
	    	line += "in " + frame.function;

	    if (!frame.file.isEmpty()) {
	    	line += " at " + frame.file;

	    	if (frame.line >= 0)
	    		line += ":" + QString::number(frame.line);
	    }

	    return line;
    }

    QStringList stackTraceToStringList(const StackTrace& stackTrace)
    {
	    QStringList lines;

	    for (const auto& frame : stackTrace)
	    	lines << stackFrameToString(frame);

	    return lines;
    }

    void copyExceptionReportToClipboard(
		const QString& title,
		const QString& reason,
		const StackTrace& stackTrace)
    {
	    QString report;

	    report += title + "\n\n";
	    report += reason + "\n\n";

	    const auto stackTraceLines = stackTraceToStringList(stackTrace);

	    if (!stackTraceLines.isEmpty()) {
	    	report += "Stack trace:\n";
	    	report += stackTraceLines.join('\n');
	    }

	    QApplication::clipboard()->setText(report);

        mv::help().addNotification("Copied to clipboard", "The exception report has been copied to the clipboard.", StyledIcon("clipboard"));
    }

    void exceptionDialog(const QString& title, const QString& reason, const StackTrace& stackTrace, QWidget* parent)
    {
        QDialog dialog(parent);

        dialog.setWindowTitle(title);
        dialog.setModal(true);
        dialog.setMinimumWidth(700);

        auto* layout = new QVBoxLayout(&dialog);
        layout->setContentsMargins(20, 18, 20, 16);
        layout->setSpacing(16);

        auto* reasonLabel = new QLabel(reason, &dialog);
        reasonLabel->setWordWrap(true);
        reasonLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        reasonLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        layout->addWidget(reasonLabel);

        QWidget* detailsWidget = nullptr;
        QToolButton* toggleButton = nullptr;
        QTreeWidget* stackTraceTree = nullptr;

        if (!stackTrace.isEmpty()) {
            toggleButton = new QToolButton(&dialog);
            toggleButton->setText("Technical details");
            toggleButton->setCheckable(true);
            toggleButton->setChecked(false);
            toggleButton->setArrowType(Qt::RightArrow);
            toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

            layout->addWidget(toggleButton, 0, Qt::AlignLeft);

            detailsWidget = new QWidget(&dialog);

            auto* detailsLayout = new QVBoxLayout(detailsWidget);
            detailsLayout->setContentsMargins(0, 0, 0, 0);
            detailsLayout->setSpacing(8);

            stackTraceTree = new QTreeWidget(detailsWidget);
            stackTraceTree->setColumnCount(3);
            stackTraceTree->setHeaderLabels({ "Function", "File", "Line" });
            stackTraceTree->setRootIsDecorated(false);
            stackTraceTree->setAlternatingRowColors(true);
            stackTraceTree->setUniformRowHeights(true);
            stackTraceTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
            stackTraceTree->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
            stackTraceTree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            stackTraceTree->setMinimumHeight(300);

            stackTraceTree->header()->setStretchLastSection(false);
            stackTraceTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
            stackTraceTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
            stackTraceTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

            for (const auto& frame : stackTrace) {
                auto* item = new QTreeWidgetItem(stackTraceTree);

                item->setText(0, frame.function);
                item->setText(1, QFileInfo(frame.file).fileName());

                if (frame.line >= 0)
                    item->setText(2, QString::number(frame.line));

                item->setToolTip(0, stackFrameToString(frame));
                item->setData(0, Qt::UserRole, stackFrameToString(frame));
            }

            detailsLayout->addWidget(stackTraceTree);

            auto* frameDetails = new QPlainTextEdit(detailsWidget);
            frameDetails->setReadOnly(true);
            frameDetails->setMaximumHeight(90);
            frameDetails->setLineWrapMode(QPlainTextEdit::NoWrap);
            frameDetails->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

            detailsLayout->addWidget(frameDetails);

            QObject::connect(
                stackTraceTree,
                &QTreeWidget::currentItemChanged,
                &dialog,
                [frameDetails](QTreeWidgetItem* current, QTreeWidgetItem*)
                {
                    if (!current) {
                        frameDetails->clear();
                        return;
                    }

                    frameDetails->setPlainText(
                        current->data(0, Qt::UserRole).toString());
                });

            detailsWidget->setVisible(false);
            layout->addWidget(detailsWidget, 1);

            QObject::connect(toggleButton, &QToolButton::toggled, &dialog,
                [&](bool expanded)
                {
                    detailsWidget->setVisible(expanded);
                    toggleButton->setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);

                    if (expanded)
                        dialog.resize(900, 560);
                    else
                        dialog.adjustSize();
                });
        }

        auto* buttons = new QDialogButtonBox(&dialog);

        auto* copyReportButton = buttons->addButton("Copy report", QDialogButtonBox::ActionRole);

        buttons->addButton(QDialogButtonBox::Ok);

        QObject::connect(copyReportButton, &QPushButton::clicked, &dialog,
            [&]
            {
                copyExceptionReportToClipboard(title, reason, stackTrace);
            });

        QObject::connect(
            buttons,
            &QDialogButtonBox::accepted,
            &dialog,
            &QDialog::accept);

        layout->addWidget(buttons);

        dialog.adjustSize();
        dialog.exec();
    }
}

void exceptionMessageBox(const QString& title, const QString& reason, QWidget* parent)
{
    const auto stackTrace = mv::errors().getDebugStackTrace();

    exceptionDialog(title, reason, stackTrace, parent);

    qDebug() << title << reason;

    for (const auto& frame : stackTrace)
        qDebug() << stackFrameToString(frame);
}

void exceptionMessageBox(const QString& title, QWidget* parent)
{
	exceptionMessageBox(title, "An unhandled error occurred.", parent);
}

void exceptionMessageBox(const QString& title, const std::exception& exception, QWidget* parent)
{
	exceptionMessageBox(title, exception.what(), parent);
}

void exceptionMessageBox(const QString& title, const ManiVaultException& exception, QWidget* parent)
{
    const auto stackTrace = exception.getStackTrace();

    exceptionDialog(title, exception.getMessage(), stackTrace, parent);

    qDebug() << title << exception.getMessage();

    for (const auto& frame : stackTrace)
        qDebug() << stackFrameToString(frame);
}

}
