// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionHighlightWidget.h"

namespace mv::gui {

WidgetActionHighlightWidget::WidgetActionHighlightWidget(QWidget* parent, WidgetAction* action) :
    OverlayWidget(parent, 0.0f),
    _action(action),
    _widgetFader(this, this, 0.f, 0.f, 1.f, 120, 60)
{
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setStyleSheet(QString("background-color: %1;").arg(palette().highlight().color().name()));
}

WidgetAction* WidgetActionHighlightWidget::getAction()
{
    return _action;
}

void WidgetActionHighlightWidget::setAction(WidgetAction* action)
{
    if (_action != nullptr)
        disconnect(_action, &WidgetAction::highlightingChanged, this, nullptr);

    _action = action;

    if (_action == nullptr)
        return;

    if (_action->isPublic())
        return;

    connect(_action, &WidgetAction::highlightingChanged, this, &WidgetActionHighlightWidget::highlightingChanged);

    highlightingChanged(_action->getHighlighting());
}

void WidgetActionHighlightWidget::highlightingChanged(const WidgetAction::HighlightOption& highlighting)
{
    switch (highlighting)
    {
        case WidgetAction::HighlightOption::None:
        {
            _widgetFader.setOpacity(0.0f, 500);

            break;
        }

        case WidgetAction::HighlightOption::Moderate:
        {
            _widgetFader.setOpacity(0.25f, 200);
            
            break;
        }

        case WidgetAction::HighlightOption::Strong:
        {
            _widgetFader.setOpacity(0.6f, 200);

            break;
        }

        default:
            break;
    }
}

}
