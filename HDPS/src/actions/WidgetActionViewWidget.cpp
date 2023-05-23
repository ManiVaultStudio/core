#include "WidgetActionViewWidget.h"
#include "WidgetActionHighlightWidget.h"
#include "WidgetAction.h"

#include <QDebug>

namespace hdps::gui {

WidgetActionViewWidget::WidgetActionViewWidget(QWidget* parent, WidgetAction* action) :
    QWidget(parent),
    _action(nullptr),
    _highlightWidget(new WidgetActionHighlightWidget(this, action))
{
}

void WidgetActionViewWidget::setAction(WidgetAction* action)
{
    if (_action != nullptr)
        disconnect(_action, &WidgetAction::changed, this, nullptr);

    _action = action;

    _highlightWidget->setAction(action);

    if (_action == nullptr)
        return;

    const auto update = [this]() -> void {
        setEnabled(_action->isEnabled());
        setToolTip(_action->toolTip());
        setVisible(_action->isVisible());
    };

    connect(_action, &WidgetAction::changed, this, update);

    update();
}

WidgetAction* WidgetActionViewWidget::getAction()
{
    return _action;
}

}
