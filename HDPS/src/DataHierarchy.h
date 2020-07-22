#ifndef HDPS_DATAHIERARCHY_H
#define HDPS_DATAHIERARCHY_H

#include <QWidget>
#include <QTreeView>

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

        private:
            /** Stores a reference to the DataManager so it can query about data in the system */
            DataManager& _dataManager;

            /** Model containing data to be displayed in the hierarchy */
            DataHierarchyModel* _model;
        };
    }
}

#endif // HDPS_DATAHIERARCHY_H
