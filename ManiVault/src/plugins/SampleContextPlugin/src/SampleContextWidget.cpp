// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SampleContextWidget.h"
#include "SampleContextPlugin.h"

#include <QDebug>

using namespace mv;
using namespace mv::gui;

SampleContextWidget::SampleContextWidget(SampleContextPlugin* sampleContextPlugin, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _sampleContextPlugin(sampleContextPlugin)
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    _layout.setContentsMargins(0, 0, 0, 0);
}
