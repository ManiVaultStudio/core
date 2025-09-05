// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ActionOverlayWidget.h"

#include <QDebug>
#include <QMainWindow>

#ifdef _DEBUG
    #define ACTION_OVERLAY_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

ActionOverlayWidget::ActionOverlayWidget(QWidget* target, const QPointer<WidgetAction>& action, const Qt::Alignment& alignment /*= Qt::AlignTop | Qt::AlignLeft*/) :
    OverlayWidget(target),
    _action(action),
    _actionWidget(action->createWidget(this))
{
    //setAttribute(Qt::WA_TransparentForMouseEvents, true);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(alignment);

    layout->addWidget(_actionWidget);

	setLayout(layout);
    
}

void ActionOverlayWidget::setTargetWidget(QWidget* targetWidget)
{
    Q_ASSERT(targetWidget);

    if (targetWidget == nullptr)
        return;

    setParent(targetWidget);

    getWidgetOverlayer().setTargetWidget(targetWidget);
}

}
