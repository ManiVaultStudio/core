#pragma once

#include <actions/TriggerAction.h>

using namespace hdps;
using namespace hdps::gui;

class ClustersAction;

/**
 * Import clusters action class
 *
 * Action class for importing clusters from file
 *
 * @author Thomas Kroes
 */
class ImportClustersAction : public TriggerAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param clustersAction Reference to clusters action
     */
    ImportClustersAction(QObject* parent, ClustersAction& clustersAction);

protected:
    ClustersAction&     _clustersAction;        /** Reference to clusters action */
};
