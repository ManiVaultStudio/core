// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/StringAction.h>
#include <actions/TriggerAction.h>

class ClustersAction;

/**
 * Prefix clusters action class
 *
 * Action class for prefixing clusters
 *
 * @author Thomas Kroes
 */
class PrefixClustersAction : public mv::gui::TriggerAction
{
protected:

    /** Widget class for prefix clusters action */
    class Widget : public mv::gui::WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param prefixClustersAction Pointer to prefix clusters action
         */
        Widget(QWidget* parent, PrefixClustersAction* prefixClustersAction);
    };

    /**
     * Get widget representation of the prefix clusters action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:

    /**
     * Constructor
     * @param clustersAction Reference to clusters action
     */
    PrefixClustersAction(ClustersAction& clustersAction);

public: // Action getters

    mv::gui::StringAction& getPrefixAction() { return _prefixAction; }
    mv::gui::TriggerAction& getApplyAction() { return _applyAction; }

protected:
    ClustersAction&             _clustersAction;        /** Reference to clusters action */
    mv::gui::StringAction       _prefixAction;          /** Cluster name prefix action */
    mv::gui::TriggerAction      _applyAction;           /** Apply cluster name prefixes action */
};
