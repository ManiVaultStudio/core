// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapSettings1DAction.h"

#include "ColorMapAction.h"
#include "GroupAction.h"

#include <QVBoxLayout>

namespace mv::gui {

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
    
    groupAction->addAction(&colorMapAction.getMirrorAction(ColorMapAction::Axis::X));
    groupAction->addAction(&colorMapAction.getRangeAction(ColorMapAction::Axis::X));
    groupAction->addAction(&colorMapAction.getDiscretizeAction());
    groupAction->addAction(&colorMapAction.getNumberOfDiscreteStepsAction());
    groupAction->addAction(&colorMapAction.getDiscretizeAlphaAction());
    groupAction->addAction(&colorMapAction.getCustomColorMapGroupAction());

    layout->addWidget(groupAction->createWidget(this));

    setLayout(layout);
}

}
