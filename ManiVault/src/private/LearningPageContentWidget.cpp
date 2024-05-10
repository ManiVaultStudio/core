// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageContentWidget.h"

#include <QDebug>

LearningPageContentWidget::LearningPageContentWidget(QWidget* parent /*= nullptr*/) :
    PageContentWidget(Qt::Vertical, parent),
    _videosWidget(this),
    _miscellaneousWidget(this)
{
    getRowsLayout().addWidget(&_videosWidget);
    getRowsLayout().addWidget(&_miscellaneousWidget);
}
