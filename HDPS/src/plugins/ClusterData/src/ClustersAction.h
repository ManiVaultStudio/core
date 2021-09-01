#pragma once

#include "actions/Actions.h"
#include "event/EventListener.h"

#include "ClustersModel.h"
#include "ClustersFilterModel.h"
#include "FilterAndSelectAction.h"

#include <QItemSelectionModel>

namespace hdps {
    class CoreInterface;
    class DataHierarchyItem;
}

class Cluster;

using namespace hdps;
using namespace hdps::gui;

/**
 * Clusters action class
 *
 * Action class for display and interaction with clusters
 *
 * @author Thomas Kroes
 */
class ClustersAction : public hdps::gui::WidgetAction, public EventListener
{
protected:

    /** Widget class for clusters action */
    class Widget : public hdps::gui::WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param clustersAction Pointer to clusters action
         * @param state State of the widget
         */
        Widget(QWidget* parent, ClustersAction* clustersAction, const WidgetActionWidget::State& state);

    protected:
        ClustersFilterModel     _filterModel;               /** Clusters filter model */
        QItemSelectionModel     _selectionModel;            /** Clusters selection model */
        TriggerAction           _removeAction;              /** Remove clusters action */
        TriggerAction           _mergeAction;               /** Merge clusters action */
        FilterAndSelectAction   _filterAndSelectAction;     /** Filter and select clusters action */
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
     * @param core Pointer to the core
     * @param datasetName Name of the points dataset
     */
    ClustersAction(QObject* parent, hdps::CoreInterface* core, const QString& datasetName);

    /** Get clusters */
    std::vector<Cluster>* getClusters();

    /**
     * Select points
     * @param indices Point indices to select
     */
    void selectPoints(const std::vector<std::uint32_t>& indices);

    /**
     * Remove clusters by their unique identifiers
     * @param ids Unique identifiers of the clusters to remove
     */
    void removeClustersById(const QStringList& ids);

    /** Get the clusters model */
    ClustersModel& getClustersModel();

protected:
    hdps::CoreInterface*        _core;                  /** Pointer to the core */
    hdps::DataHierarchyItem*    _dataHierarchyItem;     /** Pointer to the data hierarchy item of the points dataset */
    ClustersModel               _clustersModel;         /** Clusters model */
};
