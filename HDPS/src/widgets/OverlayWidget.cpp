#include "OverlayWidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>

#ifdef _DEBUG
    #define OVERLAY_WIDGET_VERBOSE
#endif

namespace hdps::gui
{

OverlayWidget::OverlayWidget(QWidget* parent) :
    QWidget(parent),
    _widgetOverlayer(this, this, parent)
{
}

hdps::util::WidgetOverlayer& OverlayWidget::getWidgetOverlayer()
{
    return _widgetOverlayer;
}

}
