#pragma once

#include "WidgetAction.h"
#include "StringAction.h"
#include "TriggerAction.h"

#include <QHBoxLayout>
#include <QCompleter>

namespace hdps {

namespace gui {

class PresetsAction;

/**
 * Save preset action class
 *
 * Action class for saving a preset
 *
 * @author Thomas Kroes
 */
class SavePresetAction : public WidgetAction
{
public:

    /** Widget class for save preset action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param savePresetAction Pointer to save preset action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, SavePresetAction* savePresetAction, const std::int32_t& widgetFlags);

    protected:
        QHBoxLayout     _layout;        /** Widget layout */

        friend class SavePresetAction;
    };

protected:

    /**
     * Get widget representation of the save preset action
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
    SavePresetAction(PresetsAction* presetsAction);

public: // Action getters

    StringAction& getPresetNameAction() { return _presetName; }
    TriggerAction& getSaveAction() { return _saveAction; }

protected:
    PresetsAction*      _presetsAction;     /** Pointer to parent presets action */
    StringAction        _presetName;        /** Preset name action */
    QCompleter          _completer;         /** Preset name completer */
    TriggerAction       _saveAction;        /** Save action */
};

}
}
