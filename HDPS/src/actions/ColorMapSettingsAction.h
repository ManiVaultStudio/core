#pragma once

#include "ColorMapAxisAction.h"
#include "ColorMapViewAction.h"
#include "ColorMapDiscreteAction.h"
#include "ColorMapSettingsOneDimensionalAction.h"
#include "ColorMapSettingsTwoDimensionalAction.h"
#include "ColorMapEditorOneDimensionalAction.h"

#include "WidgetAction.h"
#include "DecimalAction.h"
#include "DecimalRangeAction.h"
#include "ToggleAction.h"
#include "TriggerAction.h"
#include "IntegralAction.h"

namespace hdps {

namespace gui {

class ColorMapAction;

/**
 * Color map settings action class
 *
 * Group action for color map settings
 *
 * @author Thomas Kroes
 */
class ColorMapSettingsAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for color map settings action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapSettingsAction Pointer to clusters action
         */
        Widget(QWidget* parent, ColorMapSettingsAction* colorMapSettingsAction);
    };

    /**
     * Get widget representation of the color map settings action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

protected:

    /**
     * Constructor
     * @param colorMapAction Reference to color map action
     */
    ColorMapSettingsAction(ColorMapAction& colorMapAction);

protected:

    /** Update read-only status of various actions */
    void updateActionsReadOnly();

public: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    void connectToPublicAction(WidgetAction* publicAction) override;

    /** Disconnect this action from a public action */
    void disconnectFromPublicAction() override;

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }
    ColorMapAxisAction& getHorizontalAxisAction() { return _horizontalAxisAction; }
    ColorMapAxisAction& getVerticalAxisAction() { return _verticalAxisAction; }
    ColorMapDiscreteAction& getDiscreteAction() { return _discreteAction; }
    ColorMapSettingsOneDimensionalAction& getSettingsOneDimensionalAction() { return _settingsOneDimensionalAction; }
    ColorMapSettingsTwoDimensionalAction& getSettingsTwoDimensionalAction() { return _settingsTwoDimensionalAction; }
    ColorMapEditorOneDimensionalAction& getEditorOneDimensionalAction() { return _editorOneDimensionalAction; }

protected:
    ColorMapAction&                         _colorMapAction;                        /** Reference to color map action */
    ColorMapAxisAction                      _horizontalAxisAction;                  /** Horizontal axis action (used in 1D settings action) */
    ColorMapAxisAction                      _verticalAxisAction;                    /** Vertical axis action (used in 1D + 2D settings action) */
    ColorMapDiscreteAction                  _discreteAction;                        /** Discrete action (used in 1D + 2D settings action) */
    ColorMapSettingsOneDimensionalAction    _settingsOneDimensionalAction;          /** One-dimensional settings action */
    ColorMapSettingsTwoDimensionalAction    _settingsTwoDimensionalAction;          /** Two-dimensional settings action */
    ColorMapEditorOneDimensionalAction      _editorOneDimensionalAction;            /** One-dimensional editor action */

    /** Only color map actions may instantiate this class */
    friend class ColorMapAction;
};

}
}
