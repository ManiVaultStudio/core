// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StatusBarAction.h"

#ifdef _DEBUG
    #define STATUS_BAR_ACTION_VERBOSE
#endif

#include <QDebug>

namespace mv::gui {

StatusBarAction::StatusBarAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _barAction(nullptr),
    _popupAction(nullptr)
{
}

QWidget* StatusBarAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new Widget(parent, this, widgetFlags);
}

void StatusBarAction::setBarAction(WidgetAction* barAction)
{
    Q_ASSERT(barAction != nullptr);

    if (!barAction)
        return;

    _barAction = barAction;
}

void StatusBarAction::setPopupAction(WidgetAction* popupAction)
{
    auto previousPopupAction = _popupAction;

    _popupAction = popupAction;

    popupActionChanged(previousPopupAction, _popupAction);
}

mv::gui::WidgetAction* StatusBarAction::getPopupAction()
{
    return _popupAction;
}

mv::gui::WidgetAction* StatusBarAction::getBarAction()
{
    return _barAction;
}

StatusBarAction::Widget::Widget(QWidget* parent, StatusBarAction* statusBarAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, statusBarAction, widgetFlags),
    _statusBarAction(statusBarAction),
    _toolButton(this, statusBarAction),
    _badgeOverlay(this, statusBarAction)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_toolButton);

    setLayout(layout);

    _toolButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _toolButton.setObjectName("ToolButton");
    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.setIconSize(QSize(16, 16));
    _toolButton.setStyleSheet("QToolButton::menu-indicator { image: none; }");
    _toolButton.setAutoRaise(true);

    if (_statusBarAction->getPopupAction())
        _toolButton.addAction(_statusBarAction->getPopupAction());

    connect(_statusBarAction, &StatusBarAction::popupActionChanged, this, [this](WidgetAction* previousPopupAction, WidgetAction* popupAction) -> void {
        if (previousPopupAction)
            _toolButton.removeAction(previousPopupAction);

        if (popupAction)
            _toolButton.addAction(popupAction);
    });

    _badgeOverlay.raise();
    _badgeOverlay.show();
}

StatusBarAction::Widget::ToolButton::ToolButton(QWidget* parent, StatusBarAction* statusBarAction) :
    QToolButton(parent),
    _statusBarAction(statusBarAction)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(2, 2, 2, 2);
    layout->addWidget(statusBarAction->getBarAction()->createWidget(this));

    setLayout(layout);
}

StatusBarAction::Widget::BadgeOverlay::BadgeOverlay(QWidget* parent, StatusBarAction* statusBarAction) :
    OverlayWidget(parent),
    _statusBarAction(statusBarAction)
{
    connect(&_statusBarAction->getBadge(), &WidgetActionBadge::changed, this, [this]() -> void { update(); });
}

void StatusBarAction::Widget::BadgeOverlay::paintEvent(QPaintEvent* paintEvent)
{
    OverlayWidget::paintEvent(paintEvent);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    if (_statusBarAction && _statusBarAction->getBadge().getEnabled()) {
        const auto& badge = _statusBarAction->getBadge();

        const auto drawSize             = QSize(size().height(), size().height());
        const auto iconPixmapRectangle  = QRectF(QPoint(0, 0), drawSize);
        const auto badgePixmap          = createNumberBadgeOverlayPixmap(badge.getNumber(), badge.getBackgroundColor(), badge.getForegroundColor()).scaled(badge.getScale() * drawSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        const auto badgeRectangle       = QRectF(QPoint(0, 0), badgePixmap.size());
        const auto scaledIconPixmapSize = badge.getScale() * drawSize;
        const auto badgeAlignment       = badge.getAlignment();
        const auto badgeScale           = badge.getScale();

        QPoint origin;

        origin.setY(0);
        origin.setX(width() - scaledIconPixmapSize.width());

        painter.drawPixmap(origin, badgePixmap);
    }
}

}
