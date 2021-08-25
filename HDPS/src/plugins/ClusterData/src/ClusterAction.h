#pragma once

#include "actions/Actions.h"
#include "event/EventListener.h"

#include <QTimer>

namespace hdps {
    class CoreInterface;
    class DataHierarchyItem;
}

using namespace hdps;
using namespace hdps::gui;

/**
 * Cluster action class
 *
 * Action class for clustering points
 *
 * @author Thomas Kroes
 */
class ClusterAction : public hdps::gui::WidgetAction, public hdps::EventListener
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param core Pointer to the core
     * @param datasetName Name of the points dataset
     */
    ClusterAction(QObject* parent, hdps::CoreInterface* core, const QString& datasetName);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr) override;;

    /** Get selected indices in the points dataset */
    const std::vector<std::uint32_t>& getSelectedIndices() const;

public: // Action getters

    TriggerAction& getCreateCluster() { return _editAndCreateClusterAction; }

signals:

    /**
     * Signals that the selected indices changed
     * @param selectedIndices Selected indices
     */
    void selectedIndicesChanged(const std::vector<std::uint32_t>& selectedIndices);

protected:
    CoreInterface*          _core;                              /** Pointer to the core */
    DataHierarchyItem*      _inputDataHierarchyItem;            /** Pointer to the input data hierarchy item */
    DataHierarchyItem*      _clusterDataHierarchyItem;          /** Pointer to the output cluster data hierarchy item */
    TriggerAction           _editAndCreateClusterAction;        /** Edit and create cluster action */
};
