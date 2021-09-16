#include "ColorMapSettingsAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QGridLayout>
#include <QGroupBox>

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapSettingsAction::ColorMapSettingsAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction),
    _rangeAction(this, "Range", 0, 1, 0, 1, 0, 1),
    _resetToDefaultRangeAction(this, "Reset to default range"),
    _invertAction(this, "Mirror horizontally"),
    _discreteAction(this, "Discrete"),
    _numberOfDiscreteStepsAction(this, "Number of steps", 2, 25, 10, 10)
{
    setText("Settings");
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("cog"));

    _rangeAction.setToolTip("Range of the color map");
    _invertAction.setToolTip("Mirror the color map horizontally");
    _resetToDefaultRangeAction.setToolTip("Reset the min/max value to the data range");

    const auto update = [this]() {
        _resetToDefaultRangeAction.setEnabled(_rangeAction.canReset());
    };

    connect(&_resetToDefaultRangeAction, &TriggerAction::triggered, this, [this, update]() {
        _rangeAction.reset();
    });

    connect(&_rangeAction, &DecimalRangeAction::rangeChanged, this, update);

    update();

    setRangeEditingEnabled(colorMapAction.hasWidgetFlag(ColorMapAction::EditRange));
}

void ColorMapSettingsAction::setRangeEditingEnabled(const bool& rangeEditingEnabled)
{
    _rangeAction.getRangeMinAction().setEnabled(rangeEditingEnabled);
    _rangeAction.getRangeMaxAction().setEnabled(rangeEditingEnabled);
    _resetToDefaultRangeAction.setEnabled(rangeEditingEnabled);
}

ColorMapSettingsAction::Widget::Widget(QWidget* parent, ColorMapSettingsAction* colorMapSettingsAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, colorMapSettingsAction, state)
{
    auto layout = new QGridLayout();

    auto& rangeAction = colorMapSettingsAction->getRangeAction();
    auto& rangeMinAction = rangeAction.getRangeMinAction();
    auto& rangeMaxAction = rangeAction.getRangeMaxAction();

    layout->addWidget(rangeMinAction.createLabelWidget(this), 0, 0);
    layout->addWidget(rangeMinAction.createWidget(this), 0, 1);

    layout->addWidget(rangeMaxAction.createLabelWidget(this), 1, 0);
    layout->addWidget(rangeMaxAction.createWidget(this), 1, 1);

    layout->addWidget(colorMapSettingsAction->getResetToDataRangeAction().createWidget(this), 2, 1);

    layout->addWidget(colorMapSettingsAction->getInvertAction().createWidget(this), 3, 1);

    auto discreteGroupBox   = new QGroupBox("Discrete");
    auto discreteLayout     = new QHBoxLayout();

    discreteGroupBox->setCheckable(true);
    discreteGroupBox->setLayout(discreteLayout);

    auto& discrete                      = colorMapSettingsAction->getDiscreteAction();
    auto& numberOfDiscreteStepsAction   = colorMapSettingsAction->getNumberOfDiscreteStepsAction();

    discreteLayout->addWidget(numberOfDiscreteStepsAction.createLabelWidget(this));
    discreteLayout->addWidget(numberOfDiscreteStepsAction.createWidget(this));

    layout->addWidget(discreteGroupBox, layout->rowCount(), 0, 1, 2);

    const auto updateGroupBox = [this, discreteGroupBox, &discrete]() {
        QSignalBlocker blocker(discreteGroupBox);

        discreteGroupBox->setTitle(discrete.text());
        discreteGroupBox->setChecked(discrete.isChecked());
    };

    connect(&discrete, &ToggleAction::toggled, this, updateGroupBox);

    connect(discreteGroupBox, &QGroupBox::toggled, this, [this, &discrete](bool toggled) {
        discrete.setChecked(toggled);
    });

    setPopupLayout(layout);

    updateGroupBox();
}

}
}
