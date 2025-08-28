// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ActionOverlayWidget.h"

#include <QDebug>

#ifdef _DEBUG
    #define ACTION_OVERLAY_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

ActionOverlayWidget::ActionOverlayWidget(QWidget* target, const QPointer<WidgetAction>& action, const Qt::Alignment& alignment /*= Qt::AlignTop | Qt::AlignLeft*/) :
    OverlayWidget(target),
    _action(action)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(alignment);

	auto actionWidget = action->createWidget(this);

    addMouseEventReceiverWidget(actionWidget);

    layout->addWidget(actionWidget);

	setLayout(layout);
}

}
