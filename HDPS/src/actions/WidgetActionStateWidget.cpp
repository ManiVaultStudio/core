#include "WidgetActionStateWidget.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

WidgetActionStateWidget::WidgetActionStateWidget(QWidget* parent, WidgetAction* widgetAction, const std::int32_t& priority /*= 0*/, const WidgetActionWidget::State& state /*= WidgetActionWidget::State::Compact*/) :
    QWidget(parent),
    _widgetAction(widgetAction),
    _state(state),
    _priority(priority),
    _standardWidget(widgetAction->createWidget(this)),
    _compactWidget(widgetAction->createCollapsedWidget(this))
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->addWidget(_standardWidget);
    layout->addWidget(_compactWidget);

    setState(state);
    setLayout(layout);
}

WidgetActionWidget::State WidgetActionStateWidget::getState() const
{
    return _state;
}

void WidgetActionStateWidget::setState(const WidgetActionWidget::State& state)
{
    _standardWidget->setVisible(state == WidgetActionWidget::State::Standard);
    _compactWidget->setVisible(state == WidgetActionWidget::State::Collapsed);
}

std::int32_t WidgetActionStateWidget::getPriority() const
{
    return _priority;
}

void WidgetActionStateWidget::setPriority(const std::int32_t& priority)
{
    _priority = priority;
}

QSize WidgetActionStateWidget::getSizeHint(const WidgetActionWidget::State& state) const
{
    switch (state)
    {
        case WidgetActionWidget::State::Standard:
            return _standardWidget->sizeHint();

        case WidgetActionWidget::State::Collapsed:
            return _compactWidget->sizeHint();

        default:
            break;
    }

    return QSize();
}

}
}
