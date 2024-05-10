// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageWidget.h"

#include <QDebug>

LearningPageWidget::LearningPageWidget(QWidget* parent /*= nullptr*/) :
    PageWidget(parent),
    _contentWidget(this)
{
    getContentLayout().addWidget(&_contentWidget, Qt::AlignTop);
}
