#pragma once

#include "actions/Actions.h"
#include "util/DatasetRef.h"

using namespace hdps;
using namespace hdps::gui;

class ClustersFilterModel;
class QItemSelectionModel;

class ClustersAction;

/**
 * Subset action class
 *
 * Action class for creating subsets based on selected clusters
 *
 * @author Thomas Kroes
 */
class SubsetAction : public hdps::gui::WidgetAction
{
protected:

    /** Widget class for creating subsets based on selected clusters */
    class Widget : public hdps::gui::WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param subsetAction Pointer to subset action
         * @param state State of the widget
         */
        Widget(QWidget* parent, SubsetAction* subsetAction, const WidgetActionWidget::State& state);
    };

    /**
     * Get widget representation of the clusters action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param clustersAction Reference to clusters action
     * @param filterModel Reference to clusters filter model
     * @param selectionModel Reference to selection model
     */
    SubsetAction(QObject* parent, ClustersAction& clustersAction, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel);

public: // Action getters

    StringAction& getSubsetNameAction() { return _subsetNameAction; }
    TriggerAction& getCreateSubsetAction() { return _createSubsetAction; }

protected:
    ClustersAction&         _clustersAction;        /** Reference to clusters action */
    ClustersFilterModel&    _filterModel;           /** Reference to clusters filter model */
    QItemSelectionModel&    _selectionModel;        /** Reference to selection model */
    StringAction            _subsetNameAction;      /** Subset name action */
    TriggerAction           _createSubsetAction;    /** Create subset action */
};
