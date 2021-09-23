#pragma once

#include "OptionAction.h"
#include "TriggerAction.h"

namespace hdps {

namespace gui {

class PixelSelectionAction;

/**
 * Pixel selection type action class
 *
 * Action class for choosing a pixel selection type
 *
 * @author Thomas Kroes
 */
class PixelSelectionTypeAction : public WidgetAction
{
public:

    /** Describes the widget flags */
    enum WidgetFlag {
        ComboBox        = 0x00001,      /** The widget includes a combobox */
        PushButtonGroup = 0x00002,      /** The widget includes push buttons in a group */
        ResetPushButton = 0x00004,      /** The widget includes a reset push button */

        Basic   = ComboBox,
        All     = ComboBox | PushButtonGroup | ResetPushButton
    };

public:

    /** Widget class for pixel selection type action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param pixelSelectionTypeAction Pointer to pixel selection type action
         * @param state State of the widget
         */
        Widget(QWidget* parent, PixelSelectionTypeAction* pixelSelectionTypeAction, const WidgetActionWidget::State& state);

    protected:
        friend class PixelSelectionTypeAction;
    };

protected:

    /**
     * Get widget representation of the pixel selection type action
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
    PixelSelectionTypeAction(PixelSelectionAction& pixelSelectionAction);

    /** Determines whether the current value can be reset to its default */
    bool isResettable() const override;

    /** Reset the current value to the default value */
    void reset() override;

public: /** Action getters */

    OptionAction& getTypeAction() { return _typeAction; }
    TriggerAction& getRectangleAction() { return _rectangleAction; }
    TriggerAction& getBrushAction() { return _brushAction; }
    TriggerAction& getLassoAction() { return _lassoAction; }
    TriggerAction& getPolygonAction() { return _polygonAction; }

protected:
    PixelSelectionAction&   _pixelSelectionAction;      /** Reference to pixel selection action */
    OptionAction            _typeAction;                /** Selection type action */
    TriggerAction           _rectangleAction;           /** Switch to rectangle selection action */
    TriggerAction           _brushAction;               /** Switch to brush selection action */
    TriggerAction           _lassoAction;               /** Switch to lasso selection action */
    TriggerAction           _polygonAction;             /** Switch to polygon selection action */
    QActionGroup            _typeActionGroup;           /** Type action group */
};

}
}
