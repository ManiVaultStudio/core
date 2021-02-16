#include "WidgetAction.h"

#include <QMenu>

namespace hdps {

namespace gui {

WidgetAction::Widget::Widget(QWidget* parent, QAction* action) :
    QWidget(parent),
    _action(action)
{
    const auto updateAction = [this, action]() -> void {
        setEnabled(action->isEnabled());
        //setVisible(action->isVisible());
    };

    connect(action, &QAction::changed, this, [this, updateAction]() {
        updateAction();
    });

    updateAction();
}

void WidgetAction::Widget::setLayout(QLayout* layout)
{
    layout->setMargin(0);
    layout->setSpacing(4);

    QWidget::setLayout(layout);
}

bool WidgetAction::Widget::isChildOfMenu() const
{
    return dynamic_cast<QMenu*>(parent());
}

WidgetAction::WidgetAction(QObject* parent) :
    QWidgetAction(parent)
{
}

WidgetAction::PopupWidget::PopupWidget(QWidget* parent, QAction* action) :
    Widget(parent, action)
{
    setWindowTitle(action->text());
}

WidgetAction::CompactWidget::CompactWidget(QWidget* parent, WidgetAction* widgetAction) :
    WidgetAction::Widget(parent, widgetAction),
    _layout(),
    _popupPushButton()
{
    _popupPushButton.setIcon(widgetAction->icon());
    _popupPushButton.setToolTip(widgetAction->toolTip());
    _popupPushButton.setWidget(widgetAction->createWidget(this, WidgetType::Popup));

    _layout.addWidget(&_popupPushButton);

    setLayout(&_layout);
}

WidgetAction::StateWidget::StateWidget(QWidget* parent, WidgetAction* widgetAction, const std::int32_t& priority /*= 0*/, const WidgetType& state /*= WidgetType::Compact*/) :
    Widget(parent, widgetAction),
    _state(state),
    _priority(priority),
    _layout(),
    _standardWidget(widgetAction->createWidget(this, WidgetType::Standard)),
    _compactWidget(widgetAction->createWidget(this, WidgetType::Compact))
{
    _layout.addWidget(_standardWidget);
    _layout.addWidget(_compactWidget);

    setState(state);

    setLayout(&_layout);
}

WidgetAction::WidgetType WidgetAction::StateWidget::getState() const
{
    return _state;
}

void WidgetAction::StateWidget::setState(const WidgetType& state)
{
    _standardWidget->setVisible(state == WidgetType::Standard);
    _compactWidget->setVisible(state == WidgetType::Compact);
}

std::int32_t WidgetAction::StateWidget::getPriority() const
{
    return _priority;
}

void WidgetAction::StateWidget::setPriority(const std::int32_t& priority)
{
    _priority = priority;
}

QSize WidgetAction::StateWidget::getSizeHint(const WidgetType& state) const
{
    switch (state)
    {
        case WidgetType::Standard:
            return _standardWidget->sizeHint();

        case WidgetType::Compact:
            return _compactWidget->sizeHint();

        default:
            break;
    }

    return QSize();
}

}
}