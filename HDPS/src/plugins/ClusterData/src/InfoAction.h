#pragma once

#include "actions/Actions.h"
#include "event/EventListener.h"

namespace hdps {
    class CoreInterface;
    class DataHierarchyItem;
}

using namespace hdps;
using namespace hdps::gui;

/**
 * Info action class
 *
 * Action class for displaying basic points info
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
    DataHierarchyItem*      _dataHierarchyItem;         /** Pointer to the data hierarchy item of the points dataset */
    StringAction            _numberOfClustersAction;    /** Number of points action */
};