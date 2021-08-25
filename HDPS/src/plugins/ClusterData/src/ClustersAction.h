#pragma once

#include "actions/Actions.h"
#include "event/EventListener.h"

namespace hdps {
    class CoreInterface;
    class DataHierarchyItem;
}

struct Cluster;

using namespace hdps;
using namespace hdps::gui;

/**
 * Clusters action class
 *
 * Action class for display and interaction with clusters
 *
 * @author Thomas Kroes
 */
class ClustersAction : public hdps::gui::WidgetAction, public EventListener
{
    Q_OBJECT

protected:

    /** Widget class for clusters action */
    class Widget : public hdps::gui::WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param clustersAction Pointer to clusters action
         * @param state State of the widget
         */
        Widget(QWidget* parent, ClustersAction* clustersAction, const hdps::gui::WidgetActionWidget::State& state);

    protected:
        ColorAction     _colorAction;       /** Color action */
        TriggerAction   _removeAction;      /** Remove clusters action */
    };

    /**
     * Get widget representation of the clusters action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const hdps::gui::WidgetActionWidget::State& state = hdps::gui::WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param core Pointer to the core
     * @param datasetName Name of the points dataset
     */
    ClustersAction(QObject* parent, hdps::CoreInterface* core, const QString& datasetName);

    /** Get clusters */
    std::vector<Cluster>* getClusters();

    /**
     * Select points
     * @param indices Point indices to select
     */
    void selectPoints(const std::vector<std::uint32_t>& indices);

    /**
     * Remove cluster
     * @param clusters Clusters to remove
     */
    void removeClusters(const QVector<Cluster*>& clusters);

signals:

    /**
     * Signals that the clusters changed
     * @param clusters New clusters
     */
    void clustersChanged(std::vector<Cluster>* clusters);

protected:
    hdps::CoreInterface*        _core;                  /** Pointer to the core */
    hdps::DataHierarchyItem*    _dataHierarchyItem;     /** Pointer to the data hierarchy item of the points dataset */
};