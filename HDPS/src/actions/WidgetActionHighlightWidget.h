#pragma once

#include "widgets/OverlayWidget.h"

#include "WidgetAction.h"

namespace hdps::gui {

/**
 * Widget action highlight class
 *
 * Overlay widget class for highlighting a widget action
 *
 * @author Thomas Kroes
 */
class WidgetActionHighlightWidget final : public OverlayWidget
{
public:

    /**
     * Construct with parent widget
     * @param parent Pointer to parent widget
     * @param action Pointer to widget action to highlight
     */
    WidgetActionHighlightWidget(QWidget* parent, WidgetAction* action);

    /**
     * Get the highlight action
     * @return Pointer to highlight action
     */
    WidgetAction* getAction();

    /**
     * Set the highlight action
     * @param action Pointer to highlight action
     */
    void setAction(WidgetAction* action);

private:

    /**
     * Invoked when the action highlighting state changes (update the visual representation of the highlight)
     * @param highlighting Action highlighting state
     */
    void highlightingChanged(const WidgetAction::HighlightOption& highlighting);

private:
    WidgetAction*   _action;  /** Pointer to widget action to highlight */
};

}