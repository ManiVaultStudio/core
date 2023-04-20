#include "WidgetActionViewWidget.h"
#include "WidgetAction.h"

#include <QDebug>

namespace hdps::gui {

WidgetActionViewWidget::WidgetActionViewWidget(QWidget* parent, WidgetAction* action) :
    QWidget(parent),
    _action(nullptr),
    _highlightWidget(new HighlightWidget(this))
{
}

void WidgetActionViewWidget::setAction(WidgetAction* action)
{
    if (_action != nullptr) {
        disconnect(_action, &WidgetAction::changed, this, nullptr);
        disconnect(_action, &WidgetAction::highlightedChanged, this, nullptr);
    }

    _action = action;

    if (_action == nullptr)
        return;

    const auto update = [this]() -> void {
        setEnabled(_action->isEnabled());
        setToolTip(_action->toolTip());
        setVisible(_action->isVisible());
    };

    connect(_action, &WidgetAction::changed, this, update);

    update();
    
    const auto updateHighlighted = [this]() -> void {
        if (_action->isHighlighted()) {
            _highlightWidget->show();
            _highlightWidget->raise();
        } else
            _highlightWidget->hide();
    };

    connect(_action, &WidgetAction::highlightedChanged, this, updateHighlighted);

    updateHighlighted();
}

WidgetAction* WidgetActionViewWidget::getAction()
{
    return _action;
}

WidgetActionViewWidget::HighlightWidget::HighlightWidget(QWidget* parent) :
    OverlayWidget(parent)
{
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setStyleSheet("background-color: gray;");
}

}
