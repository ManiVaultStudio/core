#include "WidgetResizeEventProxy.h"

namespace hdps
{

namespace util
{

WidgetResizeEventProxy::WidgetResizeEventProxy(QWidget* parent) :
    QObject(parent),
    _initialized(false),
    _sourceWidget(nullptr),
    _callback()
{
}

bool WidgetResizeEventProxy::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() != QEvent::Resize)
        return QObject::eventFilter(target, event);

    _callback(_sourceWidget->size());

    return QObject::eventFilter(target, event);
}

void WidgetResizeEventProxy::initialize(QWidget* sourceWidget, Callback callback)
{
    if (_initialized)
        return;

    Q_ASSERT(sourceWidget != nullptr);
    Q_ASSERT(callback);

    _sourceWidget   = sourceWidget;
    _callback       = callback;

    _sourceWidget->installEventFilter(this);

    _callback(_sourceWidget->size());

    _initialized = true;
}

}
}