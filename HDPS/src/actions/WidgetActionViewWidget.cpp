// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionViewWidget.h"
#include "WidgetActionHighlightWidget.h"
#include "WidgetActionMimeData.h"
#include "WidgetAction.h"
#include "CoreInterface.h"

#include <QDebug>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>

namespace hdps::gui {

WidgetActionViewWidget::WidgetActionViewWidget(QWidget* parent, WidgetAction* action) :
    QWidget(parent),
    _action(nullptr),
    _highlightWidget(new WidgetActionHighlightWidget(this, action)),
    _cachedHighlighting(0)
{
    setAcceptDrops(true);
}

void WidgetActionViewWidget::setAction(WidgetAction* action)
{
    if (_action != nullptr)
        disconnect(_action, &WidgetAction::changed, this, nullptr);

    _action = action;

    _highlightWidget->setAction(action);

    if (_action == nullptr)
        return;

    const auto update = [this]() -> void {
        setEnabled(_action->isEnabled());
        setToolTip(_action->toolTip());
        setVisible(_action->isVisible());
    };

    connect(_action, &WidgetAction::changed, this, update);

    update();
}

WidgetAction* WidgetActionViewWidget::getAction()
{
    return _action;
}

void WidgetActionViewWidget::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
    auto actionMimeData = dynamic_cast<const WidgetActionMimeData*>(dragEnterEvent->mimeData());

    if (actionMimeData == nullptr)
        return;

    if (actionMimeData->getAction() == getAction())
        return;

    if (!getAction()->mayConnect(WidgetAction::Gui))
        return;

    if (actionMimeData->getAction()->getTypeString() != getAction()->getTypeString())
        return;

    if ((actionMimeData->getAction()->isConnected() && getAction()->isConnected()) && (actionMimeData->getAction()->getPublicAction() == getAction()->getPublicAction()))
        return;

    if (getAction()->isConnected() && (getAction()->getPublicAction() == actionMimeData->getAction()))
        return;

    _cachedHighlighting = static_cast<std::int32_t>(getAction()->getHighlighting());

    getAction()->setHighlighting(WidgetAction::HighlightOption::Strong);

    dragEnterEvent->acceptProposedAction();
}

void WidgetActionViewWidget::dragLeaveEvent(QDragLeaveEvent* dragLeaveEvent)
{
    getAction()->setHighlighting(static_cast<WidgetAction::HighlightOption>(_cachedHighlighting));
}

void WidgetActionViewWidget::dropEvent(QDropEvent* dropEvent)
{
    auto actionMimeData = dynamic_cast<const WidgetActionMimeData*>(dropEvent->mimeData());

    if (actionMimeData == nullptr)
        return;

    if (actionMimeData->getAction() == getAction())
        return;

    if (actionMimeData->getAction()->getTypeString() != getAction()->getTypeString())
        return;

    if (actionMimeData->getAction()->isPublic()) {
        hdps::actions().connectPrivateActionToPublicAction(getAction(), actionMimeData->getAction(), true);
    }
    else {
        if (getAction()->isConnected())
            hdps::actions().connectPrivateActionToPublicAction(actionMimeData->getAction(), getAction()->getPublicAction(), true);
        else
            hdps::actions().connectPrivateActions(actionMimeData->getAction(), getAction());
    }
}

}
