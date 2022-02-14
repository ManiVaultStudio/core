#include "DimensionsPickerFilterAction.h"
#include "DimensionsPickerAction.h"

namespace hdps {

namespace gui {

DimensionsPickerFilterAction::DimensionsPickerFilterAction(DimensionsPickerAction& dimensionsPickerAction) :
    WidgetAction(&dimensionsPickerAction),
    _dimensionsPickerAction(dimensionsPickerAction),
    _nameFilterAction(this, "Name filter")
{
    setText("Filter");
    setIcon(Application::getIconFont("FontAwesome").getIcon("filter"));

    // Update the proxy model filter name when the name filter action string changes
    connect(&_nameFilterAction, &StringAction::stringChanged, this, [this](const QString& name) {
        _dimensionsPickerAction.setNameFilter(name);
    });

    // Show and configure trailing action for the name filter
    _nameFilterAction.getTrailingAction().setVisible(true);
    _nameFilterAction.getTrailingAction().setIcon(Application::getIconFont("FontAwesome").getIcon("times-circle"));

    // Reset the name filter when the trailing action is triggered
    connect(&_nameFilterAction.getTrailingAction(), &QAction::triggered, &_nameFilterAction, &StringAction::reset);

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

    if (widgetFlags & WidgetActionWidget::PopupLayout) {
        setPopupLayout(layout);
    }
    else {
        layout->setContentsMargins(0,0,0,0);
        setLayout(layout);
    }
}

}
}
