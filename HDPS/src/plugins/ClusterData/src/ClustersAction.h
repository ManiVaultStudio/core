#pragma once

#include "actions/Actions.h"
#include "event/EventListener.h"
#include "util/DatasetRef.h"

#include "ClustersModel.h"
#include "ClustersFilterModel.h"
#include "FilterAndSelectAction.h"
#include "SubsetAction.h"

#include <QItemSelectionModel>

namespace hdps {
    class CoreInterface;
    class DataHierarchyItem;
}

class Cluster;
class Clusters;

using namespace hdps;
using namespace hdps::gui;
using namespace hdps::util;

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
        SubsetAction            _subsetAction;              /** Subset action */
    };

    /**
     * Get widget representation of the color action
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

    /** Create subset from selected clusters */
    void createSubset(const QString& datasetName);

    /**
     * Remove clusters by their unique identifiers
     * @param ids Unique identifiers of the clusters to remove
     */
    void removeClustersById(const QStringList& ids);

    /** Get the clusters model */
    ClustersModel& getClustersModel();

public: // Action getters

    TriggerAction& getImportAction() { return _importAction; }
    TriggerAction& getExportAction() { return _exportAction; }

protected:
    CoreInterface*          _core;              /** Pointer to the core */
    DatasetRef<Clusters>    _clusters;          /** Cluster dataset reference */
    ClustersModel           _clustersModel;     /** Clusters model */
    TriggerAction           _importAction;      /** Import clusters action */
    TriggerAction           _exportAction;      /** Export clusters action */
};
