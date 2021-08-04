#ifndef HDPS_DATA_HIERARCHY_WIDGET_H
#define HDPS_DATA_HIERARCHY_WIDGET_H

#include "DataHierarchyModel.h"

#include <QWidget>
#include <QTreeView>
#include <QMenu>

namespace hdps
{

class Core;

namespace gui
{

/**
* Widget for displaying the data hierarchy
*/
class DataHierarchyWidget : public QTreeView
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param core Pointer to the core
     */
    DataHierarchyWidget(QWidget* parent, Core* core);

signals:

    /**
     * Invoked when the selected data name changed
     * @param datasetName Name of the newly selected dataset
     */
    void selectedDatasetNameChanged(const QString& datasetName);

private:
    Core*                   _core;              /** Pointer to the core */
    DataHierarchyModel      _model;             /** Model containing data to be displayed in the hierarchy */
    QItemSelectionModel     _selectionModel;    /** Selection model */
};

}
}

#endif // HDPS_DATA_HIERARCHY_WIDGET_H
