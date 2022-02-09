#pragma once

#include "WidgetAction.h"
#include "OptionAction.h"
#include "TriggerAction.h"
#include "LoadPresetAction.h"
#include "SavePresetAction.h"

#include "util/PresetsModel.h"
#include "util/PresetsFilterModel.h"

#include <QHBoxLayout>

namespace hdps {

namespace gui {

/**
 * Action options action class
 *
 * Action class for interacting with action presets
 *
 * @author Thomas Kroes
 */
class PresetsAction : public WidgetAction
{
public:

    /** Widget class for presets action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param presetsAction Pointer to presets action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, PresetsAction* presetsAction, const std::int32_t& widgetFlags);

    protected:
        QHBoxLayout     _layout;        /** Widget layout */

        friend class PresetsAction;
    };

protected:

    /**
     * Get widget representation of the presets action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    }

public:

     /**
      * Constructor
      * @param parent Pointer to parent object
      */
    PresetsAction(QObject* parent);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr) override final;

    /** Update action states */
    void updateActions();

    /**
     * Set widget action
     * @param widgetAction Pointer to widget action to retrieve the presets for
     */
    void setWidgetAction(hdps::gui::WidgetAction* widgetAction);

    /**
     * Get presets filter model
     * @return Reference to presets filter model
     */
    util::PresetsFilterModel& getPresetsFilterModel();

public: // Action getters

    LoadPresetAction& getLoadPresetAction() { return _loadPresetAction; }
    SavePresetAction& getSavePresetAction() { return _savePresetAction; }
    TriggerAction& getLoadAction() { return _loadAction; }
    TriggerAction& getSaveAction() { return _saveAction; }

protected:
    WidgetAction*               _widgetAction;              /** Pointer to widget actions for the options */
    util::PresetsModel          _presetsModel;              /** Presets model */
    util::PresetsFilterModel    _presetsFilterModel;        /** Presets filter model */
    LoadPresetAction            _loadPresetAction;          /** Load preset action */
    SavePresetAction            _savePresetAction;          /** Save preset action */
    TriggerAction               _loadAction;                /** */
    TriggerAction               _saveAction;                /** */
    bool                        _ignoreResettableSignals;   /** Ignore (factory) resettable signals */
};

}
}
