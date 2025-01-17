// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionCollapsedWidget.h"

#include <QDebug>

namespace mv::gui {

WidgetActionCollapsedWidget::WidgetActionCollapsedWidget(QWidget* parent, WidgetAction* action) :
    WidgetActionViewWidget(parent, action),
    _toolButton(this, action)
{
    initialize(action);
}

WidgetActionCollapsedWidget::WidgetActionCollapsedWidget(QWidget* parent, WidgetAction* action, WidgetConfigurationFunction widgetConfigurationFunction) :
    WidgetActionViewWidget(parent, action),
    _toolButton(this, action, widgetConfigurationFunction)
{
    initialize(action);
}

void WidgetActionCollapsedWidget::setAction(WidgetAction* action)
{
    WidgetActionViewWidget::setAction(action);

    _toolButton.setAction(action);
}

void WidgetActionCollapsedWidget::initialize(WidgetAction* action)
{
    _layout.setContentsMargins(0, 0, 0, 0);

    _layout.addWidget(&_toolButton);

    setLayout(&_layout);

    setAction(action);
}

}
