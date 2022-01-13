#pragma once

#include <actions/TriggerAction.h>

using namespace hdps;
using namespace hdps::gui;

class ClustersActionWidget;

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
     * @param clustersActionWidget Pointer to clusters action widget
     */
    ExportClustersAction(ClustersActionWidget* clustersActionWidget);

protected:
    ClustersActionWidget*   _clustersActionWidget;      /** Pointer to clusters action widget */
};
