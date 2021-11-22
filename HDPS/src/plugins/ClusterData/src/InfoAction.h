#pragma once

#include "Dataset.h"
#include "ClusterData.h"
#include "ClustersAction.h"

#include "actions/Actions.h"
#include "event/EventListener.h"

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
     * @param clusters Reference to clusters dataset
     */
    InfoAction(QObject* parent, Clusters& clusters);

public: // Action getters

    StringAction& getNumberOfClustersAction() { return _numberOfClustersAction; }

protected:
    Dataset<Clusters>   _clusters;                  /** Clusters dataset smart pointer */
    StringAction        _numberOfClustersAction;    /** Number of points action */
    ClustersAction      _clustersAction;            /** Clusters action */
};
