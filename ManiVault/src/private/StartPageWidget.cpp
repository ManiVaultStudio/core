// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageWidget.h"
#include "ManiVaultVersion.h"

#include "CoreInterface.h"

#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;

StartPageWidget::StartPageWidget(QWidget* parent /*= nullptr*/) :
    PageWidget("StartPage", parent),
    _startPageContentWidget(this)
{
    getContentLayout().addWidget(&_startPageContentWidget, Qt::AlignTop);
}

void StartPageWidget::showEvent(QShowEvent* showEvent)
{
    _startPageContentWidget.updateActions();
}

void StartPageWidget::fromVariantMap(const QVariantMap& variantMap)
{
    _startPageContentWidget.fromParentVariantMap(variantMap);

    PageWidget::fromVariantMap(variantMap);
}

QVariantMap StartPageWidget::toVariantMap() const
{
    auto variantMap = PageWidget::toVariantMap();

	_startPageContentWidget.insertIntoVariantMap(variantMap);

	return variantMap;
}
