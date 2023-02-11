#include "ColorMapSettingsAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>

using namespace hdps::util;

namespace hdps::gui {

ColorMapSettingsAction::ColorMapSettingsAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction, "Settings"),
    _colorMapAction(colorMapAction),
    _horizontalAxisAction(*this, "Horizontal Axis"),
    _verticalAxisAction(*this, "Vertical Axis"),
    _discreteAction(*this),
    _settingsOneDimensionalAction(colorMapAction),
    _settingsTwoDimensionalAction(colorMapAction),
    _editorOneDimensionalAction(colorMapAction)
{
    setText("Settings");
    setIcon(Application::getIconFont("FontAwesome").getIcon("sliders-h"));
    setSerializationName("Settings");

    _horizontalAxisAction.setSerializationName("HorizontalAxis");
    _verticalAxisAction.setSerializationName("VerticalAxis");
}

void ColorMapSettingsAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicColorMapSettingsAction = dynamic_cast<ColorMapSettingsAction*>(publicAction);

    Q_ASSERT(publicColorMapSettingsAction != nullptr);

    _horizontalAxisAction.connectToPublicAction(&publicColorMapSettingsAction->getHorizontalAxisAction());
    _verticalAxisAction.connectToPublicAction(&publicColorMapSettingsAction->getVerticalAxisAction());
    _discreteAction.connectToPublicAction(&publicColorMapSettingsAction->getDiscreteAction());
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

ColorMapSettingsAction::Widget::Widget(QWidget* parent, ColorMapSettingsAction* colorMapSettingsAction) :
    WidgetActionWidget(parent, colorMapSettingsAction)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    switch (colorMapSettingsAction->getColorMapAction().getColorMapType())
    {
        case ColorMap::Type::OneDimensional:
            layout->addWidget(colorMapSettingsAction->getSettings1DAction().createWidget(this));
            break;

        case ColorMap::Type::TwoDimensional:
            layout->addWidget(colorMapSettingsAction->getSettings2DAction().createWidget(this));
            break;

        default:
            break;
    }

    setLayout(layout);
}

}
