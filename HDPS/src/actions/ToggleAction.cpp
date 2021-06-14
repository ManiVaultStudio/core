#include "ToggleAction.h"

#include <QPushButton>
#include <QMenu>

namespace hdps {

namespace gui {

ToggleAction::ToggleAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent)
{
    setCheckable(true);
    setText(title);
}

ToggleAction::Widget::Widget(QWidget* parent, ToggleAction* toggleAction) :
    WidgetAction::Widget(parent, toggleAction, Widget::State::Standard),
    _layout(new QHBoxLayout()),
    _pushButton(new QPushButton())
{
    _layout->setMargin(0);
    _layout->addWidget(_pushButton);

    setLayout(_layout);

    _pushButton->setCheckable(true);

    connect(_pushButton, &QPushButton::clicked, this, [this, toggleAction]() {
        toggleAction->trigger();
    });

    connect(_pushButton, &QPushButton::toggled, this, [this, toggleAction](bool toggled) {
        if (toggleAction->isCheckable())
            toggleAction->setChecked(toggled);
    });

    const auto update = [this, toggleAction]() -> void {
        QSignalBlocker blocker(_pushButton);

        _pushButton->setEnabled(toggleAction->isEnabled());
        _pushButton->setText(toggleAction->text());
        _pushButton->setIcon(toggleAction->icon());
        _pushButton->setChecked(toggleAction->isChecked());
        _pushButton->setToolTip(toggleAction->toolTip());
        
        setVisible(toggleAction->isVisible());
    };

    connect(toggleAction, &QAction::changed, this, [this, update]() {
        update();
    });

    update();
}

QWidget* ToggleAction::getWidget(QWidget* parent, const Widget::State& state /*= Widget::State::Standard*/)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    return new ToggleAction::Widget(parent, this);
}

}
}