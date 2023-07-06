// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PointData.h"

#include <actions/TriggerAction.h>
#include <actions/StringAction.h>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

/**
 * Create set from selection action class
 *
 * Action class for creating a set from a selection
 *
 * @author Thomas Kroes
 */
class CreateSetFromSelectionAction : public WidgetAction
{
    Q_OBJECT

protected:

    /** Widget class for proxy datasets action */
    class Widget : public hdps::gui::WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param createSetFromSelectionAction Pointer to create set from selection action
         */
        Widget(QWidget* parent, CreateSetFromSelectionAction* createSetFromSelectionAction);
    };

    /**
     * Get widget representation of the create set from selection action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param points Smart pointer to points dataset
     */
    CreateSetFromSelectionAction(QObject* parent, const Dataset<Points>& points);

public: // Action getters

    StringAction& getNameAction() { return _nameAction; }
    TriggerAction& getCreateAction() { return _createAction; }

signals:

    /** Signals that the dimension names changed
     * @param dimensionNames Dimension names
     */
    void dimensionNamesChanged(const QStringList& dimensionNames);

protected:
    Dataset<Points>     _points;            /** Points dataset reference */
    StringAction        _nameAction;        /** Set name action */
    TriggerAction       _createAction;      /** Create set action */
};
