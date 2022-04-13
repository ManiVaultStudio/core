#pragma once

#include "WidgetAction.h"
#include "ColorMapEditor1DNodeAction.h"
#include "ColorMapEditor1DWidget.h"

#include "TriggerAction.h"

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
class ColorMapEditor1DAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for one-dimensional color map editor action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapEditor1DAction Pointer to one-dimensional color map editor action
         */
        Widget(QWidget* parent, ColorMapEditor1DAction* colorMapEditor1DAction);

    protected:
        ColorMapEditor1DWidget      _colorMapEditor1DWidget;        /** Color map editor widget */
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
    ColorMapEditor1DAction(ColorMapAction& colorMapAction);

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
    ColorMapEditor1DNodeAction& getNodeAction() { return _nodeAction; }
    TriggerAction& getGoToFirstNodeAction() { return _goToFirstNodeAction; }
    TriggerAction& getGoToPreviousNodeAction() { return _goToPreviousNodeAction; }
    TriggerAction& getGoToNextNodeAction() { return _goToNextNodeAction; }
    TriggerAction& getGoToLastNodeAction() { return _goToLastNodeAction; }
    TriggerAction& getRemoveNodeAction() { return _removeNodeAction; }

protected:
    ColorMapAction&                 _colorMapAction;            /** Reference to color map action */
    ColorMapEditor1DNodeAction      _nodeAction;                /** Node action */
    TriggerAction                   _goToFirstNodeAction;       /** Go to first node action */
    TriggerAction                   _goToPreviousNodeAction;    /** Go to previous node action */
    TriggerAction                   _goToNextNodeAction;        /** Go to next node action */
    TriggerAction                   _goToLastNodeAction;        /** Go to last node action */
    TriggerAction                   _removeNodeAction;          /** Remove node action */

    /** Only color map settings action may instantiate this class */
    friend class ColorMapSettingsAction;
};

}
}
