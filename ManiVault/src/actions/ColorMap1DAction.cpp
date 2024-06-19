// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMap1DAction.h"

using namespace mv::util;

namespace mv::gui {

ColorMap1DAction::ColorMap1DAction(QObject* parent, const QString& title, const QString& colorMap /*= "RdYlBu"*/) :
    ColorMapAction(parent, title, ColorMap::Type::OneDimensional, colorMap)
{
}

QWidget* ColorMap1DAction::createExampleWidget(QWidget* parent) const
{
    auto exampleWidget          = new QWidget(parent);
    auto exampleWidgetLayout    = new QVBoxLayout();
    auto colorMap1DAction       = new ColorMap1DAction(exampleWidget, "Example");

    exampleWidgetLayout->addWidget(colorMap1DAction->createWidget(exampleWidget));

    exampleWidget->setLayout(exampleWidgetLayout);

    return exampleWidget;
}

}
