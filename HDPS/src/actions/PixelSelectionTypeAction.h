#pragma once

#include "OptionAction.h"
#include "TriggerAction.h"

#include "util/PixelSelection.h"

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
     * @param pixelSelectionTypes Allowed pixel selection types
     */
    PixelSelectionTypeAction(PixelSelectionAction& pixelSelectionAction, const util::PixelSelectionTypes& pixelSelectionTypes = util::defaultPixelSelectionTypes);

    /** Determines whether the current value can be reset to its default */
    bool isResettable() const override;

    /** Reset the current value to the default value */
    void reset() override;

    /** Get allowed pixel selection types */
    util::PixelSelectionTypes getAllowedTypes() const;

    /** Set allowed pixel selection types */
    void setAllowedTypes(const util::PixelSelectionTypes& pixelSelectionTypes);

public: /** Action getters */

    OptionAction& getTypeAction() { return _typeAction; }
    TriggerAction& getRectangleAction() { return _rectangleAction; }
    TriggerAction& getBrushAction() { return _brushAction; }
    TriggerAction& getLassoAction() { return _lassoAction; }
    TriggerAction& getPolygonAction() { return _polygonAction; }
    TriggerAction& getSampleAction() { return _sampleAction; }

protected:
    PixelSelectionAction&       _pixelSelectionAction;      /** Reference to pixel selection action */
    util::PixelSelectionTypes   _pixelSelectionTypes;       /** Pixel selection types */
    OptionAction                _typeAction;                /** Selection type action */
    TriggerAction               _rectangleAction;           /** Switch to rectangle selection action */
    TriggerAction               _brushAction;               /** Switch to brush selection action */
    TriggerAction               _lassoAction;               /** Switch to lasso selection action */
    TriggerAction               _polygonAction;             /** Switch to polygon selection action */
    TriggerAction               _sampleAction;              /** Switch to sample selection action */
    QActionGroup                _typeActionGroup;           /** Type action group */
};

}
}
