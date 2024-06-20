// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMap1DAction.h"
#include "WidgetActionExampleWidget.h"

using namespace mv::util;

namespace mv::gui {

ColorMap1DAction::ColorMap1DAction(QObject* parent, const QString& title, const QString& colorMap /*= "RdYlBu"*/) :
    ColorMapAction(parent, title, ColorMap::Type::OneDimensional, colorMap)
{
}

QWidget* ColorMap1DAction::createExampleWidget(QWidget* parent) const
{
    auto exampleWidget      = new WidgetActionExampleWidget(parent);
    auto colorMap1DAction   = new ColorMap1DAction(exampleWidget, "Example");

    const QString markdownText =
        "## General \n\n"
        "`mv::gui::ColorMap1DAction` (derived from [<actions/ColorMapAction.h>](https://github.com/ManiVaultStudio/core/blob/master/ManiVault/src/actions/ColorMapAction.h)) provides a color map picker and editor for one-dimensional color maps. Color maps can be `flipped` horizontally, `quantized` and it features a custom `color map editor`. \n\n"
        "## Usage \n\n"
        "[#include <actions/ColorMap1DAction.h>](https://github.com/ManiVaultStudio/core/blob/master/ManiVault/src/actions/ColorMap1DAction.h)\n\n"
        "To use: \n\n"
        "- Click on the color map button and select one of the available color maps \n\n"
        "- Click on the settings button to the right of the color map button to add the settings: \n\n"
        "## API \n\n"
        "The `mv::gui::ColorMap1DAction` API inherits common actions from the `mv::gui::ColorMapAction`, see *sub-actions* sections \n\n"
        "### Methods \n\n"
        "- `void initialize(const QString& colorMap)` \n\n"
        "  Initialize from `colorMap` string \n\n"
        "- `util::ColorMap::Type getColorMapType() const` \n\n"
        "  Gets the current color map type \n\n"
        "- `QString getColorMap() const` \n\n"
        "  Gets the current color map name \n\n"
        "- `QImage getColorMapImage() const` \n\n"
        "  Gets the current color map image \n\n"
        "- `void setColorMap(const QString& colorMap)` \n\n"
        "  Sets the current color map name to `colorMap` \n\n"
        "### Signals \n\n"
        "- `void typeChanged(const util::ColorMap::Type& colorMapType)` \n\n"
        "  Signals that the color map type changed to `colorMapType` \n\n"
        "- `void imageChanged(const QImage& image)` \n\n"
        "  Signals that the color map image changed to `image` \n\n"
        "### Sub-actions \n\n"
        "- `OptionAction& getCurrentColorMapAction()` \n\n"
        "  Returns an `mv::gui::OptionAction` that controls the color map type \n\n"
        "- `DecimalRangeAction& getRangeAction(const Axis& axis)` \n\n"
        "  Returns an `mv::gui::DecimalRangeAction` for the operating range for either the horizontal or vertical axis (vertical only available in two-dimensional color map) \n\n"
        "- `DecimalRangeAction& getDataRangeAction(const Axis& axis)` \n\n"
        "  Returns an `mv::gui::DecimalRangeAction` for the data range for either the horizontal or vertical axis (vertical only available in two-dimensional color map) \n\n"
        "- `DecimalRangeAction& getSharedDataRangeAction(const Axis& axis)` \n\n"
        "  Returns an `mv::gui::DecimalRangeAction` for the shared data range for either the horizontal or vertical axis (vertical only available in two-dimensional color map) \n\n"
        "- `ToggleAction& getLockToSharedDataRangeAction()` \n\n"
        "  Returns an `mv::gui::ToggleAction` that determines whether the display range is tied to the shared data range \n\n"
        "- `ToggleAction& getMirrorAction(const Axis& axis)` \n\n"
        "  Returns an `mv::gui::ToggleAction` that determines whether the color map should be flipped in the horizontal or vertical direction (vertical only has an affect on two-dimensional color maps) \n\n"
        "- `HorizontalGroupAction& getMirrorGroupAction()` \n\n"
        "  Returns an `mv::gui::HorizontalGroupAction` that groups mirror-related actions \n\n"
        "- `ToggleAction& getDiscretizeAction()` \n\n"
        "  Returns an `mv::gui::ToggleAction` that determines whether the color map should be *discretized* or not \n\n"
        "- `IntegralAction& getNumberOfDiscreteStepsAction()` \n\n"
        "  Returns an `mv::gui::IntegralAction` which controls the number of discrete steps when the color map is in discrete mode \n\n"
        "- `ToggleAction& getDiscretizeAlphaAction()` \n\n"
        "  Returns an `mv::gui::ToggleAction` that determines whether the alpha channel of the color map should also be discretized \n\n"
        "- `ColorMapSettings1DAction& getSettings1DAction()` \n\n"
        "- `ToggleAction& getCustomColorMapAction()` \n\n"
        "  Returns an `mv::gui::ToggleAction` that determines whether to use a custom color map or a default one \n\n"
        "- `ColorMapEditor1DAction& getEditor1DAction()` \n\n"
        "  Returns an `mv::gui::ColorMapEditor1DAction` that is editor of the one-dimensional color map \n\n"
        "### Sub-actions (for internal use only) \n\n"
        "- `ColorMapSettingsAction& getSettingsAction()` \n\n"
        "  Returns an `mv::gui::ColorMapSettingsAction` that comprises the 1D and 2D settings actions \n\n"
        "- `ColorMapSettings1DAction& getSettings1DAction()` \n\n"
        "  Returns an `mv::gui::ColorMapSettings1DAction` that bundles various actions in a layout \n\n"
        "- `ColorMapSettings2DAction& getSettings2DAction()` \n\n"
        "  Returns an `mv::gui::ColorMapSettings2DAction` that bundles various actions in a layout \n\n"
        "- `HorizontalGroupAction& getCustomColorMapGroupAction()` \n\n"
        "  Returns an `mv::gui::HorizontalGroupAction` that groups the custom color map related actions \n\n";

    exampleWidget->addWidget(colorMap1DAction->createWidget(exampleWidget));
    exampleWidget->addMarkDownSection(markdownText);

    return exampleWidget;
}

}
