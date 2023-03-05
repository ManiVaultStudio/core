#include "HorizontalToolbarAction.h"

#include <QEvent>
#include <QResizeEvent>

namespace hdps::gui {

HorizontalToolbarAction::HorizontalToolbarAction(QObject* parent, const QString& title /*= "Horizontal Toolbar"*/) :
    ToolbarAction(parent, title)
{
    setText(title);

    getGroupAction().setDefaultWidgetFlags(GroupAction::Horizontal);
}

HorizontalToolbarAction::ToolbarWidget::ToolbarWidget(QWidget* parent, HorizontalToolbarAction* horizontalToolbarAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, horizontalToolbarAction, widgetFlags),
    _horizontalToolbarAction(horizontalToolbarAction),
    _layout()
{
    parent->installEventFilter(this);

    _layout.setSizeConstraint(QLayout::SetFixedSize);

    _layout.addWidget(_horizontalToolbarAction->getGroupAction().createWidget(this));

    setLayout(&_layout);

    connect(_horizontalToolbarAction, &ToolbarAction::actionWidgetsChanged, this, &HorizontalToolbarAction::ToolbarWidget::setActionWidgets);

    setActionWidgets();
}

bool HorizontalToolbarAction::ToolbarWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            //setActionWidgets();
            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

void HorizontalToolbarAction::ToolbarWidget::setActionWidgets()
{
    QLayoutItem* layoutItem;

    while ((layoutItem = _layout.takeAt(0)) != nullptr) {
        delete layoutItem->widget();
        delete layoutItem;
    }

    for (auto actionWidget : _horizontalToolbarAction->getActionItems())
        _layout.addWidget(actionWidget->createWidget(this));
}

QString HorizontalToolbarAction::getTypeString() const
{
    return "HorizontalTolbar";
}

}
