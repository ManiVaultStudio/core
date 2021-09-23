#pragma once

#include "actions/Actions.h"

using namespace hdps::gui;

class PixelSelectionAction;

/**
 * Pixel selection operations action class
 *
 * Action class for configuring pixel selection operations (select all/none/invert)
 *
 * @author Thomas Kroes
 */
class PixelSelectionOperationsAction : public WidgetAction
{
public:

    /** Widget class for pixel selection operations action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param pixelSelectionOperationsAction Pointer to pixel selection operations action
         * @param state State of the widget
         */
        Widget(QWidget* parent, PixelSelectionOperationsAction* pixelSelectionOperationsAction, const WidgetActionWidget::State& state);

    protected:
        friend class PixelSelectionOperationsAction;
    };

protected:

    /**
     * Get widget representation of the pixel selection operations action
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
    PixelSelectionOperationsAction(PixelSelectionAction& pixelSelectionAction);

public: /** Action getters */

    TriggerAction& getClearSelectionAction() { return _clearSelectionAction; }
    TriggerAction& getSelectAllAction() { return _selectAllAction; }
    TriggerAction& getInvertSelectionAction() { return _invertSelectionAction; }

protected:
    PixelSelectionAction&   _pixelSelectionAction;      /** Reference to pixel selection action */
    TriggerAction           _clearSelectionAction;      /** Clear selection action */
    TriggerAction           _selectAllAction;           /** Select all action */
    TriggerAction           _invertSelectionAction;     /** Invert selection action */
};
