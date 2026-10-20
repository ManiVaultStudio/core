// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapSettingsAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>

using namespace mv::util;

namespace mv::gui {

ColorMapSettingsAction::ColorMapSettingsAction(ColorMapAction& colorMapAction, const QString& title) :
    WidgetAction(&colorMapAction, title),
    _colorMapAction(colorMapAction)
{
    setText("Settings");
    setIconByName("sliders");
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
