#include "CheckAction.h"

#include <QCheckBox>
#include <QMenu>

namespace hdps {

namespace gui {

CheckAction::CheckAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent)
{
    setCheckable(true);
    setText(title);
}

CheckAction::Widget::Widget(QWidget* parent, CheckAction* checkAction) :
    WidgetAction::Widget(parent, checkAction, Widget::State::Standard),
    _layout(new QHBoxLayout()),
    _checkBox(new QCheckBox())
{
    _layout->setMargin(0);
    _layout->addWidget(_checkBox);

    setLayout(_layout);

    connect(_checkBox, &QCheckBox::toggled, this, [this, checkAction](bool toggled) {
        if (checkAction->isCheckable())
            checkAction->setChecked(toggled);
    });

    const auto update = [this, checkAction]() -> void {
        QSignalBlocker blocker(_checkBox);

        _checkBox->setEnabled(checkAction->isEnabled());
        _checkBox->setText(checkAction->text());
        _checkBox->setToolTip(checkAction->toolTip());
        _checkBox->setChecked(checkAction->isChecked());
        
        setVisible(checkAction->isVisible());
    };

    connect(checkAction, &CheckAction::changed, this, [this, update]() {
        update();
    });

    update();
}

QWidget* CheckAction::getWidget(QWidget* parent, const Widget::State& state /*= Widget::State::Standard*/)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    return new CheckAction::Widget(parent, this);
}

}
}