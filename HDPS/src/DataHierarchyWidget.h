#ifndef HDPS_DATA_HIERARCHY_WIDGET_H
#define HDPS_DATA_HIERARCHY_WIDGET_H

#include "DataHierarchyModel.h"

#include <QWidget>
#include <QTreeView>
#include <QMenu>

namespace hdps
{

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
     */
    DataHierarchyWidget(QWidget* parent);

signals:

    /**
     * Invoked when the selected data name changed
     * @param datasetName Name of the newly selected dataset
     */
    void selectedDatasetNameChanged(const QString& datasetName);

private:
    DataHierarchyModel      _model;             /** Model containing data to be displayed in the hierarchy */
    QItemSelectionModel     _selectionModel;    /** Selection model */
};

}
}

#endif // HDPS_DATA_HIERARCHY_WIDGET_H
