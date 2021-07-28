#ifndef HDPS_DATA_HIERARCHY_WIDGET_H
#define HDPS_DATA_HIERARCHY_WIDGET_H

#include <QWidget>
#include <QTreeView>
#include <QMenu>

namespace hdps
{

class Core;
class PluginHierarchyModel;

namespace gui
{

/**
* Widget displaying all data currently present in the system.
*/
class DataHierarchyWidget : public QTreeView
{
    Q_OBJECT
public:
    DataHierarchyWidget(QWidget* parent, Core* core);
            
public slots:
    void itemContextMenu(const QPoint& pos);

signals:

    /**
     * Invoked when the selected data name changed
     * @param datasetName Name of the newly selected dataset
     */
    void selectedDatasetNameChanged(const QString& datasetName);

private:
    Core*                   _core;              /** Pointer to core */
    PluginHierarchyModel*   _model;             /** Model containing data to be displayed in the hierarchy */
    QItemSelectionModel*    _selectionModel;    /** Selection model */
};

}
}

#endif // HDPS_DATA_HIERARCHY_WIDGET_H
