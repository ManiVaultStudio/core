#include "TriggerAction.h"

#include <QPushButton>
#include <QMenu>

namespace hdps {

namespace gui {

TriggerAction::TriggerAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent)
{
    setText(title);
}

TriggerAction::Widget::Widget(QWidget* parent, TriggerAction* triggerAction) :
    WidgetAction::Widget(parent, triggerAction, Widget::State::Standard),
    _layout(new QHBoxLayout()),
    _pushButton(new QPushButton())
{
    _layout->setMargin(0);
    _layout->addWidget(_pushButton);

    setLayout(_layout);

    connect(_pushButton, &QPushButton::clicked, this, [this, triggerAction]() {
        triggerAction->trigger();
    });

    const auto update = [this, triggerAction]() -> void {
        QSignalBlocker blocker(_pushButton);

        _pushButton->setEnabled(triggerAction->isEnabled());
        _pushButton->setText(triggerAction->text());
        _pushButton->setIcon(triggerAction->icon());
        _pushButton->setToolTip(triggerAction->toolTip());

        setVisible(triggerAction->isVisible());
    };

    connect(triggerAction, &QAction::changed, this, [this, update]() {
        update();
    });

    update();
}

QWidget* TriggerAction::getWidget(QWidget* parent, const Widget::State& state /*= Widget::State::Standard*/)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    return new TriggerAction::Widget(parent, this);
}

}
}