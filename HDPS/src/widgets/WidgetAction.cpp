#include "WidgetAction.h"

#include <QMenu>
#include <QToolBar>
#include <QEvent>

namespace hdps {

namespace gui {

WidgetAction::PopupWidget::PopupWidget(QWidget* parent, const QString& title) :
    QWidget(parent),
    _mainLayout(),
    _groupBox(title)
{
    _mainLayout.setMargin(4);
    _mainLayout.addWidget(&_groupBox);

    setLayout(&_mainLayout);
}

void WidgetAction::PopupWidget::setContentLayout(QLayout* layout)
{
    _groupBox.setLayout(layout);
}

WidgetAction::Widget::Widget(QWidget* parent, QAction* action) :
    QWidget(parent),
    _action(action)
{
    setEnabled(action->isEnabled());
    action->installEventFilter(this);
}

bool WidgetAction::Widget::childOfMenu() const
{
    return dynamic_cast<QMenu*>(parent());
}

bool WidgetAction::Widget::childOfToolbar() const
{
    return dynamic_cast<QToolBar*>(parent());
}

bool WidgetAction::Widget::childOfWidget() const
{
    return childOfMenu() || childOfToolbar();
}

bool WidgetAction::Widget::eventFilter(QObject* object, QEvent* event)
{
    auto widget = dynamic_cast<QWidget*>(object);

    if (widget != nullptr) {
        switch (event->type())
        {
            case QEvent::EnabledChange:
                setEnabled(_action->isEnabled());
                break;

            default:
                break;
        }
    }

    return QObject::eventFilter(object, event);
}

WidgetAction::WidgetAction(QObject* parent) :
    QWidgetAction(parent)
{
}

}
}