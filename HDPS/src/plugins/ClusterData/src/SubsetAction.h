// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/StringAction.h"
#include "actions/TriggerAction.h"

using namespace hdps;
using namespace hdps::gui;

class ClustersActionWidget;

/**
 * Subset action class
 *
 * Action class for creating subsets based on selected clusters
 *
 * @author Thomas Kroes
 */
class SubsetAction : public hdps::gui::WidgetAction
{
protected:

    /** Widget class for creating subsets based on selected clusters */
    class Widget : public hdps::gui::WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param subsetAction Pointer to subset action
         */
        Widget(QWidget* parent, SubsetAction* subsetAction);
    };

    /**
     * Get widget representation of the subset action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:

    /**
     * Constructor
     * @param clustersActionWidget Pointer to clusters action widget
     */
    SubsetAction(ClustersActionWidget* clustersActionWidget);

public: // Action getters

    StringAction& getSubsetNameAction() { return _subsetNameAction; }
    TriggerAction& getCreateSubsetAction() { return _createSubsetAction; }

protected:
    ClustersActionWidget*   _clustersActionWidget;      /** Pointer to clusters action widget */
    StringAction            _subsetNameAction;          /** Subset name action */
    TriggerAction           _createSubsetAction;        /** Create subset action */
};
