#include "WidgetActionHighlightWidget.h"

namespace hdps::gui {

WidgetActionHighlightWidget::WidgetActionHighlightWidget(QWidget* parent, WidgetAction* action) :
    OverlayWidget(parent, 0.0f),
    _action(action)
{
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setStyleSheet(QString("background-color: %1;").arg(palette().highlight().color().name()));
}

WidgetAction* WidgetActionHighlightWidget::getAction()
{
    return _action;
}

void WidgetActionHighlightWidget::setAction(WidgetAction* action)
{
    if (_action != nullptr)
        disconnect(_action, &WidgetAction::highlightingChanged, this, nullptr);

    _action = action;

    if (_action == nullptr)
        return;

    if (_action->isPublic())
        return;

    connect(_action, &WidgetAction::highlightingChanged, this, &WidgetActionHighlightWidget::highlightingChanged);

    highlightingChanged(_action->getHighlighting());
}

void WidgetActionHighlightWidget::highlightingChanged(const WidgetAction::HighlightOption& highlighting)
{
    auto& widgetFader = getWidgetOverlayer().getWidgetFader();

    switch (highlighting)
    {
        case WidgetAction::HighlightOption::None:
            widgetFader.setOpacity(0.0f, 500);
            break;

        case WidgetAction::HighlightOption::Moderate:
            widgetFader.setOpacity(0.1f, 200);
            break;

        case WidgetAction::HighlightOption::Strong:
            widgetFader.setOpacity(0.3f, 200);
            break;

        default:
            break;
    }
}

}