#pragma once

#include "ToolbarItemWidget.h"

#include <QHBoxLayout>
#include <QFrame>
#include <QVariantAnimation>

namespace hdps {

namespace gui {

class WidgetAction;

/**
 * Stateful item class
 *
 * Selectively shows either a standard or collapsed widget of an action
 *
 * @author Thomas Kroes
 */
class ToolbarSpacerWidget : public ToolbarItemWidget
{

public:

    /** Describes the spacer types */
    enum State {
        Undefined,      /** No spacer type defined */
        Divider,        /** Divide with a line */
        Spacer,         /** Divide with a small space */
        Hidden          /** Spacer is hidden */
    };

public:

    /**
     * Constructor
     * @param toolbarWidget Pointer to toolbar widget
     * @param index Index
     * @param action Reference to action
     * @param action Reference to action
     */
    ToolbarSpacerWidget(ToolbarWidget* toolbarWidget, WidgetAction& action, std::int32_t priority);

    /**
     * Invoked when the state changed
     * @return previousState Previous state of the item widget
     * @return currentState Current state of the item widget
     */
    void stateChanged(std::int32_t previousState, std::int32_t currentState) override;

    /**
     * Get width
     * @return Width of the stateful item widget
     */
    std::int32_t getWidth() const override;

    /**
     * Get width
     * @param state State for which to fetch the width
     * @return Width
     */
    std::int32_t getWidth(const std::int32_t& state) const override;

protected:
    ToolbarWidget*      _toolbarWidget;     /** Pointer to owning toolbar widget */
    QHBoxLayout         _layout;            /** Widget layout */
    QFrame              _verticalLine;      /** Vertical line */
    QVariantAnimation   _sizeAnimation;     /** Animation to control the size of the widget */
};

}
}
