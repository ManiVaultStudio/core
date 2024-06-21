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
        "  Get the action alignment as a `Qt::AlignmentFlag` \n\n"
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
    exampleWidget->addMarkDownSection(markdownText);

    return exampleWidget;
}

}