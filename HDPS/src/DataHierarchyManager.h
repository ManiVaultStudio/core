#ifndef HDPS_DATA_HIERARCHY_MANAGER_H
#define HDPS_DATA_HIERARCHY_MANAGER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>

namespace hdps
{

class DataHierarchyManager : public QObject
{
    Q_OBJECT

public:

    /**
     * 
     */
    class DataHierarchyItem
    {
    public:
        DataHierarchyItem(const QString& name = "", const QString& parent = "", const bool& visibleInGui = true) :
            _datasetName(name),
            _parent(parent),
            _children(),
            _visibleInGui(visibleInGui)
        {
        }

        QString getDatasetName() const { return _datasetName; }
        QString getParent() const { return _parent; }
        QStringList getChildren() const { return _children; }
        std::uint32_t getNumberOfChildren() const { return _children.count(); }
        QString getVisibleInGui() const { return _visibleInGui; }

        void addChild(const QString& name) {
            _children << name;
        }

        void removeChild(const QString& name) {
            _children.removeAll(name);
        }

        QString toString() const {
            return QString("Name=%1, Parent=%2, Children=[%3], Visible in GUI=%4").arg(_datasetName, _parent, _children.join(", "), _visibleInGui ? "true" : "false");
        }

        DataHierarchyItem& operator= (const DataHierarchyItem& other)
        {
            _datasetName    = other._datasetName;
            _parent         = other._parent;
            _children       = other._children;
            _visibleInGui   = other._visibleInGui;

            return *this;
        }

    protected:
        QString         _datasetName;       /** Name of the dataset */
        QString         _parent;            /** Parent item */
        QStringList     _children;          /** Child items (if any) */
        bool            _visibleInGui;      /** Whether the dataset is visible in the GUI */
    };

    using DataHierarchyItemsMap = QMap<QString, DataHierarchyItem>;

public:
    DataHierarchyManager(QObject* parent = nullptr);

    /**
     * Add a dataset to the hierarchy
     * @param datasetName Name of the dataset
     * @param parentDatasetName Name of the parent dataset (if any)
     * @param visibleInGui Whether the dataset is visible in the gui
     */
    void addDataset(const QString& datasetName, const QString& parentDatasetName = "", const bool& visibleInGui = true);

    /**
     * Removes a dataset from the hierarchy
     * @param datasetName Name of the dataset
     * @return Whether the dataset was removed
     */
    bool removeDataset(const QString& datasetName);

    /**
     * Get hierarchy item by dataset name
     * @param datasetName Name of the dataset
     * @return Data hierarchy item
     */
    const DataHierarchyItem& getHierarchyItem(const QString& datasetName) const;

protected:

    /**
     * Get dataset children
     * @param datasetName Name of the dataset
     * @param recursive Whether to get all children in a recursive manner
     * @return Children
     */
    QStringList getChildren(const QString& datasetName, const bool& recursive = true);

signals:

    /**
     * Invoked when an item is added to the hierarchy manager
     * @param dataHierarchyItem Added item
     */
    void itemAdded(const DataHierarchyItem& dataHierarchyItem);

    /**
     * Invoked when an item is removed from the hierarchy manager
     * @param datasetName Name of the removed dataset
     */
    void itemRemoved(const DataHierarchyItem& dataHierarchyItem);

private:
    DataHierarchyItemsMap   _dataHierarchyItemsMap;     /** Data hierarchy items map */
};

inline QDebug operator<<(QDebug dbg, const DataHierarchyManager::DataHierarchyItem& dataHierarchyItem)
{
    dbg.nospace() << dataHierarchyItem.toString();

    return dbg.space();
}

}

#endif // HDPS_DATA_HIERARCHY_MANAGER_H
