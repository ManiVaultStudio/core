#pragma once

#include "clusterdata_export.h"

#include <actions/Actions.h>

#include "ClustersModel.h"
#include "ClustersFilterModel.h"
#include "RemoveClustersAction.h"
#include "MergeClustersAction.h"
#include "FilterClustersAction.h"
#include "SelectClustersAction.h"
#include "SubsetAction.h"
#include "ImportClustersAction.h"
#include "ExportClustersAction.h"

#include <QItemSelectionModel>

class Cluster;
class Clusters;

using namespace hdps::gui;
using namespace hdps::util;

/**
 * Clusters action class
 *
 * Action class for display and interaction with clusters
 *
 * @author Thomas Kroes
 */
class CLUSTERDATA_EXPORT ClustersAction : public WidgetAction
{
public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        Remove      = 0x00001,      /** Includes remove clusters user interface */
        Merge       = 0x00002,      /** Includes merge clusters user interface */
        Filter      = 0x00004,      /** Includes filter clusters user interface */
        Select      = 0x00008,      /** Includes select clusters user interface */
        Subset      = 0x00010,      /** Includes create subset user interface */
        Import      = 0x00020,      /** Includes import user interface */
        Export      = 0x00040,      /** Includes export user interface */

        Default = Remove | Merge | Filter | Select | Subset | Import | Export
    };

protected:

    /** Widget class for clusters action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param clustersAction Pointer to clusters action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, ClustersAction* clustersAction, const std::int32_t& widgetFlags);

    protected:
        ClustersFilterModel     _filterModel;               /** Clusters filter model */
        QItemSelectionModel     _selectionModel;            /** Clusters selection model */
        RemoveClustersAction    _removeClustersAction;      /** Remove clusters action */
        MergeClustersAction     _mergeClustersAction;       /** Merge clusters action */
        FilterClustersAction    _filterClustersAction;      /** Filter clusters action */
        SelectClustersAction    _selectClustersAction;      /** Select clusters action */
        SubsetAction            _subsetAction;              /** Subset action */
        ImportClustersAction    _importClustersAction;      /** Import clusters action */
        ExportClustersAction    _exportClustersAction;      /** Export clusters action */
    };

    /**
     * Get widget representation of the color action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param clusters Smart pointer to clusters
     */
    ClustersAction(QObject* parent, Dataset<Clusters> clusters);

    /** Get clusters */
    std::vector<Cluster>* getClusters();

    /** Get clusters */
    Dataset<Clusters>& getClustersDataset();

    /** Create subset from selected clusters */
    void createSubset(const QString& datasetName);

    /**
     * Remove clusters by their unique identifiers
     * @param ids Unique identifiers of the clusters to remove
     */
    void removeClustersById(const QStringList& ids);

    /** Get the clusters model */
    ClustersModel& getClustersModel();

protected:
    Dataset<Clusters>   _clusters;          /** Smart pointer to clusters */
    ClustersModel       _clustersModel;     /** Clusters model */
};
