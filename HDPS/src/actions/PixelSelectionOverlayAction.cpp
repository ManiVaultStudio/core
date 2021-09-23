#include "PixelSelectionOverlayAction.h"
#include "PixelSelectionAction.h"

#include "util/PixelSelectionTool.h"

PixelSelectionOverlayAction::PixelSelectionOverlayAction(PixelSelectionAction& pixelSelectionAction) :
    WidgetAction(&pixelSelectionAction),
    _pixelSelectionAction(pixelSelectionAction),
    _selectionOverlayColor(this, "Color", Qt::red, Qt::red),
    _selectionOverlayOpacity(this, "Opacity", 0.0f, 100.0f, 50.0f, 50.0f, 1)
{
    setText("Selection Overlay");

    const auto updateResettable = [this]() -> void {
        setResettable(_selectionOverlayColor.isResettable() || _selectionOverlayOpacity.isResettable());
    };

    connect(&_selectionOverlayColor, &ColorAction::resettableChanged, this, updateResettable);
    connect(&_selectionOverlayOpacity, &DecimalAction::resettableChanged, this, updateResettable);

    updateResettable();
}

PixelSelectionOverlayAction::Widget::Widget(QWidget* parent, PixelSelectionOverlayAction* pixelSelectionOverlayAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, pixelSelectionOverlayAction, state)
{
    auto layout = new QHBoxLayout();

    layout->setSpacing(3);
    layout->setMargin(0);

    layout->addWidget(pixelSelectionOverlayAction->getSelectionOverlayColor().createWidget(this));
    layout->addWidget(pixelSelectionOverlayAction->getSelectionOverlayOpacity().createWidget(this));
    layout->addWidget(new WidgetActionResetButton(pixelSelectionOverlayAction));

    setLayout(layout);
}
