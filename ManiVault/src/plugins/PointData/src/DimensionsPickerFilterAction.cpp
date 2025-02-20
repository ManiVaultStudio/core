// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DimensionsPickerFilterAction.h"
#include "DimensionsPickerAction.h"

#include <Application.h>

#include <QHBoxLayout>

using namespace mv::util;

namespace mv::gui {

DimensionsPickerFilterAction::DimensionsPickerFilterAction(DimensionsPickerAction& dimensionsPickerAction) :
    WidgetAction(&dimensionsPickerAction, "Filter"),
    _dimensionsPickerAction(dimensionsPickerAction),
    _nameFilterAction(this, "Name filter")
{
    setText("Filter");
    setIconByName("filter");

    // Update the proxy model filter name when the name filter action string changes
    connect(&_nameFilterAction, &StringAction::stringChanged, this, [this](const QString& name) {
        _dimensionsPickerAction.setNameFilter(name);
    });

    // Show and configure trailing action for the name filter
    _nameFilterAction.getTrailingAction().setVisible(true);
    _nameFilterAction.getTrailingAction().setIcon(StyledIcon("times-circle"));

    // Reset the name filter when the trailing action is triggered
    connect(&_nameFilterAction.getTrailingAction(), &QAction::triggered, this, [this]() -> void {
        _nameFilterAction.setString("");
    });

    // Update the name filter trailing action visibility based on the name filter
    const auto updateTrailingActionVisibility = [this]() {
        _nameFilterAction.getTrailingAction().setVisible(!_nameFilterAction.getString().isEmpty());
    };

    // Update the name filter trailing action visibility when the name filter changes
    connect(&_nameFilterAction, &StringAction::stringChanged, this, updateTrailingActionVisibility);

    // Initial update of trailing action visibility
    updateTrailingActionVisibility();
}

DimensionsPickerFilterAction::Widget::Widget(QWidget* parent, DimensionsPickerFilterAction* dimensionsPickerFilterAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, dimensionsPickerFilterAction, widgetFlags)
{
    auto layout = new QHBoxLayout();

    layout->addWidget(dimensionsPickerFilterAction->getNameFilterAction().createLabelWidget(this));
    layout->addWidget(dimensionsPickerFilterAction->getNameFilterAction().createWidget(this));

    setLayout(layout);
}

}
