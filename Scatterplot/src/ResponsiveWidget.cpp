#include "ResponsiveWidget.h"

ResponsiveWidget::ResponsiveWidget(QWidget* widget, VisibilityCallback visibilityCallback) :
    _widget(widget),
    _visibilityCallback(visibilityCallback)
{
}

bool ResponsiveWidget::shouldBeVisible(const QSize& sourceWidgetSize) const
{
    return _visibilityCallback(sourceWidgetSize);
}