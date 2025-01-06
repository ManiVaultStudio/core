// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageContentWidget.h"
#include "PageWidget.h"

#include <QLabel>
#include <QString>

PageContentWidget::PageContentWidget(const Qt::Orientation& orientation, QWidget* parent /*= nullptr*/) :
    QWidget(parent)
{
    setObjectName("PageContentWidget");
    setAutoFillBackground(true);

    _columnsLayout.setContentsMargins(35, 35, 35, 35);
    _rowsLayout.setContentsMargins(35, 35, 35, 35);

    _columnsLayout.setSpacing(40);

    switch (orientation) {
        case Qt::Horizontal:
            _mainLayout.addLayout(&_columnsLayout);
            break;

        case Qt::Vertical:
            _mainLayout.addLayout(&_rowsLayout);
            break;
    }

    setLayout(&_mainLayout);

    updateCustomStyle();
}

bool PageContentWidget::event(QEvent* event)
{
    if (event->type() == QEvent::ApplicationPaletteChange)
        updateCustomStyle();

    return QWidget::event(event);
}

QLabel* PageContentWidget::createHeaderLabel(const QString& title, const QString& tooltip)
{
    auto label = new QLabel(title);

    label->setAlignment(Qt::AlignLeft);
    label->setStyleSheet("QLabel { font-weight: 200; font-size: 13pt; }");
    label->setToolTip(tooltip);

    return label;
}

void PageContentWidget::updateCustomStyle()
{
    PageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}
