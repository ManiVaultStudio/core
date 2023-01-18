#include "WidgetOverlayer.h"

#include <QDebug>
#include <QWidget>
#include <QVBoxLayout>
#include <QResizeEvent>

#ifdef _DEBUG
    #define WIDGET_OVERLAYER_VERBOSE
#endif

namespace hdps::util {

WidgetOverlayer::WidgetOverlayer(QObject* parent, QWidget* sourceWidget, QWidget* targetWidget) :
    QObject(parent),
    _widgetFader(this, sourceWidget, 0.0f, 0.0f, 1.0f, 120, 60),
    _sourceWidget(sourceWidget),
    _targetWidget(targetWidget)
{
    Q_ASSERT(_sourceWidget != nullptr);
    Q_ASSERT(_targetWidget != nullptr);
    
    setObjectName("WidgetOverlayer");

    _sourceWidget->setAttribute(Qt::WA_TransparentForMouseEvents);

    _widgetFader.setMaximumOpacity(0.35f);

    _sourceWidget->show();

    _sourceWidget->installEventFilter(this);
    _targetWidget->installEventFilter(this);
}

bool WidgetOverlayer::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) != _targetWidget)
                break;

            _sourceWidget->setFixedSize(static_cast<QResizeEvent*>(event)->size());

            break;
        }

        case QEvent::Show:
        {
            if (dynamic_cast<QWidget*>(target) != _sourceWidget)
                break;

            if(_widgetFader.isFadedIn())
                break;

#ifdef WIDGET_OVERLAYER_VERBOSE
            qDebug() << __FUNCTION__;
#endif

            _widgetFader.fadeIn();

            break;
        }

        case QEvent::Hide:
        {
            if (dynamic_cast<QWidget*>(target) != _sourceWidget)
                break;

            if (_widgetFader.isFadedOut())
                break;

#ifdef WIDGET_OVERLAYER_VERBOSE
            qDebug() << __FUNCTION__;
#endif

            _widgetFader.fadeOut();

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

hdps::util::WidgetFader& WidgetOverlayer::getWidgetFader()
{
    return _widgetFader;
}

}
