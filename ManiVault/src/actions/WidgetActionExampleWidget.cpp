// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionExampleWidget.h"

#include <QTextBrowser>
#include <QAbstractTextDocumentLayout>

using namespace mv::util;

namespace mv::gui {

WidgetActionExampleWidget::WidgetActionExampleWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout()
{
    setLayout(&_layout);
}

void WidgetActionExampleWidget::addMarkDownSection(const QString& markdown)
{
    auto markdownLayout         = new QVBoxLayout();
    auto markdownTextBrowser    = new QTextBrowser();

    connect(markdownTextBrowser->document()->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged, this, [this, markdownTextBrowser]() -> void {
        markdownTextBrowser->setFixedHeight(markdownTextBrowser->document()->size().height());
    });

    markdownTextBrowser->setMarkdown(markdown);
    markdownTextBrowser->setOpenExternalLinks(true);
    markdownTextBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    markdownTextBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    markdownTextBrowser->document()->setDocumentMargin(0);
    markdownTextBrowser->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    markdownTextBrowser->setStyleSheet("background-color: transparent; border: none; margin: 0px; padding: 0px;");

    markdownLayout->addWidget(markdownTextBrowser);

    _layout.addLayout(markdownLayout);
}

void WidgetActionExampleWidget::addWidget(QWidget* widget)
{
    _layout.addWidget(widget);
}

}
