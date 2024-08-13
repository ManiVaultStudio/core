// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewTooltipAction.h"

using namespace mv::util;

namespace mv::gui {

ViewTooltipAction::ViewTooltipAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _viewWidget(nullptr),
    _tooltipGeneratorFunction(),
    _roiSizeAction(this, "ROI size")
{
    addAction(&_roiSizeAction);
}

void ViewTooltipAction::initialize(QWidget* viewWidget, const TooltipGeneratorFunction& tooltipGeneratorFunction)
{
    if (_viewWidget)
        _viewWidget->removeEventFilter(this);

    Q_ASSERT(viewWidget);

    if (!viewWidget)
        return;

    _viewWidget                 = viewWidget;
    _tooltipGeneratorFunction   = tooltipGeneratorFunction;

    _viewWidget->setMouseTracking(true);
    _viewWidget->installEventFilter(this);

    drawTooltip();
}

bool ViewTooltipAction::eventFilter(QObject* target, QEvent* event)
{
    if (target != _viewWidget)
        return VerticalGroupAction::eventFilter(target, event);

    switch (event->type())
    {
        case QEvent::MouseMove:
        {
            drawTooltip();
            break;
        }

        default:
            break;
    }

    return VerticalGroupAction::eventFilter(target, event);
}

void ViewTooltipAction::drawTooltip()
{
    if (!isChecked())
        return;

    qDebug() << __FUNCTION__;
}

}
