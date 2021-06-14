#include "CheckAction.h"

#include <QCheckBox>
#include <QPushButton>
#include <QMenu>

namespace hdps {

namespace gui {

CheckAction::CheckAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent)
{
    setCheckable(true);
    setText(title);
}

CheckAction::Widget::Widget(QWidget* parent, CheckAction* checkAction, const bool& toggleButton /*= false*/) :
    WidgetAction::Widget(parent, checkAction, Widget::State::Standard),
    _layout(new QHBoxLayout()),
    _checkBox(nullptr),
    _pushButton(nullptr)
{
    _layout->setMargin(0);

    if (toggleButton) {
        _pushButton = new QPushButton();

        _layout->addWidget(_pushButton);

        _pushButton->setCheckable(true);

        connect(_pushButton, &QPushButton::clicked, this, [this, checkAction]() {
            checkAction->trigger();
        });

        connect(_pushButton, &QPushButton::toggled, this, [this, checkAction](bool toggled) {
            checkAction->setChecked(toggled);
        });

        const auto update = [this, checkAction]() -> void {
            QSignalBlocker blocker(_pushButton);

            _pushButton->setEnabled(checkAction->isEnabled());
            _pushButton->setText(checkAction->text());
            _pushButton->setIcon(checkAction->icon());
            _pushButton->setChecked(checkAction->isChecked());
            _pushButton->setToolTip(checkAction->toolTip());

            setVisible(checkAction->isVisible());
        };

        connect(checkAction, &QAction::changed, this, [this, update]() {
            update();
        });

        update();
    }
    else {
        _checkBox = new QCheckBox();

        _layout->addWidget(_checkBox);

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

    setLayout(_layout);
}

QWidget* CheckAction::getWidget(QWidget* parent, const Widget::State& state /*= Widget::State::Standard*/)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    return new CheckAction::Widget(parent, this);
}

}
}