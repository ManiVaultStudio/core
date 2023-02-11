#include "ToggleAction.h"

#include <QCheckBox>
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>
#include <QLabel>
#include <QEvent>

using namespace hdps::util;

namespace hdps::gui {

ToggleAction::ToggleAction(QObject* parent, const QString& title /*= ""*/, const bool& toggled /*= false*/, const bool& defaultToggled /*= false*/) :
    WidgetAction(parent, title),
    _defaultToggled(defaultToggled),
    _indeterminate(false)
{
    setCheckable(true);
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    initialize(toggled, defaultToggled);
}

void ToggleAction::initialize(const bool& toggled /*= false*/, const bool& defaultToggled /*= false*/)
{
    setChecked(toggled);
    setDefaultToggled(defaultToggled);
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

void ToggleAction::setChecked(bool checked)
{
    QAction::setChecked(checked);

    _indeterminate = false;

    saveToSettings();
}

bool ToggleAction::getIndeterminate() const
{
    return _indeterminate;
}

void ToggleAction::setIndeterminate(bool indeterminate)
{
    if (indeterminate == _indeterminate)
        return;

    _indeterminate = indeterminate;

    emit indeterminateChanged(_indeterminate);
}

void ToggleAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    setChecked(variantMap["Value"].toBool());
}

QVariantMap ToggleAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", QVariant::fromValue(isChecked()) }
    });

    return variantMap;
}

void ToggleAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicToggleAction = dynamic_cast<ToggleAction*>(publicAction);

    Q_ASSERT(publicToggleAction != nullptr);

    connect(this, &ToggleAction::toggled, publicToggleAction, &ToggleAction::setChecked);
    connect(publicToggleAction, &ToggleAction::toggled, this, &ToggleAction::setChecked);

    setChecked(publicToggleAction->isChecked());

    WidgetAction::connectToPublicAction(publicAction);
}

void ToggleAction::disconnectFromPublicAction()
{
    auto publicToggleAction = dynamic_cast<ToggleAction*>(getPublicAction());

    Q_ASSERT(publicToggleAction != nullptr);

    disconnect(this, &ToggleAction::toggled, publicToggleAction, &ToggleAction::setChecked);
    disconnect(publicToggleAction, &ToggleAction::toggled, this, &ToggleAction::setChecked);

    WidgetAction::disconnectFromPublicAction();
}

WidgetAction* ToggleAction::getPublicCopy() const
{
    return new ToggleAction(parent(), text(), isChecked(), _defaultToggled);
}

ToggleAction::CheckBoxWidget::CheckBoxWidget(QWidget* parent, ToggleAction* toggleAction) :
    QCheckBox(parent),
    _toggleAction(toggleAction)
{
    setAcceptDrops(true);
    setText("");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto layout = new QHBoxLayout();

    layout->setContentsMargins(18, 0, 0, 0);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    auto labelWidget = toggleAction->createLabelWidget(this, 0);
    
    labelWidget->installEventFilter(this);

    layout->addWidget(labelWidget);
    layout->addStretch(1);

    setLayout(layout);

    connect(this, &QCheckBox::toggled, this, [this](bool toggled) {
        _toggleAction->setChecked(toggled);
    });

    const auto update = [this]() -> void {
        QSignalBlocker blocker(this);

        setEnabled(_toggleAction->isEnabled());
        setToolTip(_toggleAction->toolTip());
        setVisible(_toggleAction->isVisible());
    };

    connect(toggleAction, &ToggleAction::changed, this, [this, update]() {
        update();
    });

    const auto updateToggle = [this]() -> void {
        QSignalBlocker blocker(this);

        if (_toggleAction->getIndeterminate())
            setCheckState(Qt::CheckState::PartiallyChecked);
        else
            setChecked(_toggleAction->isChecked());
    };

    connect(_toggleAction, &ToggleAction::toggled, this, [this, updateToggle]() {
        updateToggle();
    });

    connect(_toggleAction, &ToggleAction::indeterminateChanged, this, [this, updateToggle]() {
        updateToggle();
    });

    update();
    updateToggle();
}

bool ToggleAction::CheckBoxWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::MouseButtonPress:
            _toggleAction->toggle();
            break;

        default:
            break;
    }

    return QCheckBox::eventFilter(target, event);
}

ToggleAction::PushButtonWidget::PushButtonWidget(QWidget* parent, ToggleAction* toggleAction, const std::int32_t& widgetFlags) :
    QPushButton(parent),
    _toggleAction(toggleAction)
{
    setCheckable(true);

    connect(this, &QPushButton::toggled, this, [this](bool toggled) {
        _toggleAction->setChecked(toggled);
    });

    const auto update = [this, widgetFlags]() -> void {
        QSignalBlocker blocker(this);

        if (isEnabled() != _toggleAction->isEnabled())
            setEnabled(_toggleAction->isEnabled());

        if (isChecked() != _toggleAction->isChecked())
            setChecked(_toggleAction->isChecked());

        if (toolTip() != _toggleAction->toolTip())
            setToolTip(_toggleAction->toolTip());

        if (isVisible() != _toggleAction->isVisible())
            setVisible(_toggleAction->isVisible());

        if (widgetFlags & WidgetFlag::Text && text() != _toggleAction->text())
            setText(_toggleAction->text());

        if (widgetFlags & WidgetFlag::Icon) {
            setIcon(_toggleAction->icon());
            setProperty("class", "square-button");
        }
    };

    connect(_toggleAction, &QAction::changed, this, update);

    update();
}

QWidget* ToggleAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);

    if (widgetFlags & WidgetFlag::CheckBox)
        layout->addWidget(new ToggleAction::CheckBoxWidget(parent, this));

    if (widgetFlags & WidgetFlag::PushButton)
        layout->addWidget(new ToggleAction::PushButtonWidget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

}
