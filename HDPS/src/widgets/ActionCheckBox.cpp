#include "StandardAction.h"

#include <QDebug>

namespace hdps {

namespace gui {

StandardAction::StandardAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent)
{
    setText(title);
}

QWidget* StandardAction::createWidget(QWidget* parent, const WidgetType& widgetType /*= WidgetType::PushButton*/)
{
    switch (widgetType)
    {
        case WidgetType::PushButton:
            return new PushButton(parent, this);

        case WidgetType::CheckBox:
            return new CheckBox(parent, this);

        default:
            break;
    }

    return nullptr;
}

StandardAction::PushButton::PushButton(QWidget* parent, StandardAction* standardAction) :
    WidgetAction::Widget(parent, standardAction),
    _layout(),
    _pushButton(standardAction->text())
{
    _layout.setMargin(0);
    _layout.addWidget(&_pushButton);

    setLayout(&_layout);

    if (!standardAction->icon().isNull())
        _pushButton.setIcon(standardAction->icon());

    connect(&_pushButton, &QPushButton::clicked, this, [this, standardAction]() {
        if (!standardAction->isCheckable())
            standardAction->trigger();
    });

    connect(&_pushButton, &QPushButton::toggled, this, [this, standardAction](bool toggled) {
        if (standardAction->isCheckable())
            standardAction->setChecked(toggled);
    });

    const auto updatePushButton = [this, standardAction]() -> void {
        const auto actionIsCheckable = standardAction->isCheckable();

        _pushButton.setCheckable(actionIsCheckable);

        if (actionIsCheckable)
            _pushButton.setChecked(standardAction->isChecked());
    };

    connect(standardAction, &StandardAction::changed, this, [this, updatePushButton]() {
        updatePushButton();
    });

    updatePushButton();
}

StandardAction::CheckBox::CheckBox(QWidget* parent, StandardAction* standardAction) :
    WidgetAction::Widget(parent, standardAction),
    _layout(),
    _checkBox(standardAction->text())
{
    _layout.setMargin(0);
    _layout.addWidget(&_checkBox);

    setLayout(&_layout);

    connect(&_checkBox, &QPushButton::toggled, this, [this, standardAction](bool toggled) {
        if (standardAction->isCheckable())
            standardAction->setChecked(toggled);
    });

    const auto updatePushButton = [this, standardAction]() -> void {
        _checkBox.setChecked(standardAction->isChecked());
    };

    connect(standardAction, &StandardAction::changed, this, [this, updatePushButton]() {
        updatePushButton();
    });

    updatePushButton();
}

}
}