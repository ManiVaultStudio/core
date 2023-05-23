#include "WidgetActionViewWidget.h"
#include "WidgetActionHighlightWidget.h"
#include "WidgetActionMimeData.h"
#include "WidgetAction.h"

#include <QDebug>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>

namespace hdps::gui {

WidgetActionViewWidget::WidgetActionViewWidget(QWidget* parent, WidgetAction* action) :
    QWidget(parent),
    _action(nullptr),
    _highlightWidget(new WidgetActionHighlightWidget(this, action)),
    _cachedHighlighting(0)
{
    setAcceptDrops(true);
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

void WidgetActionViewWidget::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
    auto actionMimeData = dynamic_cast<const WidgetActionMimeData*>(dragEnterEvent->mimeData());

    if (actionMimeData == nullptr)
        return;

    if (actionMimeData->getAction() == getAction())
        return;

    if (actionMimeData->getAction()->getTypeString() != getAction()->getTypeString())
        return;

    //if (actionMimeData->getAction()->getPublicAction() == getAction()->getPublicAction())
    //    return;

    _cachedHighlighting = static_cast<std::int32_t>(getAction()->getHighlighting());

    getAction()->setHighlighting(WidgetAction::HighlightOption::Strong);
}

void WidgetActionViewWidget::dragLeaveEvent(QDragLeaveEvent* dragLeaveEvent)
{
    getAction()->setHighlighting(static_cast<WidgetAction::HighlightOption>(_cachedHighlighting));
}

}
