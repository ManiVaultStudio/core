#include "ColorMapSettingsAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>

using namespace hdps::util;

namespace hdps::gui {

ColorMapSettingsAction::ColorMapSettingsAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction),
{
    setText("Settings");
    setIcon(Application::getIconFont("FontAwesome").getIcon("sliders-h"));
    setSerializationName("Settings");

    _horizontalAxisAction.setSerializationName("HorizontalAxis");
    _verticalAxisAction.setSerializationName("VerticalAxis");

    _synchronizeWithLocalDataRange.setSerializationName("UseLocalDataRange");
    _synchronizeWithSharedDataRange.setSerializationName("UseGlobalDataRange");
    _localDataRangeOneDimensionalAction.setSerializationName("LocalDataRangeOne1D");
    _sharedDataRangeOneDimensionalAction.setSerializationName("GlobalDataRangeOne1D");

    _localDataRangeOneDimensionalAction.setEnabled(false);
    _sharedDataRangeOneDimensionalAction.setEnabled(false);

    const auto updateUseGlobalDataRangeVisibility = [this]() -> void {
        _sharedDataRangeOneDimensionalAction.setVisible(_colorMapAction.isConnected());
        _synchronizeWithSharedDataRange.setVisible(_colorMapAction.isConnected());
    };

    updateUseGlobalDataRangeVisibility();

    connect(&_colorMapAction, &ColorMapAction::isConnectedChanged, this, updateUseGlobalDataRangeVisibility);

    const auto updateSharedDataRange = [this]() -> void {

        NumericalRange<float> sharedDataRangeOneDimensional;

        for (auto connectedAction : _colorMapAction.getConnectedActions()) {
            auto privateColorMapAction = dynamic_cast<ColorMapAction*>(connectedAction);

            sharedDataRangeOneDimensional += privateColorMapAction->getSettingsAction().getLocalDataRangeOneDimensionalAction().getRange();
        }

        for (auto connectedAction : _colorMapAction.getConnectedActions()) {
            auto privateColorMapAction = dynamic_cast<ColorMapAction*>(connectedAction);

            privateColorMapAction->getSettingsAction().getSharedDataRangeOneDimensionalAction().setRange(sharedDataRangeOneDimensional);
        }
    };

    updateSharedDataRange();

    connect(&_colorMapAction, &ColorMapAction::actionConnected, this, updateSharedDataRange);
    connect(&_colorMapAction, &ColorMapAction::actionDisconnected, this, updateSharedDataRange);
    connect(&_synchronizeWithSharedDataRange, &ToggleAction::toggled, this, &ColorMapSettingsAction::synchronizeWithSharedDataRange);
}

void ColorMapSettingsAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicColorMapSettingsAction = dynamic_cast<ColorMapSettingsAction*>(publicAction);

    Q_ASSERT(publicColorMapSettingsAction != nullptr);

    _horizontalAxisAction.connectToPublicAction(&publicColorMapSettingsAction->getHorizontalAxisAction());
    _verticalAxisAction.connectToPublicAction(&publicColorMapSettingsAction->getVerticalAxisAction());
    _discreteAction.connectToPublicAction(&publicColorMapSettingsAction->getDiscreteAction());

    connect(&publicColorMapSettingsAction->getSharedDataRangeOneDimensionalAction(), &DecimalRangeAction::rangeChanged, this, &ColorMapSettingsAction::synchronizeWithSharedDataRange);


    //connect(&publicColorMapAction->getGlobalDataRange1dAction(), &DecimalRangeAction::rangeChanged, this, [this]() -> void {
    //});
}

void ColorMapSettingsAction::disconnectFromPublicAction()
{
    _horizontalAxisAction.disconnectFromPublicAction();
    _verticalAxisAction.disconnectFromPublicAction();
    _discreteAction.disconnectFromPublicAction();
}

void ColorMapSettingsAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _horizontalAxisAction.fromParentVariantMap(variantMap);
    _verticalAxisAction.fromParentVariantMap(variantMap);
    _discreteAction.fromParentVariantMap(variantMap);
    _editorOneDimensionalAction.fromParentVariantMap(variantMap);
}

QVariantMap ColorMapSettingsAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _horizontalAxisAction.insertIntoVariantMap(variantMap);
    _verticalAxisAction.insertIntoVariantMap(variantMap);
    _discreteAction.insertIntoVariantMap(variantMap);
    _editorOneDimensionalAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

void ColorMapSettingsAction::synchronizeWithSharedDataRange()
{
    if (!_colorMapAction.isConnected())
        return;

    if (!_synchronizeWithSharedDataRange.isChecked())
        return;

    const auto sharedDataRange = dynamic_cast<ColorMapAction*>(_colorMapAction.getPublicAction())->getSettingsAction().getSharedDataRangeOneDimensionalAction().getRange();

    getHorizontalAxisAction().getRangeAction().initialize(sharedDataRange, sharedDataRange);
}

ColorMapSettingsAction::Widget::Widget(QWidget* parent, ColorMapSettingsAction* colorMapSettingsAction) :
    WidgetActionWidget(parent, colorMapSettingsAction)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    auto& colorMapAction = colorMapSettingsAction->getColorMapAction();

    switch (colorMapSettingsAction->getColorMapAction().getColorMapType())
    {
        case ColorMap::Type::OneDimensional:
            layout->addWidget(colorMapAction.getSettings1DAction().createWidget(this));
            break;

        case ColorMap::Type::TwoDimensional:
            layout->addWidget(colorMapAction.getSettings2DAction().createWidget(this));
            break;

        default:
            break;
    }

    setPopupLayout(layout);
}

}
