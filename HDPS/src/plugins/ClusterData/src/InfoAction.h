#pragma once

#include "actions/Actions.h"
#include "event/EventListener.h"
#include "util/DatasetRef.h"

#include "ClusterData.h"
#include "ClustersAction.h"

namespace hdps {
    class CoreInterface;
}

using namespace hdps;
using namespace hdps::gui;
using namespace hdps::util;

/**
 * Info action class
 *
 * Action class for displaying basic clusters info
 *
 * @author Thomas Kroes
 */
class InfoAction : public GroupAction, public hdps::EventListener
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param core Pointer to the core
     * @param datasetName Name of the points dataset
     */
    InfoAction(QObject* parent, CoreInterface* core, const QString& datasetName);

public: // Action getters

    StringAction& getNumberOfClustersAction() { return _numberOfClustersAction; }

protected:
    CoreInterface*          _core;                      /** Pointer to the core */
    DatasetRef<Clusters>    _clusters;                  /** Clusters dataset reference */
    StringAction            _numberOfClustersAction;    /** Number of points action */
    ClustersAction          _clustersAction;            /** Clusters action */
};
