#include "ColorMapSettingsAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>

using namespace hdps::util;

namespace hdps::gui {

ColorMapSettingsAction::ColorMapSettingsAction(ColorMapAction& colorMapAction, const QString& title) :
    WidgetAction(&colorMapAction, title),
    _colorMapAction(colorMapAction)
{
    setText("Settings");
    setIcon(Application::getIconFont("FontAwesome").getIcon("sliders-h"));
}

ColorMapSettingsAction::Widget::Widget(QWidget* parent, ColorMapSettingsAction* colorMapSettingsAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, colorMapSettingsAction, widgetFlags)
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

    setLayout(layout);
}

}
