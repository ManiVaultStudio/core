// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageWidget.h"

#include <QDebug>

StartPageWidget::StartPageWidget(QWidget* parent /*= nullptr*/) :
    PageWidget(parent),
    _startPageContentWidget(this)
{
    getContentLayout().addWidget(&_startPageContentWidget, 3);
}

void StartPageWidget::showEvent(QShowEvent* showEvent)
{
    _startPageContentWidget.updateActions();
}
