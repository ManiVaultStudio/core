#pragma once

#include "actions/Actions.h"
#include "event/EventListener.h"

#include <QTimer>

namespace hdps {
    class CoreInterface;
    class DataHierarchyItem;
}

/**
 * Selected indices action class
 *
 * Action class for selected point indices
 *
 * @author Thomas Kroes
 */
class SelectedIndicesAction : public hdps::gui::WidgetAction, public hdps::EventListener
{
    Q_OBJECT

protected:

    /** Widget class for points info action */
    class Widget : public hdps::gui::WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param selectedIndicesAction Pointer to selected indices action
         * @param state State of the widget
         */
        Widget(QWidget* parent, SelectedIndicesAction* selectedIndicesAction, const hdps::gui::WidgetActionWidget::State& state);
    };

    /**
     * Get widget representation of the selected indices action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const hdps::gui::WidgetActionWidget::State& state = hdps::gui::WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param core Pointer to the core
     * @param datasetName Name of the points dataset
     */
    SelectedIndicesAction(QObject* parent, hdps::CoreInterface* core, const QString& datasetName);

    /** Get selected indices in the points dataset */
    const std::vector<std::uint32_t>& getSelectedIndices() const;

public: // Action getters

    hdps::gui::TriggerAction& getUpdateAction() { return _updateAction; }
    hdps::gui::ToggleAction& getManualUpdateAction() { return _manualUpdateAction; }

signals:

    /**
     * Signals that the selected indices changed
     * @param selectedIndices Selected indices
     */
    void selectedIndicesChanged(const std::vector<std::uint32_t>& selectedIndices);

protected:
    hdps::CoreInterface*        _core;                      /** Pointer to the core */
    hdps::DataHierarchyItem*    _dataHierarchyItem;         /** Pointer to the data hierarchy item of the points dataset */
    hdps::gui::TriggerAction    _updateAction;              /** Update action */
    hdps::gui::ToggleAction     _manualUpdateAction;        /** Manual update action */
    QTimer                      _selectionChangedTimer;     /** Timer to control when selection changes are processed */
};