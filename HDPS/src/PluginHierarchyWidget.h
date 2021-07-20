#ifndef HDPS_PLUGIN_HIERARCHY_WIDGET_H
#define HDPS_PLUGIN_HIERARCHY_WIDGET_H

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
class PluginHierarchyWidget : public QTreeView
{
    Q_OBJECT
public:
    PluginHierarchyWidget(Core* core);
            
public slots:
    void itemContextMenu(const QPoint& pos);

    void dataRenamed();

private:
    Core*                   _core;          /** Pointer to core */
    PluginHierarchyModel*   _model;         /** Model containing data to be displayed in the hierarchy */
};

}
}

#endif // HDPS_PLUGIN_HIERARCHY_WIDGET_H
