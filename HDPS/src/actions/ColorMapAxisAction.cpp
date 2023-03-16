#include "ColorMapAxisAction.h"
#include "ColorMapAction.h"
#include "ColorMapSettingsAction.h"

#include <Application.h>

#include <QGridLayout>
#include <QGroupBox>

using namespace hdps::util;

namespace hdps::gui {

ColorMapAxisAction::ColorMapAxisAction(ColorMapSettingsAction& colorMapSettingsAction, const QString& title) :
    WidgetAction(&colorMapSettingsAction),
    _colorMapSettingsAction(colorMapSettingsAction),
    _rangeAction(this, "Range", NumericalRange<float>(0.0f, 1.0f), NumericalRange<float>(0.0f, 1.0f), 1),
    _mirrorAction(this, "Mirror")
{
    setText(title);
    setSerializationName("Axis");
    setDefaultWidgetFlags(DecimalRangeAction::Slider);

    _rangeAction.setSerializationName("Range");
    _mirrorAction.setSerializationName("Mirror");

    _rangeAction.setToolTip("Range of the color map");
    _mirrorAction.setToolTip("Mirror the color map");
}

void ColorMapAxisAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicColorMapAxisAction = dynamic_cast<ColorMapAxisAction*>(publicAction);

    Q_ASSERT(publicColorMapAxisAction != nullptr);

    _mirrorAction.connectToPublicAction(&publicColorMapAxisAction->getMirrorAction());

    WidgetAction::connectToPublicAction(publicAction);
}

void ColorMapAxisAction::disconnectFromPublicAction()
{
    _mirrorAction.disconnectFromPublicAction();

    WidgetAction::disconnectFromPublicAction();
}

void ColorMapAxisAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _rangeAction.fromParentVariantMap(variantMap);
    _mirrorAction.fromParentVariantMap(variantMap);
}

QVariantMap ColorMapAxisAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _rangeAction.insertIntoVariantMap(variantMap);
    _mirrorAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

ColorMapAxisAction::Widget::Widget(QWidget* parent, ColorMapAxisAction* colorMapAxisAction) :
    WidgetActionWidget(parent, colorMapAxisAction)
{
    auto layout = new QGridLayout();

    auto rowIndex = 0;
        
    layout->addWidget(colorMapAxisAction->getRangeAction().createLabelWidget(this), rowIndex, 0);
    layout->addWidget(colorMapAxisAction->getRangeAction().createWidget(this), rowIndex, 1);

    rowIndex++;

    auto& colorMapSettings = colorMapAxisAction->getColorMapSettingsAction();

    layout->addWidget(colorMapSettings.getLocalDataRangeOneDimensionalAction().createLabelWidget(this), rowIndex, 0);
    layout->addWidget(colorMapSettings.getLocalDataRangeOneDimensionalAction().createWidget(this, DecimalRangeAction::MinimumLineEdit | DecimalRangeAction::MaximumLineEdit), rowIndex, 1);

    rowIndex++;

    if (colorMapAxisAction->isConnected()) {
        layout->addWidget(colorMapSettings.getGlobalDataRangeOneDimensionalAction().createLabelWidget(this), rowIndex, 0);
        layout->addWidget(colorMapSettings.getGlobalDataRangeOneDimensionalAction().createWidget(this, DecimalRangeAction::MinimumLineEdit | DecimalRangeAction::MaximumLineEdit), rowIndex, 1);

        rowIndex++;
    }

    layout->addWidget(colorMapSettings.getUseDataRangeAction().createLabelWidget(this), rowIndex, 0);
    layout->addWidget(colorMapSettings.getUseDataRangeAction().createWidget(this), rowIndex, 1);

    rowIndex++;

    layout->addWidget(colorMapAxisAction->getMirrorAction().createWidget(this), rowIndex, 1);

    layout->setColumnStretch(1, 1);

    setPopupLayout(layout);
}

}
