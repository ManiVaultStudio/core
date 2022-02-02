#include "ColorMapDiscreteAction.h"
#include "ColorMapAction.h"
#include "ColorMapSettingsAction.h"

#include <QVBoxLayout>
#include <QGroupBox>

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapDiscreteAction::ColorMapDiscreteAction(ColorMapSettingsAction& colorMapSettingsAction) :
    WidgetAction(&colorMapSettingsAction),
    _numberOfStepsAction(this, "Number of steps", 2, 10, 5, 5)
{
    setText("Discrete");
    setCheckable(true);

    _numberOfStepsAction.setSerializable(true);
    _numberOfStepsAction.setToolTip("Number of discrete steps");

    const auto updateResettable = [this]() {
        setResettable(isResettable());
    };

    connect(this, &ColorMapDiscreteAction::toggled, this, updateResettable);
    connect(&_numberOfStepsAction, &IntegralAction::resettableChanged, this, updateResettable);
}

void ColorMapDiscreteAction::setValueFromVariant(const QVariant& value)
{
    if (!value.isValid())
        return;

    setChecked(value.toBool());
}

QVariant ColorMapDiscreteAction::valueToVariant() const
{
    return QVariant(isChecked());
}

QVariant ColorMapDiscreteAction::defaultValueToVariant() const
{
    return QVariant(false);
}

ColorMapDiscreteAction::Widget::Widget(QWidget* parent, ColorMapDiscreteAction* colorMapDiscreteAction) :
    WidgetActionWidget(parent, colorMapDiscreteAction)
{
    auto layout         = new QVBoxLayout();
    auto groupBox       = new QGroupBox();
    auto groupBoxLayout = new QHBoxLayout();

    layout->setMargin(4);
    layout->addWidget(groupBox);

    groupBox->setCheckable(true);
    groupBox->setLayout(groupBoxLayout);

    auto& numberOfDiscreteStepsAction = colorMapDiscreteAction->getNumberOfStepsAction();

    groupBoxLayout->addWidget(numberOfDiscreteStepsAction.createLabelWidget(this));
    groupBoxLayout->addWidget(numberOfDiscreteStepsAction.createWidget(this));

    const auto updateGroupBox = [this, groupBox, colorMapDiscreteAction]() {
        QSignalBlocker blocker(groupBox);

        groupBox->setTitle(colorMapDiscreteAction->text());
        groupBox->setChecked(colorMapDiscreteAction->isChecked());
    };

    connect(colorMapDiscreteAction, &ToggleAction::toggled, this, updateGroupBox);

    connect(groupBox, &QGroupBox::toggled, this, [this, colorMapDiscreteAction](bool toggled) {
        colorMapDiscreteAction->setChecked(toggled);
    });

    setLayout(layout);

    updateGroupBox();
}

}
}
