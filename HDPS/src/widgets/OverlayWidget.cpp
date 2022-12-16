#include "OverlayWidget.h"

#include <QDebug>
#include <QResizeEvent>

#ifdef _DEBUG
    #define OVERLAY_WIDGET_VERBOSE
#endif

using namespace hdps::util;

namespace hdps::gui
{

OverlayWidget::OverlayWidget(QWidget* parent) :
    QWidget(parent),
    _widgetFader(this, this, 0.0f, 0.0f, 1.0f, 120, 60)
{
    initialize();

    connect(&_widgetFader, &WidgetFader::fadedIn, this, &OverlayWidget::shown);
    connect(&_widgetFader, &WidgetFader::fadedOut, this, &OverlayWidget::hidden);
}

bool OverlayWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) != parent())
                break;

            setFixedSize(static_cast<QResizeEvent*>(event)->size());

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void OverlayWidget::show()
{
#ifdef OVERLAY_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _widgetFader.fadeIn();
}

void OverlayWidget::hide()
{
#ifdef OVERLAY_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _widgetFader.fadeOut();
}

hdps::util::WidgetFader& OverlayWidget::getWidgetFader()
{
    return _widgetFader;
}

void OverlayWidget::initialize()
{
    QWidget::show();

    parent()->installEventFilter(this);

    setObjectName("OverlayWidget");
}

}
