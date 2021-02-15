#include "ActionCheckBox.h"

#include <QDebug>

namespace hdps {

namespace gui {

ActionCheckBox::ActionCheckBox(QWidget* parent, QAction* action) :
    WidgetAction::Widget(parent, action),
    _layout(),
    _checkBox(action->text())
{
    _layout.setMargin(0);
    _layout.addWidget(&_checkBox);

    setLayout(&_layout);

    connect(&_checkBox, &QCheckBox::toggled, this, [this, action](bool toggled) {
        if (action->isCheckable())
            action->setChecked(toggled);
    });

    const auto updateCheckBox = [this, action]() -> void {
        _checkBox.setChecked(action->isChecked());
    };

    connect(action, &QAction::changed, this, [this, updateCheckBox]() {
        updateCheckBox();
    });

    updateCheckBox();
}

}
}