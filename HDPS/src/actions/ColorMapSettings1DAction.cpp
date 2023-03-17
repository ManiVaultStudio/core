#include "ColorMapSettings1DAction.h"
#include "ColorMapAction.h"
#include "GroupAction.h"
#include "Application.h"

#include <QVBoxLayout>

using namespace hdps::util;

namespace hdps::gui {

ColorMapSettings1DAction::ColorMapSettings1DAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction)
{
    setText("Settings 1D");
    setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
}

ColorMapSettings1DAction::Widget::Widget(QWidget* parent, ColorMapSettings1DAction* colorMapSettings1DAction) :
    WidgetActionWidget(parent, colorMapSettings1DAction)
{
    setAutoFillBackground(true);

    auto& colorMapAction = colorMapSettings1DAction->getColorMapAction();

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    auto groupAction = new GroupAction(this);

    groupAction->setLabelSizingType(GroupAction::LabelSizingType::Auto);

    WidgetActions actions;

    actions << &colorMapAction.getRangeAction(ColorMapAction::Axis::X);

    if (colorMapAction.isConnected())
        actions << &colorMapAction.getSynchronizeWithSharedDataRangeAction();

    actions << &colorMapAction.getDataRangeAction(ColorMapAction::Axis::X);

    if (colorMapAction.isConnected())
        actions << &colorMapAction.getSharedDataRangeAction(ColorMapAction::Axis::X);
    
    actions << &colorMapAction.getMirrorAction(ColorMapAction::Axis::X);
    actions << &colorMapAction.getDiscretizeAction();
    actions << &colorMapAction.getNumberOfDiscreteStepsAction();
    actions << &colorMapAction.getDiscretizeAlphaAction();
    actions << &colorMapAction.getCustomColorMapGroupAction();

    groupAction->setActions(actions);

    layout->addWidget(groupAction->createWidget(this));

    setPopupLayout(layout);
}

}
