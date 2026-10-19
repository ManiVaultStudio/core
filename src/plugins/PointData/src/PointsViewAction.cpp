// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointData.h"
#include "PointsViewAction.h"

#include "PointsViewWidget.h"

#include <QVBoxLayout>

PointsViewAction::PointsViewAction(QObject* parent, const mv::Dataset<Points>& points) :
    WidgetAction(parent, "Points view"),
    _points(points)
{
    setToolTip("Inspect point data through lazy table and facade access");
}

QWidget* PointsViewAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new Widget(parent, this);
}

const mv::Dataset<Points>& PointsViewAction::getPoints() const
{
    return _points;
}

PointsViewWidget::ViewState PointsViewAction::getViewState() const
{
    return _pointsViewWidget != nullptr ? _pointsViewWidget->getViewState() : PointsViewWidget::ViewState();
}

PointsViewAction::Widget::Widget(QWidget* parent, PointsViewAction* pointsViewAction) :
    WidgetActionWidget(parent, pointsViewAction)
{
    auto* pointsViewWidget = new PointsViewWidget(this, false);

    pointsViewAction->_pointsViewWidget = pointsViewWidget;
    pointsViewWidget->setPoints(pointsViewAction->getPoints());
    pointsViewWidget->layout()->setContentsMargins(0, 0, 0, 0);

    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(pointsViewWidget);

    setLayout(layout);
}
