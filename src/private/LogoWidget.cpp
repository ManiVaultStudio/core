// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LogoWidget.h"

LogoWidget::LogoWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent)
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    const int pixelRatio = devicePixelRatio();

    QString iconName = ":/Images/AppBackground256";

    if (pixelRatio > 1)
        iconName = ":/Images/AppBackground512";

    if (pixelRatio > 2)
        iconName = ":/Images/AppBackground1024";

    _headerLabel.setPixmap(QPixmap(iconName).scaled(200, 200));
    _headerLabel.setAlignment(Qt::AlignCenter);

    _layout.setContentsMargins(36, 36, 36, 36);
    _layout.addWidget(&_headerLabel);
}
