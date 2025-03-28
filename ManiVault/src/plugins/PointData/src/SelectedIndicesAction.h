// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PointData.h"

#include <actions/TriggerAction.h>
#include <actions/ToggleAction.h>

#include <QTimer>
#include <QListView>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

/**
 * Selected indices action class
 *
 * Action class for selected point indices
 *
 * @author Thomas Kroes
 */
class SelectedIndicesAction : public WidgetAction
{
protected:

    /** Widget class for points info action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param selectedIndicesAction Pointer to selected indices action
         */
        Widget(QWidget* parent, SelectedIndicesAction* selectedIndicesAction);

    protected:

        /** Inherit list view to override size hint method */
        class ListView : public QListView
        {
            using QListView::QListView;

        public:

            /** Return smallest possible size hint to avoid layout problems */
            QSize sizeHint() const override { return QSize(0, 0); }
        };

    private:
        QTimer  _timer;     /** Timer to sparingly update the number of selected points */

        static const std::int32_t LAZY_UPDATE_INTERVAL = 500;
    };

    /**
     * Get widget representation of the selected indices action
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
    SelectedIndicesAction(QObject* parent, const Dataset<Points>& points);

    /**
     * Get points
     * @return Smart pointer to points dataset
     */
    Dataset<Points>& getPoints();

    /**
     * Get selected indices
     * @return Selected indices
     */
    std::vector<std::uint32_t> getSelectedIndices() const;

public: // Action getters

    TriggerAction& getUpdateAction() { return _updateAction; }
    ToggleAction& getManualUpdateAction() { return _manualUpdateAction; }

protected:
    Dataset<Points>     _points;                    /** Points dataset reference */
    TriggerAction       _updateAction;              /** Update action */
    ToggleAction        _manualUpdateAction;        /** Manual update action */

    /** Above this threshold, selected indices need to be updated manually */
    static const std::int32_t MANUAL_UPDATE_THRESHOLD = 1000;
};