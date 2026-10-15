// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageContentWidget.h"

#include "CoreInterface.h"

#include <qguiapplication.h>
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

    setBackgroundRole(QPalette::Window);
}

QLabel* PageContentWidget::createHeaderLabel(const QString& title, const QString& tooltip)
{
    auto headerLabel = new QLabel(title);

    headerLabel->setAlignment(Qt::AlignLeft);
    headerLabel->setToolTip(tooltip);

    const auto updateCustomStyle = [headerLabel]() -> void {
        headerLabel->setStyleSheet(QString(
            "QLabel {"
                "color: %1;"
                "font-weight: 200;"
                "font-size: 13pt;"
            "}").arg(qApp->palette().text().color().name()));
    };

    updateCustomStyle();

    connect(&mv::theme(), &mv::AbstractThemeManager::colorSchemeChanged, headerLabel, updateCustomStyle);

    return headerLabel;
}

