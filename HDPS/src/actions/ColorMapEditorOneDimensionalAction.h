#pragma once

#include "WidgetAction.h"

namespace hdps {

namespace gui {

class ColorMapAction;

/**
 * Color map editor action class
 *
 * Action class for manually defining a one-dimensional color map
 *
 * @author Thomas Kroes
 */
class ColorMapEditorOneDimensionalAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for color map editor action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapEditorAction Pointer to color map editor action
         */
        Widget(QWidget* parent, ColorMapEditorOneDimensionalAction* colorMapEditorOneDimensionalAction);
    };

    /**
     * Get widget representation of the color map editor action
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
    ColorMapEditorOneDimensionalAction(ColorMapAction& colorMapAction);

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

protected:
    ColorMapAction&     _colorMapAction;    /** Reference to color map action */

    /** Only color map settings action may instantiate this class */
    friend class ColorMapSettingsAction;
};

}
}
