// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/WidgetAction.h>
#include <actions/WidgetActionWidget.h>
#include <actions/StringAction.h>

#include <QCheckBox>
#include <QPushButton>

class DimensionsPickerAction;

namespace mv::gui {

/**
 * Dimensions picker filter action
 *
 * Action class for filtering dimensions
 *
 * @author Thomas Kroes
 */
class DimensionsPickerFilterAction : public WidgetAction
{
public:

    /** Widget class for dimensions picker filter action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param dimensionsPickerFilterAction Pointer to dimensions picker filter action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, DimensionsPickerFilterAction* dimensionsPickerFilterAction, const std::int32_t& widgetFlags);

    protected:

        friend class DimensionsPickerFilterAction;
    };

protected:

    /**
     * Get widget representation of the dimensions picker filter action
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
    DimensionsPickerFilterAction(DimensionsPickerAction& dimensionsPickerAction);

public: // Action getters

    StringAction& getNameFilterAction() { return _nameFilterAction; }

protected:
    DimensionsPickerAction&     _dimensionsPickerAction;    /** Reference to the dimensions picker action */
    StringAction                _nameFilterAction;          /** Name filter action */
};

}
