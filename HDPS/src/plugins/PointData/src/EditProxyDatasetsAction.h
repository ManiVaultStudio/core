// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PointData.h"

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

/**
 * Edit proxy datasets action class
 *
 * Action class for editing proxy datasets
 *
 * @author Thomas Kroes
 */
class EditProxyDatasetsAction : public WidgetAction
{
    Q_OBJECT

protected:

    /** Widget class for edit proxy datasets action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param editProxyDatasetsAction Pointer to edit proxy datasets action
         */
        Widget(QWidget* parent, EditProxyDatasetsAction* editProxyDatasetsAction);
    };

    /**
     * Get widget representation of the edit proxy datasets action
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
    EditProxyDatasetsAction(QObject* parent, const Dataset<Points>& points);

    /**
     * Get points
     * @return Smart pointer to points dataset
     */
    Dataset<Points>& getPoints();

public: // Action getters

protected:
    Dataset<Points>     _points;        /** Points dataset reference */
};
