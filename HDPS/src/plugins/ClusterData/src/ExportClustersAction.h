#pragma once

#include <actions/TriggerAction.h>

using namespace hdps;
using namespace hdps::gui;

class ClustersAction;
class ClustersFilterModel;
class QItemSelectionModel;

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
     * @param filterModel Reference to clusters filter model
     * @param selectionModel Reference to selection model
     */
    ExportClustersAction(QObject* parent, ClustersAction& clustersAction, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel);

protected:
    ClustersAction&         _clustersAction;        /** Reference to clusters action */
    ClustersFilterModel&    _filterModel;           /** Reference to clusters filter model */
    QItemSelectionModel&    _selectionModel;        /** Reference to selection model */
};
