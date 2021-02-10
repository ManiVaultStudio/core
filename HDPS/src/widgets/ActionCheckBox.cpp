#include "ActionCheckBox.h"

#include <QAction>

namespace hdps {

namespace gui {

ActionCheckBox::ActionCheckBox(QWidget *parent) :
    QCheckBox(parent)
{
}

ActionCheckBox::ActionCheckBox(QAction *action) :
    QCheckBox()
{
    setAction(action);
}

void ActionCheckBox::setAction(QAction* action) {

    if (_actionOwner && _actionOwner != action) {
        disconnect(_actionOwner, &QAction::changed, this, &ActionCheckBox::updateFromAction);
        disconnect(this, &ActionCheckBox::clicked, _actionOwner, &QAction::trigger);
    }

    _actionOwner = action;

    
    updateFromAction();
    
    connect(action, &QAction::changed, this, &ActionCheckBox::updateFromAction);
    connect(this, &ActionCheckBox::clicked, _actionOwner, &QAction::trigger);
}

void ActionCheckBox::updateFromAction() {

    if (!_actionOwner)
        return;

    setText(_actionOwner->text());
    setStatusTip(_actionOwner->statusTip());
    setToolTip(_actionOwner->toolTip());
    setEnabled(_actionOwner->isEnabled());
    setChecked(_actionOwner->isChecked());
}

}
}