#include "ToolbarItemWidget.h"
#include "ToolbarWidget.h"
#include "Application.h"

#include <QResizeEvent>

namespace hdps {

namespace gui {

ToolbarItemWidget::ToolbarItemWidget(ToolbarWidget* toolbarWidget) :
    QWidget(toolbarWidget),
    _toolbarWidget(toolbarWidget),
    _state(0),
    _sizeAnimation(this),
    _fadeableWidget(toolbarWidget, this)
{
}

std::int32_t ToolbarItemWidget::getState() const
{
    return _state;
}

void ToolbarItemWidget::setState(std::int32_t state)
{
    const auto previousState = _state;

    _state = state;

    stateChanged(previousState, _state);
}

void ToolbarItemWidget::stateChanged(std::int32_t previousState, std::int32_t currentState)
{

}

void ToolbarItemWidget::show()
{
    setVisibility(true);
}

void ToolbarItemWidget::hide()
{
    setVisibility(false);
}

void ToolbarItemWidget::setVisibility(bool visible)
{
    _fadeableWidget.setOpacity(visible ? 1.0f : 0.0f);
}

}
}
