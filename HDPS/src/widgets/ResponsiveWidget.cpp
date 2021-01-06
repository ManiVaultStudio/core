#include "ResponsiveWidget.h"

#include <QWidget>
#include <QResizeEvent>

namespace hdps
{

namespace gui
{

ResponsiveWidget::ResponsiveWidget(QObject* parent, QWidget* sourceWidget, QWidget* targetWidget, const std::uint32_t& visibleFromWidth /*= 0*/) :
    QObject(parent),
    _sourceWidget(sourceWidget),
    _targetWidget(targetWidget),
    _visibleFromWidth(visibleFromWidth)
{
    Q_ASSERT(_sourceWidget != nullptr && _targetWidget != nullptr);

    _sourceWidget->installEventFilter(this);

    sourceWidthChanged(_sourceWidget->size().width());
}

std::uint32_t ResponsiveWidget::getVisibleFromWidth() const
{
    return _visibleFromWidth;
}

void ResponsiveWidget::setVisibleFromWidth(const std::uint32_t& visibleFromWidth)
{
    _visibleFromWidth = visibleFromWidth;
}

bool ResponsiveWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
            sourceWidthChanged(static_cast<QResizeEvent*>(event)->size().width());
            break;
    }

    return QObject::eventFilter(target, event);
}

void ResponsiveWidget::sourceWidthChanged(const std::uint32_t& sourceWidth)
{
    Q_ASSERT(_targetWidget != nullptr);

    _targetWidget->setVisible(sourceWidth >= _visibleFromWidth);
}

QWidget* ResponsiveWidget::getSourceWidget()
{
    return _sourceWidget;
}

void ResponsiveWidget::setSourceWidget(QWidget* sourceWidget)
{
    Q_ASSERT(sourceWidget != nullptr);

    if (sourceWidget == _sourceWidget)
        return;

    _sourceWidget = sourceWidget;
}

QWidget* ResponsiveWidget::getTargetWidget()
{
    return _targetWidget;
}

void ResponsiveWidget::setTargetWidget(QWidget* targetWidget)
{
    Q_ASSERT(targetWidget != nullptr);

    if (targetWidget == _targetWidget)
        return;

    _targetWidget = targetWidget;
}

}
}