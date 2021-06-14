#include "ToggleAction.h"

#include <QCheckBox>
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

ToggleAction::Widget::Widget(QWidget* parent, ToggleAction* checkAction, const Mode& mode /*= Mode::CheckBox*/) :
    WidgetAction::Widget(parent, checkAction, Widget::State::Standard),
    _layout(new QHBoxLayout()),
    _checkBox(nullptr),
    _pushButton(nullptr)
{
    _layout->setMargin(0);

    switch (mode) {
        case Mode::CheckBox:
        {
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

            connect(checkAction, &ToggleAction::changed, this, [this, update]() {
                update();
            });

            update();

            break;
        }

        case Mode::Button:
        {
            _pushButton = new QPushButton();

            _layout->addWidget(_pushButton);

            _pushButton->setCheckable(true);
            _pushButton->setText(checkAction->text());
            _pushButton->setIcon(checkAction->icon());

            connect(_pushButton, &QPushButton::clicked, this, [this, checkAction]() {
                checkAction->trigger();
            });

            connect(_pushButton, &QPushButton::toggled, this, [this, checkAction](bool toggled) {
                checkAction->setChecked(toggled);
            });

            const auto update = [this, checkAction]() -> void {
                QSignalBlocker blocker(_pushButton);

                _pushButton->setEnabled(checkAction->isEnabled());
                //_pushButton->setText(checkAction->text());
                //_pushButton->setIcon(checkAction->icon());
                _pushButton->setChecked(checkAction->isChecked());
                _pushButton->setToolTip(checkAction->toolTip());

                setVisible(checkAction->isVisible());
            };

            connect(checkAction, &QAction::changed, this, [this, update]() {
                update();
            });

            update();

            break;
        }

    default:
        break;

    }

    setLayout(_layout);
}

QWidget* ToggleAction::getWidget(QWidget* parent, const Widget::State& state /*= Widget::State::Standard*/)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    return new ToggleAction::Widget(parent, this);
}

}
}