#pragma once

#include "actions/Actions.h"
#include "event/EventListener.h"

#include "PointData.h"

#include <QTimer>

namespace hdps {
    class CoreInterface;
}

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
class SelectedIndicesAction : public WidgetAction, public hdps::EventListener
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
     * @param core Pointer to the core
     * @param points Reference to points dataset
     */
    SelectedIndicesAction(QObject* parent, hdps::CoreInterface* core, Points& points);

    /** Get selected indices in the points dataset */
    const std::vector<std::uint32_t>& getSelectedIndices() const;

public: // Action getters

    TriggerAction& getUpdateAction() { return _updateAction; }
    ToggleAction& getManualUpdateAction() { return _manualUpdateAction; }

protected:

    /** Update the selected indices */
    void updateSelectedIndices();

signals:

    /**
     * Signals that the selected indices changed
     * @param selectedIndices Selected indices
     */
    void selectedIndicesChanged(const std::vector<std::uint32_t>& selectedIndices);

protected:
    CoreInterface*              _core;                      /** Pointer to the core */
    Dataset<Points>             _points;                    /** Points dataset reference */
    TriggerAction               _updateAction;              /** Update action */
    ToggleAction                _manualUpdateAction;        /** Manual update action */
    QTimer                      _selectionChangedTimer;     /** Timer to control when selection changes are processed */
    std::vector<std::uint32_t>  _selectedIndices;           /** Selected indices */

    static const std::int32_t MANUAL_UPDATE_THRESHOLD = 10000;
};