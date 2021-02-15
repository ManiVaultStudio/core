#include "ActionPushButton.h"

#include <QDebug>

namespace hdps {

namespace gui {

ActionPushButton::ActionPushButton(QWidget* parent, QAction* action) :
    WidgetAction::Widget(parent, action),
    _layout(),
    _pushButton(action->text())
{
    _layout.setMargin(0);
    _layout.addWidget(&_pushButton);

    setLayout(&_layout);

    if (!action->icon().isNull())
        _pushButton.setIcon(action->icon());

    connect(&_pushButton, &QPushButton::clicked, this, [this, action]() {
        if (!action->isCheckable())
            action->trigger();
    });

    connect(&_pushButton, &QPushButton::toggled, this, [this, action](bool toggled) {
        if (action->isCheckable())
            action->setChecked(toggled);
    });

    const auto updatePushButton = [this, action]() -> void {
        const auto actionIsCheckable = action->isCheckable();

        _pushButton.setCheckable(actionIsCheckable);

        if (actionIsCheckable)
            _pushButton.setChecked(action->isChecked());
    };

    connect(action, &QAction::changed, this, [this, updatePushButton]() {
        updatePushButton();
    });

    updatePushButton();
}

}
}