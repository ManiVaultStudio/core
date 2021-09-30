#pragma once

#include "actions/Actions.h"
#include "event/EventListener.h"
#include "util/DatasetRef.h"

#include "PointData.h"

namespace hdps {
    class CoreInterface;
}

using namespace hdps::util;
using namespace hdps::gui;

/**
 * Dimension names action class
 *
 * Action class for displaying dimension names
 *
 * @author Thomas Kroes
 */
class DimensionNamesAction : public hdps::gui::WidgetAction, public hdps::EventListener
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
         * @param state State of the widget
         */
        Widget(QWidget* parent, DimensionNamesAction* dimensionNamesAction, const hdps::gui::WidgetActionWidget::State& state);
    };

    /**
     * Get widget representation of the dimension names action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     * @param state State of the widget (for stateful widgets)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags, const hdps::gui::WidgetActionWidget::State& state = hdps::gui::WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param core Pointer to the core
     * @param datasetName Name of the points dataset
     */
    DimensionNamesAction(QObject* parent, hdps::CoreInterface* core, const QString& datasetName);

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
    DatasetRef<Points>      _points;                /** Points dataset reference */
    QStringList             _dimensionNames;        /** Dimension names */
    TriggerAction           _updateAction;          /** Update action */
    ToggleAction            _manualUpdateAction;    /** Manual update action */
};
