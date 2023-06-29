#include "DimensionsPickerMiscellaneousAction.h"
#include "DimensionsPickerAction.h"

#include <Application.h>

namespace hdps::gui {

DimensionsPickerMiscellaneousAction::DimensionsPickerMiscellaneousAction(DimensionsPickerAction& dimensionsPickerAction) :
    WidgetAction(&dimensionsPickerAction, "Miscellaneous"),
    _dimensionsPickerAction(dimensionsPickerAction),
    _showOnlySelectedDimensionsAction(this, "Show only selected dimensions"),
    _applyExclusionListAction(this, "Apply exclusion list"),
    _ignoreZeroValuesAction(this, "Ignore zero values")
{
    setText("Settings");
    setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));

    // Set only show selected dimensions when the corresponding action is triggered
    connect(&_showOnlySelectedDimensionsAction, &ToggleAction::triggered, this, [this](bool checked) {
        _dimensionsPickerAction.setShowOnlySelectedDimensions(checked);
    });

    // Set apply exclusion list option when the corresponding action is triggered
    connect(&_applyExclusionListAction, &ToggleAction::triggered, this, [this](bool checked) {
        _dimensionsPickerAction.setApplyExclusionList(checked);
    });

    // Set ignore zero option when the corresponding action is triggered
    connect(&_ignoreZeroValuesAction, &ToggleAction::triggered, this, [this](bool checked) {
        _dimensionsPickerAction.setIgnoreZeroValues(checked);
    });
}

DimensionsPickerMiscellaneousAction::Widget::Widget(QWidget* parent, DimensionsPickerMiscellaneousAction* dimensionsPickerMiscellaneousAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, dimensionsPickerMiscellaneousAction, widgetFlags)
{
    auto layout = new QVBoxLayout();

    layout->addWidget(dimensionsPickerMiscellaneousAction->getShowOnlySelectedDimensionsAction().createWidget(this, ToggleAction::CheckBox));
    layout->addWidget(dimensionsPickerMiscellaneousAction->getApplyExclusionListAction().createWidget(this, ToggleAction::CheckBox));
    layout->addWidget(dimensionsPickerMiscellaneousAction->getIgnoreZeroValuesAction().createWidget(this, ToggleAction::CheckBox));

    setLayout(layout);
}

}
