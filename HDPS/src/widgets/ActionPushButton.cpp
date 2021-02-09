#include "ActionPushButton.h"

#include <QAction>

namespace hdps {

namespace gui {

ActionPushButton::ActionPushButton(QWidget *parent) :
    QPushButton(parent)
{
}

ActionPushButton::ActionPushButton(QAction *action) :
    QPushButton()
{
    setAction(action);
}

void ActionPushButton::setAction(QAction *action) {

    // if I've got already an action associated to the button
    // remove all connections
    if (actionOwner && actionOwner != action) {
        disconnect(actionOwner, &QAction::changed,
            this, &ActionPushButton::updateButtonStatusFromAction);
        disconnect(this, &ActionPushButton::clicked,
            actionOwner, &QAction::trigger);
    }
    // store the action
    actionOwner = action;
    // configure the button
    updateButtonStatusFromAction();
    // connect the action and the button
    // so that when the action is changed the
    // button is changed too!
    connect(action, &QAction::changed,
        this, &ActionPushButton::updateButtonStatusFromAction);
    // connect the button to the slot that forwards the
    // signal to the action
    connect(this, &ActionPushButton::clicked,
        actionOwner, &QAction::trigger);
}

void ActionPushButton::updateButtonStatusFromAction() {

    if (!actionOwner)
        return;

    setText(actionOwner->text());
    setStatusTip(actionOwner->statusTip());
    setToolTip(actionOwner->toolTip());
    setIcon(actionOwner->icon());
    setEnabled(actionOwner->isEnabled());
    setCheckable(actionOwner->isCheckable());
    setChecked(actionOwner->isChecked());
}

}
}