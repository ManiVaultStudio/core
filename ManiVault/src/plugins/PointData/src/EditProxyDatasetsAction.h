// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PointData.h"

/**
 * Edit proxy datasets action class
 *
 * Action class for editing proxy datasets
 *
 * @author Thomas Kroes
 */
class EditProxyDatasetsAction : public mv::gui::WidgetAction
{
    Q_OBJECT

protected:

    /** Widget class for edit proxy datasets action */
    class Widget : public mv::gui::WidgetActionWidget {
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
    EditProxyDatasetsAction(QObject* parent, const mv::Dataset<Points>& points);

    /**
     * Get points
     * @return Smart pointer to points dataset
     */
    mv::Dataset<Points>& getPoints();

public: // Action getters

protected:
    mv::Dataset<Points>     _points;        /** Points dataset reference */
};
