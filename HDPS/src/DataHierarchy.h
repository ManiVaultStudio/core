#ifndef HDPS_DATAHIERARCHY_H
#define HDPS_DATAHIERARCHY_H

#include <QWidget>
#include <QTreeView>
#include <QMenu>

namespace hdps
{
    class DataManager;
    class DataHierarchyModel;

    namespace gui
    {
        /**
         * Widget displaying all data currently present in the system.
         */
        class DataHierarchy : public QTreeView
        {
            Q_OBJECT
        public:
            DataHierarchy(DataManager& dataManager);
            
        public slots:
            void updateDataModel();
            void itemContextMenu(const QPoint& pos);

            void dataRenamed();

        private:
            /** Stores a reference to the DataManager so it can query about data in the system */
            DataManager& _dataManager;

            /** Model containing data to be displayed in the hierarchy */
            DataHierarchyModel* _model;

            /** Context menu for items containing possible item operations */
            QMenu* _contextMenu;
        };
    }
}

#endif // HDPS_DATAHIERARCHY_H
