#pragma once

#include "actions/Actions.h"

using namespace hdps::gui;

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

    /** 
     * Constructor
     * @param pixelSelectionAction Reference to pixel selection action
     */
    PixelSelectionTypeAction(PixelSelectionAction& pixelSelectionAction);

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
