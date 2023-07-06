// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapSettings2DAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>

namespace hdps::gui {

ColorMapSettings2DAction::ColorMapSettings2DAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _colorMapAction(*static_cast<ColorMapAction*>(parent))
{
    setText("2D Color Map Settings");
}

ColorMapSettings2DAction::Widget::Widget(QWidget* parent, ColorMapSettings2DAction* colorMapSettings2DAction) :
    WidgetActionWidget(parent, colorMapSettings2DAction),
    _colorMapViewAction(colorMapSettings2DAction->getColorMapAction())
{
    setAutoFillBackground(true);

    auto& colorMapAction = colorMapSettings2DAction->getColorMapAction();

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    auto groupAction = new GroupAction(this, colorMapSettings2DAction->text());

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

    groupAction->addAction(&colorMapAction.getMirrorGroupAction());
    groupAction->addAction(&colorMapAction.getDiscretizeAction());
    groupAction->addAction(&colorMapAction.getNumberOfDiscreteStepsAction());
    groupAction->addAction(&colorMapAction.getDiscretizeAlphaAction());
    groupAction->addAction(&_colorMapViewAction);

    layout->addWidget(groupAction->createWidget(this));

    setLayout(layout);
}

}
