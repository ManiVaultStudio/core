#include "ColorMapSettingsAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapSettingsAction::ColorMapSettingsAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
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

    connect(&_editorOneDimensionalAction, &ColorMapEditorOneDimensionalAction::toggled, this, &ColorMapSettingsAction::updateActionsReadOnly);

    updateActionsReadOnly();
}

void ColorMapSettingsAction::updateActionsReadOnly()
{
    _discreteAction.setEnabled(!_editorOneDimensionalAction.isChecked());
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

ColorMapSettingsAction::Widget::Widget(QWidget* parent, ColorMapSettingsAction* colorMapSettingsAction) :
    WidgetActionWidget(parent, colorMapSettingsAction)
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    switch (colorMapSettingsAction->getColorMapAction().getColorMapType())
    {
        case ColorMap::Type::OneDimensional:
            layout->addWidget(colorMapSettingsAction->getSettingsOneDimensionalAction().createWidget(this));
            break;

        case ColorMap::Type::TwoDimensional:
            layout->addWidget(colorMapSettingsAction->getSettingsTwoDimensionalAction().createWidget(this));
            break;

        default:
            break;
    }

    layout->addWidget(colorMapSettingsAction->getEditorOneDimensionalAction().createWidget(this));

    setLayout(layout);
}

}
}
