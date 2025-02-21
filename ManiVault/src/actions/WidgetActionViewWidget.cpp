// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionViewWidget.h"

#include "CoreInterface.h"
#include "WidgetAction.h"
#include "WidgetActionCollapsedWidget.h"
#include "WidgetActionHighlightWidget.h"
#include "WidgetActionMimeData.h"

#include <QDebug>
#include <QDragEnterEvent>
#include <QWindow>

#ifdef _DEBUG
    //#define WIDGET_ACTION_VIEW_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui {

WidgetActionViewWidget::WidgetActionViewWidget(QWidget* parent, WidgetAction* action, std::int32_t widgetFlags /*= 0*/) :
    QWidget(parent),
    _action(nullptr),
    _widgetFlags(widgetFlags),
    _highlightWidget(new WidgetActionHighlightWidget(this, action)),
    _badgeOverlayWidget(nullptr),
    _cachedHighlighting(0)
{
    setAcceptDrops(true);
}

WidgetAction* WidgetActionViewWidget::getAction() const
{
    return _action;
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
        setToolTip(_action->toolTip());
    };

    connect(_action, &WidgetAction::changed, this, update);

    update();

    const auto enabledChanged = [this]() -> void {
        setEnabled(_action->isEnabled());
    };

    enabledChanged();

    connect(_action, &WidgetAction::enabledChanged, this, enabledChanged);

    const auto visibleChanged = [this]() -> void {
        setVisible(_action->isVisible());
    };

    visibleChanged();

    connect(_action, &WidgetAction::visibleChanged, this, visibleChanged);

    const auto badgeEnabledChanged = [this]() -> void {
        if (_action->getBadge().getEnabled())
            enableBadge();
        else
            disableBadge();
    };

    badgeEnabledChanged();

    connect(&_action->getBadge(), &Badge::enabledChanged, this, badgeEnabledChanged);
}

std::int32_t WidgetActionViewWidget::getWidgetFlags() const
{
    return _widgetFlags;
}

bool WidgetActionViewWidget::isPopup() const
{
    return getWidgetFlags() & PopupLayout;
}

void WidgetActionViewWidget::mousePressEvent(QMouseEvent* event)
{
    if (isPopup() && !rect().contains(event->pos()))
        close();
    else
        QWidget::mousePressEvent(event);
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
        mv::actions().connectPrivateActionToPublicAction(getAction(), actionMimeData->getAction(), true);
    }
    else {
        if (getAction()->isConnected())
            mv::actions().connectPrivateActionToPublicAction(actionMimeData->getAction(), getAction()->getPublicAction(), true);
        else
            mv::actions().connectPrivateActions(actionMimeData->getAction(), getAction());
    }
}

void WidgetActionViewWidget::enableBadge()
{
    Q_ASSERT(_action);

    if (!_action)
        return;

#ifdef WIDGET_ACTION_VIEW_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << (_action ? _action->text() : "");
#endif

    auto currentParentWidget = parentWidget();

    WidgetActionCollapsedWidget* widgetActionCollapsedWidget = nullptr;

    while (currentParentWidget) {
        if (widgetActionCollapsedWidget = dynamic_cast<WidgetActionCollapsedWidget*>(currentParentWidget))
            break;

        currentParentWidget = currentParentWidget->parentWidget();
    }

    if (widgetActionCollapsedWidget)
        if (widgetActionCollapsedWidget->getAction() == _action)
            return;

    if (!isPopup()) {
        _badgeOverlayWidget.reset(new WidgetActionBadgeOverlayWidget(this, _action));
        _badgeOverlayWidget->show();
    }
}

void WidgetActionViewWidget::disableBadge()
{
    Q_ASSERT(_action);

    if (!_action)
        return;

#ifdef WIDGET_ACTION_VIEW_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << (_action ? _action->text() : "");
#endif

    _badgeOverlayWidget.reset();
}

}
