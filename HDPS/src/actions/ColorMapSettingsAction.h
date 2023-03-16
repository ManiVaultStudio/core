#pragma once

#include "ColorMapAxisAction.h"
#include "ColorMapViewAction.h"
#include "ColorMapDiscreteAction.h"
#include "ColorMapSettings1DAction.h"
#include "ColorMapSettings2DAction.h"
#include "ColorMapEditor1DAction.h"

#include "WidgetAction.h"
#include "DecimalAction.h"
#include "DecimalRangeAction.h"
#include "ToggleAction.h"
#include "TriggerAction.h"
#include "IntegralAction.h"
#include "HorizontalGroupAction.h"

namespace hdps::gui {

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

private:

    void synchronizeWithSharedDataRange();

public: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    void connectToPublicAction(WidgetAction* publicAction) override;

    /** Disconnect this action from a public action */
    void disconnectFromPublicAction() override;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }
    ColorMapAxisAction& getHorizontalAxisAction() { return _horizontalAxisAction; }
    ColorMapAxisAction& getVerticalAxisAction() { return _verticalAxisAction; }
    ColorMapDiscreteAction& getDiscreteAction() { return _discreteAction; }
    ColorMapSettings1DAction& getSettings1DAction() { return _settingsOneDimensionalAction; }
    ColorMapSettings2DAction& getSettings2DAction() { return _settingsTwoDimensionalAction; }
    ColorMapEditor1DAction& getEditor1DAction() { return _editorOneDimensionalAction; }
    ToggleAction& getSynchronizeWithLocalDataRange() { return _synchronizeWithLocalDataRange; }
    ToggleAction& getSynchronizeWithSharedDataRange() { return _synchronizeWithSharedDataRange; }
    DecimalRangeAction& getLocalDataRangeOneDimensionalAction() { return _localDataRangeOneDimensionalAction; }
    DecimalRangeAction& getSharedDataRangeOneDimensionalAction() { return _sharedDataRangeOneDimensionalAction; }

protected:
    ColorMapAction&             _colorMapAction;                        /** Reference to color map action */
    ColorMapAxisAction          _horizontalAxisAction;                  /** Horizontal axis action (used in 1D settings action) */
    ColorMapAxisAction          _verticalAxisAction;                    /** Vertical axis action (used in 1D + 2D settings action) */
    ColorMapDiscreteAction      _discreteAction;                        /** Discrete action (used in 1D + 2D settings action) */
    ColorMapSettings1DAction    _settingsOneDimensionalAction;          /** One-dimensional settings action */
    ColorMapSettings2DAction    _settingsTwoDimensionalAction;          /** Two-dimensional settings action */
    ColorMapEditor1DAction      _editorOneDimensionalAction;            /** One-dimensional editor action */
    ToggleAction                _synchronizeWithLocalDataRange;         /** Synchronize with local data range */
    ToggleAction                _synchronizeWithSharedDataRange;        /** Synchronize with shared data range */
    DecimalRangeAction          _localDataRangeOneDimensionalAction;    /** One-dimensional local data range of the dataset which is associated with the color map */
    DecimalRangeAction          _sharedDataRangeOneDimensionalAction;   /** One-dimensional global data range (in case the color map is public, this data range is the absolute min/max of all connected color maps) */

    /** Only color map actions may instantiate this class */
    friend class ColorMapAction;
};

}
