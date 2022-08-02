#pragma once

#include "PointData.h"

#include <actions/StringAction.h>

#include <QTimer>

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

/**
 * Number of selected points action class
 *
 * Action class for viewing the number of selected points
 *
 * @author Thomas Kroes
 */
class NumberOfSelectedPointsAction : public WidgetAction
{
    Q_OBJECT

protected:

    /** Widget class for number of selected points action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param numberOfSelectedPointsAction Pointer to number of selected points action
         */
        Widget(QWidget* parent, NumberOfSelectedPointsAction* numberOfSelectedPointsAction);

    private:
        QTimer  _timer;     /** Timer to sparingly update the number of selected points */

        static const std::int32_t LAZY_UPDATE_INTERVAL = 250;
    };

    /**
     * Get widget representation of the number of selected points action
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
    NumberOfSelectedPointsAction(QObject* parent, const Dataset<Points>& points);

    /**
     * Get points
     * @return Smart pointer to points dataset
     */
    Dataset<Points>& getPoints();

public: // Action getters

    StringAction& getNumberOfSelectedPointsAction() { return _numberOfSelectedPointsAction; }

protected:
    Dataset<Points>     _points;                            /** Points dataset reference */
    StringAction        _numberOfSelectedPointsAction;      /** Number of selected points action */
};
