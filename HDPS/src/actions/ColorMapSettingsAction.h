#pragma once

#include "WidgetAction.h"

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
    Q_INVOKABLE ColorMapSettingsAction(ColorMapAction& colorMapAction);

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }

protected:
    ColorMapAction&     _colorMapAction;        /** Reference to owning color map action */

    /** Only color map may instantiate this class */
    friend class ColorMapAction;
};

}

Q_DECLARE_METATYPE(hdps::gui::ColorMapSettingsAction)

inline const auto colorMapSettingsActionMetaTypeId = qRegisterMetaType<hdps::gui::ColorMapSettingsAction*>("ColorMapSettingsAction");
