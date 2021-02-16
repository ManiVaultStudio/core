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
        setVisible(action->isVisible());
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

}
}