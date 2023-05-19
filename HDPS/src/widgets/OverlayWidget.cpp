#include "OverlayWidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>

#ifdef _DEBUG
    #define OVERLAY_WIDGET_VERBOSE
#endif

namespace hdps::gui
{

OverlayWidget::OverlayWidget(QWidget* parent, float initialOpacity /*= 1.0f*/) :
    QWidget(parent),
    _widgetOverlayer(this, this, parent, initialOpacity)
{
}

hdps::util::WidgetOverlayer& OverlayWidget::getWidgetOverlayer()
{
    return _widgetOverlayer;
}

}
