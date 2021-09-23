#include "PixelSelectionModifierAction.h"
#include "PixelSelectionAction.h"
#include "Application.h"

#include "util/PixelSelectionTool.h"

PixelSelectionModifierAction::PixelSelectionModifierAction(PixelSelectionAction& pixelSelectionAction) :
    WidgetAction(&pixelSelectionAction),
    _pixelSelectionAction(pixelSelectionAction),
    _modifierAddAction(this, "Add to selection"),
    _modifierRemoveAction(this, "Remove from selection"),
    _modifierActionGroup(this)
{
    setText("Selection modifier");

    auto targetWidget = _pixelSelectionAction.getTargetWidget();

    auto& pixelSelectionTool = _pixelSelectionAction.getPixelSelectionTool();

    targetWidget->addAction(&_modifierAddAction);
    targetWidget->addAction(&_modifierRemoveAction);

    _modifierAddAction.setCheckable(true);
    _modifierRemoveAction.setCheckable(true);

    const auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _modifierAddAction.setIcon(fontAwesome.getIcon("plus"));
    _modifierRemoveAction.setIcon(fontAwesome.getIcon("minus"));

    _modifierAddAction.setToolTip("Add items to the existing selection");
    _modifierRemoveAction.setToolTip("Remove items from the existing selection");

    _modifierActionGroup.addAction(&_modifierAddAction);
    _modifierActionGroup.addAction(&_modifierRemoveAction);

    connect(&_modifierAddAction, &QAction::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setModifier(checked ? PixelSelectionTool::Modifier::Add : PixelSelectionTool::Modifier::Replace);
    });

    connect(&_modifierRemoveAction, &QAction::toggled, [this, &pixelSelectionTool](bool checked) {
        pixelSelectionTool.setModifier(checked ? PixelSelectionTool::Modifier::Remove : PixelSelectionTool::Modifier::Replace);
    });

    const auto updateResettable = [this]() -> void {
        setResettable(_modifierAddAction.isResettable() || _modifierRemoveAction.isResettable());
    };

    connect(&_modifierAddAction, &ColorAction::resettableChanged, this, updateResettable);
    connect(&_modifierRemoveAction, &DecimalAction::resettableChanged, this, updateResettable);

    updateResettable();
}

PixelSelectionModifierAction::Widget::Widget(QWidget* parent, PixelSelectionModifierAction* pixelSelectionModifierAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, pixelSelectionModifierAction, state)
{
    auto layout = new QHBoxLayout();

    layout->setSpacing(3);
    layout->setMargin(0);

    layout->addWidget(pixelSelectionModifierAction->getModifierAddAction().createWidget(this));
    layout->addWidget(pixelSelectionModifierAction->getModifierRemoveAction().createWidget(this));
    layout->addWidget(new WidgetActionResetButton(pixelSelectionModifierAction));

    setLayout(layout);
}
