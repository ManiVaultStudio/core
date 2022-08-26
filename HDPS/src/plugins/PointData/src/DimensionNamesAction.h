#pragma once

#include "PointData.h"

#include <actions/TriggerAction.h>
#include <actions/ToggleAction.h>

#include <QTimer>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

/**
 * Dimension names action class
 *
 * Action class for displaying dimension names
 *
 * @author Thomas Kroes
 */
class DimensionNamesAction : public hdps::gui::WidgetAction
{
    Q_OBJECT

protected:

    /** Widget class for dimension names action */
    class Widget : public hdps::gui::WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param dimensionNamesAction Pointer to dimension names action
         */
        Widget(QWidget* parent, DimensionNamesAction* dimensionNamesAction);

    private:
        QTimer  _timer;     /** Timer to sparingly update the number of selected points */
        bool    _dirty;     /** Whether the current selected indices display is dirty or not */

        static const std::int32_t LAZY_UPDATE_INTERVAL = 500;
    };

    /**
     * Get widget representation of the dimension names action
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
    DimensionNamesAction(QObject* parent, const Dataset<Points>& points);

    /**
     * Get points
     * @return Smart pointer to points dataset
     */
    Dataset<Points>& getPoints();

    /** Get the dimension names */
    QStringList getDimensionNames() const;

public: // Action getters

    TriggerAction& getUpdateAction() { return _updateAction; }
    ToggleAction& getManualUpdateAction() { return _manualUpdateAction; }

signals:

    /** Signals that the dimension names changed
     * @param dimensionNames Dimension names
     */
    void dimensionNamesChanged(const QStringList& dimensionNames);

protected:
    Dataset<Points>         _points;                /** Points dataset reference */
    QStringList             _dimensionNames;        /** Dimension names */
    TriggerAction           _updateAction;          /** Update action */
    ToggleAction            _manualUpdateAction;    /** Manual update action */

    /** Above this threshold, dimension names need to be updated manually */
    static const std::int32_t MANUAL_UPDATE_THRESHOLD = 1000;
};
