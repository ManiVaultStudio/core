#include "ActionWidget.h"

#include <QAction>
#include <QEvent>
#include <QMenu>
#include <QToolBar>

ActionWidget::ActionWidget(QWidget* parent, QAction* action) :
    QWidget(parent),
    _action(action)
{
    setEnabled(action->isEnabled());
    action->installEventFilter(this);
}

bool ActionWidget::childOfMenu() const
{
    return dynamic_cast<QMenu*>(parent());
}

bool ActionWidget::childOfToolbar() const
{
    return dynamic_cast<QToolBar*>(parent());
}

bool ActionWidget::childOfWidget() const
{
    return childOfMenu() || childOfToolbar();
}

bool ActionWidget::eventFilter(QObject* object, QEvent* event)
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
