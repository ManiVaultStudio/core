#pragma once

#include "actions/Actions.h"

using namespace hdps;
using namespace hdps::gui;

class ClustersFilterModel;
class QItemSelectionModel;

/**
 * Filter and select action class
 *
 * Action class for filtering and selecting clusters
 *
 * @author Thomas Kroes
 */
class FilterAndSelectAction : public hdps::gui::WidgetAction
{
protected:

    /** Widget class for filtering and selection of clusters action */
    class Widget : public hdps::gui::WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param filterAndSelectAction Pointer to filter and select action
         * @param state State of the widget
         */
        Widget(QWidget* parent, FilterAndSelectAction* filterAndSelectAction, const WidgetActionWidget::State& state);
    };

    /**
     * Get widget representation of the filter and select action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     * @param state State of the widget (for stateful widgets)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param filterModel Reference to clusters filter model
     * @param selectionModel Reference to selection model
     */
    FilterAndSelectAction(QObject* parent, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel);

public: // Action getters

    StringAction& getNameFilterAction() { return _nameFilterAction; }
    TriggerAction& getSelectAllAction() { return _selectAllAction; }
    TriggerAction& getSelectNoneAction() { return _selectNoneAction; }
    TriggerAction& getSelectInvertAction() { return _selectInvertAction; }

protected:
    ClustersFilterModel&    _filterModel;           /** Reference to clusters filter model */
    QItemSelectionModel&    _selectionModel;        /** Reference to selection model */
    StringAction            _nameFilterAction;      /** Name filter action */
    TriggerAction           _selectAllAction;       /** Select all clusters action */
    TriggerAction           _selectNoneAction;      /** Clear cluster selection action */
    TriggerAction           _selectInvertAction;    /** Invert the cluster selection action */
};
