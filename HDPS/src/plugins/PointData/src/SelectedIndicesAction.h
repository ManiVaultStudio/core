#pragma once

#include "actions/Actions.h"

#include "PointData.h"

#include <actions/TriggerAction.h>
#include <actions/ToggleAction.h>

#include <QTimer>

using namespace hdps;
using namespace hdps::gui;
using namespace hdps::util;

/**
 * Selected indices action class
 *
 * Action class for selected point indices
 *
 * @author Thomas Kroes
 */
class SelectedIndicesAction : public WidgetAction
{
    Q_OBJECT

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

    private:
        QTimer  _timer;     /** Timer to sparingly update the number of selected points */
        bool    _dirty;     /** Whether the current selected indices display is dirty or not */

        static const std::int32_t LAZY_UPDATE_INTERVAL = 250;
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

    static const std::int32_t MANUAL_UPDATE_THRESHOLD = 10000;
};