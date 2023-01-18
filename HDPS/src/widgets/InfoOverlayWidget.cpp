#include "InfoOverlayWidget.h"

#include <QDebug>

#ifdef _DEBUG
    #define INFO_OVERLAY_WIDGET_VERBOSE
#endif

namespace hdps::gui
{

InfoOverlayWidget::InfoOverlayWidget(QWidget* parent) :
    InfoWidget(parent),
    _widgetOverlayer(this, this, parent)
{
    setColors(Qt::lightGray, Qt::black);
}

InfoOverlayWidget::InfoOverlayWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description /*= ""*/, const QColor backgroundColor /*= Qt::lightgray*/, const QColor textColor /*= Qt::black*/) :
    InfoWidget(parent),
    _widgetOverlayer(this, this, parent)
{
    set(icon, title, description);
    setColors(backgroundColor, textColor);
}

hdps::util::WidgetFader& InfoOverlayWidget::getWidgetFader()
{
    return _widgetOverlayer.getWidgetFader();
}

}
