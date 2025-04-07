// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageWidget.h"
#include "ManiVaultVersion.h"

#include <QDebug>

StartPageWidget::StartPageWidget(QWidget* parent /*= nullptr*/) :
    PageWidget(QString("<b>ManiVault</b> v%1.%2.%3-%4").arg(QString::number(MV_VERSION_MAJOR), QString::number(MV_VERSION_MINOR), QString::number(MV_VERSION_PATCH), QString(MV_VERSION_SUFFIX.data())), parent),
    _startPageContentWidget(this)
{
    getContentLayout().addWidget(&_startPageContentWidget, Qt::AlignTop);
}

void StartPageWidget::showEvent(QShowEvent* showEvent)
{
    _startPageContentWidget.updateActions();
}
