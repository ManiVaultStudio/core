#pragma once

#include <actions/TriggerAction.h>

#include <QItemSelectionModel>

using namespace hdps;
using namespace hdps::gui;

class ClustersActionWidget;

/**
 * Merge clusters action class
 *
 * Action class for merging selected clusters in a clusters model
 *
 * @author Thomas Kroes
 */
class MergeClustersAction : public TriggerAction
{
public:

    /**
     * Constructor
     * @param clustersActionWidget Pointer to clusters action widget
     */
    MergeClustersAction(ClustersActionWidget* clustersActionWidget);

protected:
    ClustersActionWidget*   _clustersActionWidget;      /** Pointer to clusters action widget */
};