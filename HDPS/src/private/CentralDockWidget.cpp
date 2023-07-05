// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "CentralDockWidget.h"

#ifdef _DEBUG
    #define CENTRAL_DOCK_WIDGET_VERBOSE
#endif

CentralDockWidget::CentralDockWidget(QWidget* parent /*= nullptr*/) :
    DockWidget("CentralDockWidget", parent)
{
    setObjectName("CentralDockWidget");
}

QString CentralDockWidget::getTypeString() const
{
    return "CentralDockWidget";
}
