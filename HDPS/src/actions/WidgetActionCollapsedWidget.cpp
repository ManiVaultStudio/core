#include "WidgetActionCollapsedWidget.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QPainter>

namespace hdps {

namespace gui {

WidgetActionCollapsedWidget::WidgetActionCollapsedWidget(QWidget* parent, WidgetAction* widgetAction) :
    WidgetActionWidget(parent, widgetAction, WidgetActionWidget::State::Collapsed),
    _layout(),
    _toolButton()
{
    _layout.setMargin(0);

    _toolButton.setIcon(widgetAction->icon());
    _toolButton.setToolTip(widgetAction->toolTip());
    _toolButton.addAction(widgetAction);
    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.setIconSize(QSize(12, 12));
    _toolButton.setFixedSize(QSize(24, 24));
    _toolButton.setStyleSheet("QToolButton::menu-indicator { image: none; }");

    _layout.addWidget(&_toolButton);

    setLayout(&_layout);
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
