#pragma once

#include <actions/TriggerAction.h>

#include <QItemSelectionModel>

using namespace hdps;
using namespace hdps::gui;

class ClustersAction;
class ClustersFilterModel;
class QItemSelectionModel;

/**
 * Remove clusters action class
 *
 * Action class for removing selected clusters from a clusters model
 *
 * @author Thomas Kroes
 */
class RemoveClustersAction : public TriggerAction
{
public:

    /**
     * Constructor
     * @param clustersAction Reference to clusters action
     * @param filterModel Reference to clusters filter model
     * @param selectionModel Reference to selection model
     */
    RemoveClustersAction(ClustersAction& clustersAction, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel);

protected:
    ClustersAction&         _clustersAction;        /** Reference to clusters action */
    ClustersFilterModel&    _filterModel;           /** Reference to clusters filter model */
    QItemSelectionModel&    _selectionModel;        /** Reference to item selection model */
};
