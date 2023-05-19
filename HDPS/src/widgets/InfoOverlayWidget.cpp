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
    setColors(Qt::black, Qt::lightGray);

    _widgetOverlayer.getWidgetFader().setOpacity(0.0f);
}

InfoOverlayWidget::InfoOverlayWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description /*= ""*/, const QColor foregroundColor /*= Qt::black*/, const QColor backgroundColor /*= Qt::lightGray*/) :
    InfoWidget(parent),
    _widgetOverlayer(this, this, parent)
{
    set(icon, title, description);
    setColors(foregroundColor, backgroundColor);
}

hdps::util::WidgetFader& InfoOverlayWidget::getWidgetFader()
{
    return _widgetOverlayer.getWidgetFader();
}

}
