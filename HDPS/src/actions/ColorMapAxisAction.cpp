#include "ColorMapAxisAction.h"
#include "ColorMapAction.h"
#include "ColorMapSettingsAction.h"

#include <Application.h>

#include <QGridLayout>
#include <QGroupBox>

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapAxisAction::ColorMapAxisAction(ColorMapSettingsAction& colorMapSettingsAction, const QString& title) :
    WidgetAction(&colorMapSettingsAction),
    _rangeAction(this, "Range", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1),
    _resetAction(this, "Reset"),
    _mirrorAction(this, "Mirror")
{
    setText(title);
    setSerializable(true);

    _rangeAction.setSerializable(true);
    _mirrorAction.setSerializable(true);

    _rangeAction.setToolTip("Range of the color map");
    _resetAction.setToolTip("Reset the color map");
    _mirrorAction.setToolTip("Mirror the color map");

    const auto update = [this]() {
        _resetAction.setEnabled(_rangeAction.isResettable());
    };

    connect(&_resetAction, &TriggerAction::triggered, this, [this, update]() {
        _rangeAction.reset();
    });

    connect(&_rangeAction, &DecimalRangeAction::rangeChanged, this, update);
    connect(&_rangeAction, &DecimalRangeAction::resettableChanged, this, update);

    update();

    connect(&_rangeAction, &DecimalRangeAction::resettableChanged, this, &ColorMapAxisAction::notifyResettable);
    connect(&_mirrorAction, &ToggleAction::resettableChanged, this, &ColorMapAxisAction::notifyResettable);
}

void ColorMapAxisAction::setValueFromVariant(const QVariant& value)
{
}

QVariant ColorMapAxisAction::valueToVariant() const
{
    return QVariant();
}

QVariant ColorMapAxisAction::defaultValueToVariant() const
{
    return QVariant();
}

ColorMapAxisAction::Widget::Widget(QWidget* parent, ColorMapAxisAction* colorMapAxisAction) :
    WidgetActionWidget(parent, colorMapAxisAction)
{
    auto layout = new QGridLayout();

    auto& rangeAction = colorMapAxisAction->getRangeAction();

    auto& rangeMinAction = rangeAction.getRangeMinAction();
    auto& rangeMaxAction = rangeAction.getRangeMaxAction();

    layout->addWidget(rangeMinAction.createLabelWidget(this), 0, 0);
    layout->addWidget(rangeMinAction.createWidget(this), 0, 1);

    layout->addWidget(rangeMaxAction.createLabelWidget(this), 1, 0);
    layout->addWidget(rangeMaxAction.createWidget(this), 1, 1);

    auto miscellaneousLayout = new QHBoxLayout();

    miscellaneousLayout->addWidget(colorMapAxisAction->getMirrorAction().createWidget(this));
    miscellaneousLayout->addWidget(colorMapAxisAction->getResetAction().createWidget(this));
    
    layout->addLayout(miscellaneousLayout, layout->rowCount(), 0, 1, 2);

    setPopupLayout(layout);
}

}
}
