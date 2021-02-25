#include "StandardAction.h"

namespace hdps {

namespace gui {

StandardAction::StandardAction(QObject* parent, const QString& title /*= ""*/) :
    QWidgetAction(parent)
{
    setText(title);
}

StandardAction::PushButton::PushButton(QWidget* parent, QAction* action) :
    QPushButton(parent)
{
    connect(this, &QPushButton::clicked, this, [this, action]() {
        if (!action->isCheckable())
            action->trigger();
    });

    connect(this, &QPushButton::toggled, this, [this, action](bool toggled) {
        if (action->isCheckable())
            action->setChecked(toggled);
    });

    const auto updateUI = [this, action]() -> void {
        setEnabled(action->isEnabled());
        setText(action->text());
        setIcon(action->icon().isNull() ? QIcon() : action->icon());
        setCheckable(action->isCheckable());
        setChecked(action->isChecked());
        setToolTip(action->toolTip());
    };

    connect(action, &QAction::changed, this, [this, updateUI]() {
        updateUI();
    });

    updateUI();
}

StandardAction::CheckBox::CheckBox(QWidget* parent, QAction* action) :
    QCheckBox(parent)
{
    connect(this, &QCheckBox::toggled, this, [this, action](bool toggled) {
        if (action->isCheckable())
            action->setChecked(toggled);
    });

    const auto updateUI = [this, action]() -> void {
        setEnabled(action->isEnabled());
        setText(action->text());
        setToolTip(action->toolTip());
        setChecked(action->isChecked());
    };

    connect(action, &QAction::changed, this, [this, updateUI]() {
        updateUI();
    });

    updateUI();
}

}
}