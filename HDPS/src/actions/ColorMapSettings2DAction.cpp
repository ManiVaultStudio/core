#include "ColorMapSettings2DAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>

namespace hdps::gui {

ColorMapSettings2DAction::ColorMapSettings2DAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction)
{
    setText("Settings 2D");
    setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
}

ColorMapSettings2DAction::Widget::Widget(QWidget* parent, ColorMapSettings2DAction* colorMapSettings2DAction) :
    WidgetActionWidget(parent, colorMapSettings2DAction),
    _colorMapViewAction(colorMapSettings2DAction->getColorMapAction())
{
    setAutoFillBackground(true);

    auto& colorMapAction = colorMapSettings2DAction->getColorMapAction();

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    auto groupAction = new GroupAction(this);

    groupAction->setLabelSizingType(GroupAction::LabelSizingType::Auto);

    WidgetActions actions;

    //actions << &colorMapAction.getRangeAction(ColorMapAction::Axis::X);
    //actions << &colorMapAction.getRangeAction(ColorMapAction::Axis::Y);

    //if (colorMapAction.isConnected())
    //    actions << &colorMapAction.getSynchronizeWithSharedDataRangeAction();

    //actions << &colorMapAction.getDataRangeAction(ColorMapAction::Axis::X);
    //actions << &colorMapAction.getDataRangeAction(ColorMapAction::Axis::Y);

    if (colorMapAction.isConnected()) {
        actions << &colorMapAction.getSharedDataRangeAction(ColorMapAction::Axis::X);
        actions << &colorMapAction.getSharedDataRangeAction(ColorMapAction::Axis::Y);
    }

    actions << &colorMapAction.getMirrorGroupAction();
    actions << &colorMapAction.getDiscretizeAction();
    actions << &colorMapAction.getNumberOfDiscreteStepsAction();
    actions << &colorMapAction.getDiscretizeAlphaAction();
    actions << &_colorMapViewAction;

    groupAction->setActions(actions);

    layout->addWidget(groupAction->createWidget(this));

    setPopupLayout(layout);
}

}
