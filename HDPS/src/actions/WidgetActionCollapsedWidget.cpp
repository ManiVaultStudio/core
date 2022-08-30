#include "WidgetActionCollapsedWidget.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QPainter>

namespace hdps {

namespace gui {

WidgetActionCollapsedWidget::WidgetActionCollapsedWidget(QWidget* parent, WidgetAction* widgetAction) :
    WidgetActionWidget(parent, widgetAction),
    _layout(),
    _toolButton()
{
    _layout.setContentsMargins(0, 0, 0, 0);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.setIconSize(QSize(12, 12));
    _toolButton.setStyleSheet("QToolButton::menu-indicator { image: none; }");

    _layout.addWidget(&_toolButton);

    setLayout(&_layout);
    setWidgetAction(widgetAction);
}

void WidgetActionCollapsedWidget::setWidgetAction(WidgetAction* widgetAction)
{
    WidgetActionWidget::setWidgetAction(widgetAction);

    if (_widgetAction != nullptr)
        _toolButton.removeAction(_widgetAction);

    if (widgetAction == nullptr)
        return;

    _toolButton.addAction(_widgetAction);

    const auto updateToolButton = [this, widgetAction]() {
        _toolButton.setIcon(widgetAction->icon());
        _toolButton.setToolTip(widgetAction->toolTip());
    };

    connect(_widgetAction, &WidgetAction::changed, this, updateToolButton);

    updateToolButton();
}

void WidgetActionCollapsedWidget::ToolButton::paintEvent(QPaintEvent* paintEvent)
{
    QToolButton::paintEvent(paintEvent);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    const auto margin = 5.0f;

    QPointF center;

    center.setY(height() - margin);
    center.setX(width() - margin);

    painter.setPen(QPen(QBrush(isEnabled() ? Qt::black : Qt::gray), 2.5, Qt::SolidLine, Qt::RoundCap));
    painter.setBrush(Qt::NoBrush);
    painter.drawPoint(center);
}

}
}
