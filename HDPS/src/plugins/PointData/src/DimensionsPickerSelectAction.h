// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/WidgetAction.h>
#include <actions/WidgetActionWidget.h>
#include <actions/IntegralAction.h>
#include <actions/TriggerAction.h>

class DimensionsPickerAction;

namespace hdps::gui {

/**
 * Dimensions picker select action
 *
 * Action class for selecting dimensions based on visibility
 *
 * @author Thomas Kroes
 */
class DimensionsPickerSelectAction : public WidgetAction
{
public:

    /** Widget class for dimensions picker miscellaneous action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param dimensionsPickerSelectAction Pointer to dimensions picker select action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, DimensionsPickerSelectAction* dimensionsPickerSelectAction, const std::int32_t& widgetFlags);

    protected:

        friend class DimensionsPickerSelectAction;
    };

protected:

    /**
     * Get widget representation of the dimensions picker select action
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
    DimensionsPickerSelectAction(DimensionsPickerAction& dimensionsPickerAction);

public: // Action getters

    IntegralAction& getSelectionThresholdAction() { return _selectionThresholdAction; }
    TriggerAction& getComputeStatisticsAction() { return _computeStatisticsAction; }
    TriggerAction& getSelectVisibleAction() { return _selectVisibleAction; }
    TriggerAction& getSelectNonVisibleAction() { return _selectNonVisibleAction; }
    TriggerAction& getLoadSelectionAction() { return _loadSelectionAction; }
    TriggerAction& getSaveSelectionAction() { return _saveSelectionAction; }
    TriggerAction& getLoadExclusionAction() { return _loadExclusionAction; }

protected:
    DimensionsPickerAction&     _dimensionsPickerAction;        /** Reference to the dimensions picker action */
    IntegralAction              _selectionThresholdAction;      /** Selection threshold action */
    TriggerAction               _selectVisibleAction;           /** Select visible dimensions action */
    TriggerAction               _computeStatisticsAction;       /** Compute statistics action */
    TriggerAction               _selectNonVisibleAction;        /** Select non visible dimensions action */
    TriggerAction               _loadSelectionAction;           /** Load selection action */
    TriggerAction               _saveSelectionAction;           /** Save selection action */
    TriggerAction               _loadExclusionAction;           /** Load exclusion action */
    
};

}
