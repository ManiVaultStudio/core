#pragma once

#include "actions/Actions.h"

using namespace hdps::gui;

class PixelSelectionAction;

/**
 * Pixel selection modifier action class
 *
 * Action class for configuring pixel selection modifier settings (add or remove)
 *
 * @author Thomas Kroes
 */
class PixelSelectionModifierAction : public WidgetAction
{
public:

    /** Widget class for pixel selection modifier action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param pixelSelectionModifierAction Pointer to pixel selection modifier action
         * @param state State of the widget
         */
        Widget(QWidget* parent, PixelSelectionModifierAction* pixelSelectionModifierAction, const WidgetActionWidget::State& state);

    protected:
        friend class PixelSelectionModifierAction;
    };

protected:

    /**
     * Get widget representation of the pixel selection modifier action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

public:

    /** 
     * Constructor
     * @param pixelSelectionAction Reference to pixel selection action
     */
    PixelSelectionModifierAction(PixelSelectionAction& pixelSelectionAction);

public: /** Action getters */

    ToggleAction& getModifierAddAction() { return _modifierAddAction; }
    ToggleAction& getModifierRemoveAction() { return _modifierRemoveAction; }

protected:
    PixelSelectionAction&   _pixelSelectionAction;      /** Reference to pixel selection action */
    ToggleAction            _modifierAddAction;         /** Add to current selection action */
    ToggleAction            _modifierRemoveAction;      /** Remove from current selection action */
    QActionGroup            _modifierActionGroup;       /** Modifier action group */
};
