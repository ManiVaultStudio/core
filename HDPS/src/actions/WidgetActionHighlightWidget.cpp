#include "WidgetActionHighlightWidget.h"
#include "WidgetAction.h"

namespace hdps::gui {

WidgetActionHighlightWidget::WidgetActionHighlightWidget(QWidget* parent, WidgetAction* action) :
    OverlayWidget(parent, 0.0f),
    _action(action)
{
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setStyleSheet("background-color: gray;");

    auto& widgetFader = getWidgetOverlayer().getWidgetFader();

    widgetFader.setMaximumOpacity(0.3f);
    widgetFader.setFadeInDuration(250);
    widgetFader.setFadeOutDuration(500);
}

WidgetAction* WidgetActionHighlightWidget::getAction()
{
    return _action;
}

void WidgetActionHighlightWidget::setAction(WidgetAction* action)
{
    if (_action != nullptr)
        disconnect(_action, &WidgetAction::highlightedChanged, this, nullptr);

    _action = action;

    if (_action == nullptr)
        return;

    connect(_action, &WidgetAction::highlightedChanged, this, &WidgetActionHighlightWidget::updateHighlight);

    updateHighlight();
}

void WidgetActionHighlightWidget::updateHighlight()
{
    auto& widgetFader = getWidgetOverlayer().getWidgetFader();

    if (_action->isHighlighted())
        widgetFader.fadeIn();
    else
        widgetFader.fadeOut();
}

}