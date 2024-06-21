// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HorizontalGroupAction.h"

#include "WidgetActionExampleWidget.h"

namespace mv::gui {

HorizontalGroupAction::HorizontalGroupAction(QObject* parent, const QString& title, const bool& expanded /*= false*/) :
    GroupAction(parent, title, expanded)
{
    setDefaultWidgetFlags(GroupAction::Horizontal);
}

QWidget* HorizontalGroupAction::createExampleWidget(QWidget* parent) const
{
    auto exampleWidget          = new WidgetActionExampleWidget(parent);
    auto horizontalGroupAction  = new HorizontalGroupAction(exampleWidget, "Example");
    auto colorAction            = new ColorAction(exampleWidget, "Color");
    auto inputAAction           = new StringAction(exampleWidget, "Input A");
    auto inputBAction           = new StringAction(exampleWidget, "Input B");

    inputAAction->setPlaceHolderString("Input first string here...");
    inputBAction->setPlaceHolderString("Input second string here...");

    horizontalGroupAction->addAction(colorAction);
    horizontalGroupAction->addAction(inputAAction);
    horizontalGroupAction->addAction(inputBAction);

    const QString markdownText =
        "## General \n\n"
        "`mv::gui::HorizontalGroupAction` (derived from [<actions/GroupAction.h>](https://github.com/ManiVaultStudio/core/blob/master/ManiVault/src/actions/GroupAction.h) groups actions `horizontally`. \n\n"
        "## Usage \n\n"
        "[#include <actions/HorizontalGroupAction.h>](https://github.com/ManiVaultStudio/core/blob/master/ManiVault/src/actions/HorizontalGroupAction.h)\n\n"
        "To use: \n\n"
        "- Create an instance of `HorizontalGroupAction` \n\n"
        "- Add actions using `HorizontalGroupAction::addAction(...)` \n\n"
        "- Remove actions using `HorizontalGroupAction::removeAction(...)` \n\n"
        "## API \n\n"
        "### Alignment \n\n"
        "- `Qt::AlignmentFlag getAlignment() const final` \n\n"
        "  Get the action alignment as a `Qt::AlignmentFlag` (only applies to vertical layouts) \n\n"
        "### Expansion \n\n"
        "- `void setExpanded(const bool& expanded)` \n\n"
        "  Set expansion state to `expanded` (when added to a [GroupsAction](https://github.com/ManiVaultStudio/core/blob/master/ManiVault/src/actions/GroupsAction.h)) \n\n"
        "- `void expand()` \n\n"
        "  Expand the group \n\n"
        "- `void collapse()` \n\n"
        "  Collapse the group \n\n"
        "- `void toggle()` \n\n"
        "  Toggles the expansion state \n\n"
        "- `bool isExpanded() const` \n\n"
        "  Establishes whether the group is in an expanded state or not \n\n"
        "- `bool isCollapsed() const` \n\n"
        "  Establishes whether the group is in an collapsed state or not \n\n"
        "### Read-only \n\n"
        "- `bool isReadOnly() const` \n\n"
        "  Establishes whether the group is in a read-only state or not \n\n"
        "- `void setReadOnly(const bool& readOnly)` \n\n"
        "  Set the read-only state to `readOnly` \n\n"
        "### Label visibility \n\n"
        "- `bool getShowLabels() const` \n\n"
        "  Establishes whether the group shows an action label that precedes the action \n\n"
        "- `void setShowLabels(bool showLabels)` \n\n"
        "  Set show labels to `showLabels` \n\n"
        "### Stretch \n\n"
        "- `StretchAction* addStretch(std::int32_t stretch = 1)` \n\n"
        "  Add `stretch` at the current location n \n\n"
        "### Actions management \n\n"
        "- `void addAction(WidgetAction* action, std::int32_t widgetFlags = -1, WidgetConfigurationFunction widgetConfigurationFunction = WidgetConfigurationFunction())` \n\n"
        "  Add `action` to the group with possibly `widgetFlags` and `widgetConfigurationFunction` \n\n"
        "- `void removeAction(WidgetAction* action) final` \n\n"
        "  Remove `action` from the group \n\n"
        "- `void clear() final` \n\n"
        "  Remove all actions from the group \n\n"
        "- `WidgetActions getActions() final` \n\n"
        "  Get all added actions \n\n"
        "### Signals \n\n"
        "- `void actionsChanged(const WidgetActions& actions)` \n\n"
        "  Signals that the actions changed to `actions` \n\n"
        "- `void expanded()` \n\n"
        "  Signals that the group got expanded \n\n"
        "- `void collapsed()` \n\n"
        "  Signals that the group got collapsed \n\n"
        "- `void readOnlyChanged(const bool& readOnly)` \n\n"
        "  Signals that the read-only status changed to `readOnly` \n\n"
        "- `void showLabelsChanged(const bool& showLabels)` \n\n"
        "  Signals that the show labels state changed to `showLabels` \n\n";

    exampleWidget->addWidget(horizontalGroupAction->createWidget(exampleWidget));

    auto settingsAction         = new VerticalGroupAction(exampleWidget, "Settings");
    auto showLabelsAction       = new ToggleAction(exampleWidget, "Show labels", horizontalGroupAction->getShowLabels());
    auto stretchAction          = new VerticalGroupAction(exampleWidget, "Stretch");
    auto colorStretchAction     = new IntegralAction(exampleWidget, "Color", 0, 10, 0);
    auto inputAStretchAction    = new IntegralAction(exampleWidget, "Input A", 0, 10, 0);
    auto inputBStretchAction    = new IntegralAction(exampleWidget, "Input B", 0, 10, 0);
    auto sortIndexAction        = new VerticalGroupAction(exampleWidget, "Sort index");
    auto colorSortIndexAction   = new IntegralAction(exampleWidget, "Color", -1, 10, -1);
    auto inputASortIndexAction  = new IntegralAction(exampleWidget, "Input A", -1, 10, -1);
    auto inputBSortIndexAction  = new IntegralAction(exampleWidget, "Input B", -1, 10, -1);

    settingsAction->setLabelSizingType(GroupAction::LabelSizingType::Auto);

    stretchAction->setLabelSizingType(GroupAction::LabelSizingType::Auto);
    stretchAction->setDefaultWidgetFlag(GroupAction::WidgetFlag::NoMargins);

    settingsAction->addAction(showLabelsAction);
    settingsAction->addAction(stretchAction);
    settingsAction->addAction(sortIndexAction);

    stretchAction->addAction(colorStretchAction);
    stretchAction->addAction(inputAStretchAction);
    stretchAction->addAction(inputBStretchAction);

    sortIndexAction->setLabelSizingType(GroupAction::LabelSizingType::Auto);
    sortIndexAction->setDefaultWidgetFlag(GroupAction::WidgetFlag::NoMargins);

    sortIndexAction->addAction(colorSortIndexAction);
    sortIndexAction->addAction(inputASortIndexAction);
    sortIndexAction->addAction(inputBSortIndexAction);

    const auto updateShowLabels = [horizontalGroupAction, showLabelsAction]() -> void {
        horizontalGroupAction->setShowLabels(showLabelsAction->isChecked());
    };

    updateShowLabels();

    connect(showLabelsAction, &ToggleAction::toggled, exampleWidget, updateShowLabels);

    const auto updateStretchColor = [colorAction, colorStretchAction]() -> void {
        colorAction->setStretch(colorStretchAction->getValue());
    };

    const auto updateStretchInputA = [inputAAction, inputAStretchAction]() -> void {
        inputAAction->setStretch(inputAStretchAction->getValue());
    };

    const auto updateStretchInputB = [inputBAction, inputBStretchAction]() -> void {
        inputBAction->setStretch(inputBStretchAction->getValue());
    };

    updateStretchColor();
    updateStretchInputA();
    updateStretchInputB();

    connect(colorStretchAction, &IntegralAction::valueChanged, exampleWidget, updateStretchColor);
    connect(inputAStretchAction, &IntegralAction::valueChanged, exampleWidget, updateStretchInputA);
    connect(inputBStretchAction, &IntegralAction::valueChanged, exampleWidget, updateStretchInputB);

    const auto updateSortIndexColor = [colorAction, colorSortIndexAction]() -> void {
        colorAction->setSortIndex(colorSortIndexAction->getValue());
    };

    const auto updateSortIndexInputA = [inputAAction, inputASortIndexAction]() -> void {
        inputAAction->setSortIndex(inputASortIndexAction->getValue());
    };

    const auto updateSortIndexInputB = [inputBAction, inputBSortIndexAction]() -> void {
        inputBAction->setSortIndex(inputBSortIndexAction->getValue());
    };

    updateSortIndexColor();
    updateSortIndexInputA();
    updateSortIndexInputB();

    connect(colorSortIndexAction, &IntegralAction::valueChanged, exampleWidget, updateSortIndexColor);
    connect(inputASortIndexAction, &IntegralAction::valueChanged, exampleWidget, updateSortIndexInputA);
    connect(inputBSortIndexAction, &IntegralAction::valueChanged, exampleWidget, updateSortIndexInputB);

    exampleWidget->addWidget(settingsAction->createWidget(exampleWidget));

    exampleWidget->addMarkDownSection(markdownText);

    return exampleWidget;
}

}