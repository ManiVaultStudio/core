#include "CollapsedWidget.h"

#include <QDebug>
#include <QWidgetAction>
#include <QPainter>

namespace hdps {

namespace gui {

CollapsedWidget::CollapsedWidget(QWidget* parent, QWidget* widget) :
    QToolButton(parent)
{
    //setIcon(widgetAction->icon());
    //setToolTip(widgetAction->toolTip());
    
    auto action = new QWidgetAction(this);

    //action->setDefaultWidget(widget);

    addAction(action);

    setPopupMode(QToolButton::InstantPopup);
    setIconSize(QSize(12, 12));
    setStyleSheet("QToolButton::menu-indicator { image: none; }");
}

void CollapsedWidget::CollapsedWidget::paintEvent(QPaintEvent* paintEvent)
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
