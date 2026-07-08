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

namespace mv::util {

namespace {

void copyExceptionReportToClipboard(const QString& title, const QString& reason, const QStringList& stackTrace)
{
	QString report;

	report += title + "\n\n";
	report += reason + "\n\n";

	if (!stackTrace.isEmpty()) {
		report += "Stack trace:\n";
		report += stackTrace.join('\n');
	}

	QApplication::clipboard()->setText(report);
}

void exceptionDialog(
    const QString& title,
    const QString& reason,
    const QStringList& stackTrace,
    QWidget* parent)
{
    QDialog dialog(parent);

    dialog.setWindowTitle(title);
    dialog.setModal(true);
    dialog.setMinimumWidth(700);

    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(18, 16, 18, 14);
    layout->setSpacing(12);

    auto* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(12);

    auto* iconLabel = new QLabel(&dialog);
    iconLabel->setFixedSize(24, 24);
    iconLabel->setPixmap(
        QApplication::style()
        ->standardIcon(QStyle::SP_MessageBoxWarning)
        .pixmap(24, 24));

    auto* reasonLabel = new QLabel(reason, &dialog);
    reasonLabel->setWordWrap(true);
    reasonLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    reasonLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    headerLayout->addWidget(iconLabel, 0, Qt::AlignTop);
    headerLayout->addWidget(reasonLabel, 1, Qt::AlignVCenter);

    layout->addLayout(headerLayout);

    auto* detailsWidget = new QWidget(&dialog);
    auto* detailsLayout = new QVBoxLayout(detailsWidget);
    detailsLayout->setContentsMargins(0, 0, 0, 0);
    detailsLayout->setSpacing(0);

    auto* stackTraceEdit = new QPlainTextEdit(detailsWidget);
    stackTraceEdit->setReadOnly(true);
    stackTraceEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    stackTraceEdit->setPlainText(stackTrace.join('\n'));
    stackTraceEdit->setMinimumHeight(320);
    stackTraceEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    detailsLayout->addWidget(stackTraceEdit);
    detailsWidget->setVisible(false);

    QToolButton* toggleButton = nullptr;

    if (!stackTrace.isEmpty()) {
        toggleButton = new QToolButton(&dialog);
        toggleButton->setText("Show technical details");
        toggleButton->setCheckable(true);
        toggleButton->setChecked(false);
        toggleButton->setArrowType(Qt::RightArrow);
        toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        layout->addWidget(toggleButton);
        layout->addWidget(detailsWidget, 1);
    }

    auto* buttons = new QDialogButtonBox(&dialog);

    auto* copyReportButton =
        buttons->addButton("Copy report", QDialogButtonBox::ActionRole);

    buttons->addButton(QDialogButtonBox::Ok);

    QObject::connect(copyReportButton, &QPushButton::clicked, &dialog, [&] {
        copyExceptionReportToClipboard(title, reason, stackTrace);
        });

    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    layout->addWidget(buttons);

    if (toggleButton) {
        QObject::connect(toggleButton, &QToolButton::toggled, &dialog, [&](bool expanded) {
            detailsWidget->setVisible(expanded);

            toggleButton->setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);
            toggleButton->setText(expanded ? "Hide technical details" : "Show technical details");

            dialog.setUpdatesEnabled(false);

            if (expanded)
                dialog.resize(900, 560);
            else
                dialog.adjustSize();

            dialog.setUpdatesEnabled(true);
            });
    }

    dialog.adjustSize();
    dialog.exec();
}

}

void exceptionMessageBox(const QString& title, const QString& reason, QWidget* parent)
{
	const auto stackTraceLines = mv::errors().getDebugStackTrace().split('\n', Qt::SkipEmptyParts);

	exceptionDialog(title, reason, stackTraceLines, parent);

	qDebug() << title << reason;

	for (const auto& frame : stackTraceLines)
		qDebug() << frame;
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
    const auto stackTraceLines = exception.getStackTrace();

    exceptionDialog(title, exception.getMessage(), stackTraceLines, parent);
}

}
