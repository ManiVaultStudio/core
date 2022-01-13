#pragma once

#include <actions/TriggerAction.h>

using namespace hdps;
using namespace hdps::gui;

class ClustersActionWidget;

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
     * @param clustersActionWidget Pointer to clusters action widget
     */
    ImportClustersAction(ClustersActionWidget* clustersActionWidget);

protected:
    ClustersActionWidget*   _clustersActionWidget;      /** Pointer to clusters action widget */
};
