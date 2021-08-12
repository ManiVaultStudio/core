#include "ToggleAction.h"

#include <QCheckBox>
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

ToggleAction::ToggleAction(QObject* parent, const QString& title /*= ""*/, const bool& toggled /*= false*/, const bool& defaultToggled /*= false*/) :
    WidgetAction(parent),
    _defaultToggled(defaultToggled)
{
    setCheckable(true);
    setText(title);
}

void ToggleAction::initialize(const bool& toggled /*= false*/, const bool& defaultToggled /*= false*/)
{
    setChecked(toggled);
}

bool ToggleAction::getToggled() const
{
    return isChecked();
}

void ToggleAction::setToggled(const bool& toggled)
{
    setChecked(toggled);
}

bool ToggleAction::getDefaultToggled() const
{
    return _defaultToggled;
}

void ToggleAction::setDefaultToggled(const bool& defaultToggled)
{
    if (defaultToggled == _defaultToggled)
        return;

    _defaultToggled = defaultToggled;

    emit defaultToggledChanged(_defaultToggled);
}

bool ToggleAction::canReset() const
{
    return isChecked() == _defaultToggled;
}

void ToggleAction::reset()
{
    setChecked(_defaultToggled);
}

ToggleAction::CheckBoxWidget::CheckBoxWidget(QWidget* parent, ToggleAction* checkAction) :
    WidgetActionWidget(parent, checkAction, WidgetActionWidget::State::Standard),
    _checkBox(nullptr)
{
    setAcceptDrops(true);

    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    _checkBox = new QCheckBox();

    layout->addWidget(_checkBox);

    connect(_checkBox, &QCheckBox::toggled, this, [this, checkAction](bool toggled) {
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

    setLayout(layout);
}

ToggleAction::PushButtonWidget::PushButtonWidget(QWidget* parent, ToggleAction* checkAction) :
    WidgetActionWidget(parent, checkAction, WidgetActionWidget::State::Standard),
    _pushButton(nullptr)
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    _pushButton = new QPushButton();

    layout->addWidget(_pushButton);

    _pushButton->setCheckable(true);
    _pushButton->setText(checkAction->text());
    _pushButton->setIcon(checkAction->icon());

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

    setLayout(layout);
}

QWidget* ToggleAction::getWidget(QWidget* parent, const WidgetActionWidget::State& state /*= WidgetActionWidget::State::Standard*/)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    return new ToggleAction::PushButtonWidget(parent, this);
}

}
}
