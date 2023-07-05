// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/WidgetAction.h>
#include <actions/WidgetActionWidget.h>
#include <actions/ToggleAction.h>

class DimensionsPickerAction;

namespace hdps::gui {

/**
 * Dimensions picker miscellaneous widget action
 *
 * Action class for managing dimension picker miscellaneous settings
 *
 * @author Thomas Kroes
 */
class DimensionsPickerMiscellaneousAction : public WidgetAction
{
public:

    /** Widget class for dimensions picker miscellaneous action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param dimensionsPickerMiscellaneousAction Pointer to dimensions picker miscellaneous action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, DimensionsPickerMiscellaneousAction* dimensionsPickerMiscellaneousAction, const std::int32_t& widgetFlags);

    protected:

        friend class DimensionsPickerMiscellaneousAction;
    };

protected:

    /**
     * Get widget representation of the dimensions picker miscellaneous action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override
    {
        return new Widget(parent, this, widgetFlags);
    }

public:

    /**
     * Constructor
     * @param dimensionsPickerAction Reference to the dimensions picker action
     */
    DimensionsPickerMiscellaneousAction(DimensionsPickerAction& dimensionsPickerAction);

public: // Action getters

    ToggleAction& getShowOnlySelectedDimensionsAction() { return _showOnlySelectedDimensionsAction; }
    ToggleAction& getApplyExclusionListAction() { return _applyExclusionListAction; }
    ToggleAction& getIgnoreZeroValuesAction() { return _ignoreZeroValuesAction; }

protected:
    DimensionsPickerAction&     _dimensionsPickerAction;                /** Reference to the dimensions picker action */
    ToggleAction                _showOnlySelectedDimensionsAction;      /** Show only selected dimensions action */
    ToggleAction                _applyExclusionListAction;              /** Apply exclusion list action */
    ToggleAction                _ignoreZeroValuesAction;                /** Ignore zero values for statistics action */
};

}
