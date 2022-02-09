#pragma once

#include "WidgetAction.h"
#include "OptionAction.h"

#include <QHBoxLayout>

namespace hdps {

namespace gui {

class PresetsAction;

/**
 * Load preset action class
 *
 * Action class for loading a preset
 *
 * @author Thomas Kroes
 */
class LoadPresetAction : public WidgetAction
{
public:

    /** Widget class for load preset action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param loadPresetAction Pointer to load preset action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, LoadPresetAction* loadPresetAction, const std::int32_t& widgetFlags);

    protected:
        QHBoxLayout     _layout;        /** Widget layout */

        friend class LoadPresetAction;
    };

protected:

    /**
     * Get widget representation of the load preset action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    }

public:

     /**
      * Constructor
      * @param presetsAction Pointer to parent presets action
      */
    LoadPresetAction(PresetsAction* presetsAction);

public: // Action getters

    OptionAction& getSelectionAction() { return _selectionAction; }

protected:
    PresetsAction*      _presetsAction;     /** Pointer to parent presets action */
    OptionAction        _selectionAction;   /** Selection action */
};

}
}
