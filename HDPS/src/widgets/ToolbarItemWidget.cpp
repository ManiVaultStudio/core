#include "ToolbarItemWidget.h"

#include <QDebug>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

ToolbarItemWidget::ToolbarItemWidget(QWidget* parent, GetWidgetFN getWidget, const std::int32_t& priority /*= 0*/, const WidgetActionWidget::State& state /*= WidgetActionWidget::State::Compact*/) :
    QWidget(parent),
    _state(state),
    _priority(priority),
    _getWidget(getWidget),
    _layout(),
    _collapsedWidget(this, nullptr)
{
    _layout.setContentsMargins(0, 0, 0, 0);

    setLayout(&_layout);
    setState(state);

    _layout.addWidget(&_collapsedWidget);
}

WidgetActionWidget::State ToolbarItemWidget::getState() const
{
    return _state;
}

void ToolbarItemWidget::setState(const WidgetActionWidget::State& state)
{
    _state = state;

    /*
    auto layoutItem = layout()->itemAt(0);

    if (layoutItem) {
        auto widget = layoutItem->widget();

        if (widget)
            layout()->removeItem(layoutItem);
    }

    auto widget = _getWidget(_state);
    */
    
    qDebug() << "ToolbarItemWidget::setState()" << _state;

    _collapsedWidget.setVisible(_state == WidgetActionWidget::Collapsed);

    _layout.insertWidget(1, _getWidget(_state).get());
}

std::int32_t ToolbarItemWidget::getPriority() const
{
    return _priority;
}

void ToolbarItemWidget::setPriority(const std::int32_t& priority)
{
    _priority = priority;
}

QSize ToolbarItemWidget::getSizeHint(const WidgetActionWidget::State& state) const
{
    auto widget = _getWidget(_state);

    const auto sizeHint = widget->sizeHint();

    //delete widget;

    //qDebug() << "WidgetActionWidget::getSizeHint()" << _getWidget(_state)->sizeHint();
    return sizeHint;
}

}
}
