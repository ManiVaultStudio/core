#pragma once

#include <actions/TriggerAction.h>

using namespace hdps;
using namespace hdps::gui;

class ClustersAction;

/**
 * Export clusters action class
 *
 * Action class for exporting clusters from file
 *
 * @author Thomas Kroes
 */
class ExportClustersAction : public TriggerAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param clustersAction Reference to clusters action
     */
    ExportClustersAction(QObject* parent, ClustersAction& clustersAction);

protected:
    ClustersAction&     _clustersAction;        /** Reference to clusters action */
};
