// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Divider.h"

#include <QFrame>
#include <QString>
#include <QWidget>

namespace mv {

namespace gui {

QWidget* createHorizontalDivider()
{
    auto horizontalLine = new QFrame();

    horizontalLine->setObjectName(QString::fromUtf8("HorizontalLine"));
    horizontalLine->setFrameShape(QFrame::HLine);
    horizontalLine->setFrameShadow(QFrame::Sunken);

    return horizontalLine;
}

QWidget* createVerticalDivider()
{
    auto verticalLine = new QFrame();

    verticalLine->setObjectName(QString::fromUtf8("VerticalLine"));
    verticalLine->setFrameShape(QFrame::VLine);
    verticalLine->setFrameShadow(QFrame::Sunken);

    return verticalLine;
}

}
}
