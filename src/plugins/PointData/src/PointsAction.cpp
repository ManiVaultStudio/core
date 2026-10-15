// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointsAction.h"

#include "PointsViewWidget.h"

#include <QVBoxLayout>

PointsAction::PointsAction(QObject* parent) :
    GroupAction(parent, "Points", true)
{
    setToolTip("Inspect point data through lazy table and facade access");
}

QWidget* PointsAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new Widget(parent, this);
}

PointsAction::Widget::Widget(QWidget* parent, PointsAction* pointsAction) :
    WidgetActionWidget(parent, pointsAction)
{
    auto* layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new PointsViewWidget(this));

    setLayout(layout);
}
