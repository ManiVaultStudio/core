#include "ColorMapSettings1DAction.h"
#include "ColorMapAction.h"
#include "GroupAction.h"
#include "Application.h"

#include <QVBoxLayout>

namespace hdps::gui {

ColorMapSettings1DAction::ColorMapSettings1DAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _colorMapAction(*static_cast<ColorMapAction*>(parent))
{
    setText("1D Color Map Settings");
}

ColorMapSettings1DAction::Widget::Widget(QWidget* parent, ColorMapSettings1DAction* colorMapSettings1DAction) :
    WidgetActionWidget(parent, colorMapSettings1DAction)
{
    setAutoFillBackground(true);

    auto& colorMapAction = colorMapSettings1DAction->getColorMapAction();

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    auto groupAction = new GroupAction(this, colorMapSettings1DAction->text());

    groupAction->setLabelSizingType(GroupAction::LabelSizingType::Auto);

    WidgetActions actions;

    actions << &colorMapAction.getRangeAction(ColorMapAction::Axis::X);

    if (colorMapAction.isConnected())
        actions << &colorMapAction.getLockToSharedDataRangeAction();

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
