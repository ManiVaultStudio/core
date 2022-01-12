#pragma once

#include <actions/TriggerAction.h>

#include <QItemSelectionModel>

using namespace hdps;
using namespace hdps::gui;

class ClustersAction;
class ClustersFilterModel;
class QItemSelectionModel;

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
     * @param clustersAction Reference to clusters action
     * @param filterModel Reference to clusters filter model
     * @param selectionModel Reference to selection model
     */
    MergeClustersAction(ClustersAction& clustersAction, ClustersFilterModel& filterModel, QItemSelectionModel& selectionModel);

protected:
    ClustersAction&         _clustersAction;        /** Reference to clusters action */
    ClustersFilterModel&    _filterModel;           /** Reference to clusters filter model */
    QItemSelectionModel&    _selectionModel;        /** Reference to item selection model */
};