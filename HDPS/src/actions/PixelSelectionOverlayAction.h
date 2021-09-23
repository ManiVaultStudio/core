#pragma once

#include "actions/Actions.h"

using namespace hdps::gui;

class PixelSelectionAction;

/**
 * Pixel selection overlay action class
 *
 * Action class for configuring pixel selection overlay settings (color + opacity)
 *
 * @author Thomas Kroes
 */
class PixelSelectionOverlayAction : public WidgetAction
{
public:

    /** Widget class for pixel selection overlay action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param pixelSelectionOverlayAction Pointer to pixel selection overlay action
         * @param state State of the widget
         */
        Widget(QWidget* parent, PixelSelectionOverlayAction* pixelSelectionOverlayAction, const WidgetActionWidget::State& state);

    protected:
        friend class PixelSelectionOverlayAction;
    };

protected:

    /**
     * Get widget representation of the settings action
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
    PixelSelectionOverlayAction(PixelSelectionAction& pixelSelectionAction);

public: /** Action getters */

    ColorAction& getSelectionOverlayColor() { return _selectionOverlayColor; }
    DecimalAction& getSelectionOverlayOpacity() { return _selectionOverlayOpacity; }

protected:
    PixelSelectionAction&   _pixelSelectionAction;      /** Reference to pixel selection action */
    ColorAction             _selectionOverlayColor;     /** Selection overlay color action */
    DecimalAction           _selectionOverlayOpacity;   /** Selection overlay opacity action */
};
