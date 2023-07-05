// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Divider.h"

namespace hdps {

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
