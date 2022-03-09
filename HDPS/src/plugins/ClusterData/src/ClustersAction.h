#pragma once

#include "clusterdata_export.h"

#include "ClustersModel.h"
#include "ClustersFilterModel.h"
#include "ClustersActionWidget.h"
#include "ColorizeClustersAction.h"
#include "PrefixClustersAction.h"

#include <actions/Actions.h>

#include <QItemSelectionModel>

class Cluster;
class Clusters;

using namespace hdps::gui;
using namespace hdps::util;

/**
 * Clusters action class
 *
 * Action class for display of and interaction with clusters
 *
 * @author Thomas Kroes
 */
class CLUSTERDATA_EXPORT ClustersAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        Remove      = 0x00001,      /** Includes remove clusters user interface */
        Merge       = 0x00002,      /** Includes merge clusters user interface */
        Filter      = 0x00004,      /** Includes filter clusters user interface */
        Select      = 0x00008,      /** Includes select clusters user interface */
        Colorize    = 0x00010,      /** Includes colorize clusters user interface */
        Prefix      = 0x00020,      /** Includes prefix clusters user interface */
        Subset      = 0x00040,      /** Includes subset user interface */
        Refresh     = 0x00080,      /** Includes refresh clusters user interface */

        Default = Remove | Merge | Filter | Select | Colorize | Prefix | Subset
    };

protected:

    /**
     * Get widget representation of the color action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new ClustersActionWidget(parent, this, widgetFlags);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param clustersDataset Smart pointer to clusters
     */
    ClustersAction(QObject* parent, Dataset<Clusters> clustersDataset = Dataset<Clusters>());

    /**
     * Get clusters
     * @return Pointer to vector of clusters
     */
    QVector<Cluster>* getClusters();

    /**
     * Get clusters dataset
     * @return Smart pointer to clusters dataset
     */
    Dataset<Clusters>& getClustersDataset();

    /**
     * Set clusters dataset
     * @param clustersDataset Smart pointer to clusters dataset
     */
    void setClustersDataset(Dataset<Clusters> clustersDataset);

    /**
     * Create subset from selected clusters
     * @param datasetName Name of the subset
     */
    void createSubsetFromSelection(const QString& datasetName);

    /**
     * Remove clusters by their unique identifiers
     * @param ids Unique identifiers of the clusters to remove
     */
    void removeClustersById(const QStringList& ids);

    /**
     * Get the clusters model
     * @param Reference to the clusters model
     */
    ClustersModel& getClustersModel();

    /** Invalidates the clusters and requests an (external) refresh */
    void invalidateClusters();

protected:

    /** Update clusters model from clusters dataset */
    void updateClustersModel();

    /** Update clusters dataset from clusters model */
    void updateClustersDataset();

public: // Action getters

    ColorizeClustersAction& getColorizeClustersAction() { return _colorizeClustersAction; }
    PrefixClustersAction& getPrefixClustersAction() { return _prefixClustersAction; }

signals:

    /** Signals that a refresh is required */
    void refreshClusters();

protected:
    Dataset<Clusters>           _clustersDataset;           /** Smart pointer to the clusters dataset */
    ClustersModel               _clustersModel;             /** Clusters model */
    ColorizeClustersAction      _colorizeClustersAction;    /** Colorize clusters action */
    PrefixClustersAction        _prefixClustersAction;      /** Prefix clusters action */
};
