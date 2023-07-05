// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/Actions.h>

using namespace hdps;
using namespace hdps::gui;

class ClustersActionWidget;

/**
 * Select clusters action class
 *
 * Action class for selection clusters (all/none/invert)
 *
 * @author Thomas Kroes
 */
class SelectClustersAction : public WidgetAction
{
protected:

    /** Widget class for select clusters action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param selectClustersAction Pointer to select clusters action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, SelectClustersAction* selectClustersAction, const std::int32_t& widgetFlags);
    };

    /**
     * Get widget representation of the select clusters action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

public:

    /**
     * Constructor
     * @param clustersActionWidget Pointer to clusters action widget
     */
    SelectClustersAction(ClustersActionWidget* clustersActionWidget);

public: // Action getters

    TriggerAction& getSelectAllAction() { return _selectAllAction; }
    TriggerAction& getSelectNoneAction() { return _selectNoneAction; }
    TriggerAction& getSelectInvertAction() { return _selectInvertAction; }

protected:
    ClustersActionWidget*   _clustersActionWidget;      /** Pointer to clusters action widget */
    TriggerAction           _selectAllAction;           /** Select all clusters action */
    TriggerAction           _selectNoneAction;          /** Clear cluster selection action */
    TriggerAction           _selectInvertAction;        /** Invert the cluster selection action */
};
