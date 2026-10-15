// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointsAction.h"

#include "PointData.h"
#include "PointsViewWidget.h"

#include <QVBoxLayout>

PointsAction::PointsAction(QObject* parent, const mv::Dataset<Points>& points) :
    GroupAction(parent, "Points", true),
    _points(points)
{
    setToolTip("Inspect point data through lazy table and facade access");
}

QWidget* PointsAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new Widget(parent, this);
}

const mv::Dataset<Points>& PointsAction::getPoints() const
{
    return _points;
}

PointsAction::Widget::Widget(QWidget* parent, PointsAction* pointsAction) :
    WidgetActionWidget(parent, pointsAction)
{
    auto* layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    auto* pointsViewWidget = new PointsViewWidget(this, nullptr, false);
    pointsViewWidget->setPoints(pointsAction->getPoints());
    layout->addWidget(pointsViewWidget);

    setLayout(layout);
}
