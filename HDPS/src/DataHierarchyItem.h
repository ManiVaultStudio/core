#ifndef HDPS_DATA_HIERARCHY_ITEM_H
#define HDPS_DATA_HIERARCHY_ITEM_H

#include "DataType.h"

#include "util/DatasetRef.h"
#include "actions/WidgetAction.h"

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>
#include <QIcon>

namespace hdps
{

class DataSet;

/** Shared pointer of data hierarchy item */
using SharedDataHierarchyItem = QSharedPointer<DataHierarchyItem>;

/** Vector of data hierarchy item shared pointers */
using SharedDataHierarchyItems = QVector<SharedDataHierarchyItem>;

/** Vector of data hierarchy item pointers */
using DataHierarchyItems = QVector<DataHierarchyItem*>;

/**
 * Data hierarchy item class
 *
 * Represents a dataset as an item in a dataset hierarchy
 *
 * @author Thomas Kroes
 */
class DataHierarchyItem : public QObject
{
    Q_OBJECT

public:

    /** Task status enumeration */
    enum class TaskStatus {
        Undefined = -1,     /** Task status is undefined */
        Idle,               /** Task is idle */
        Running,            /** An Task is currently running */
        Finished,           /** Task has finished successfully */
        Aborted             /** Task has been aborted */
    };

public:

    /** Named icon */
    using NamedIcon = QPair<QString, QIcon>;

    /** List of named icons */
    using IconList = QList<NamedIcon>;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param dataset Dataset
     * @param parentDataset Pointer to parent dataset (if any)
     * @param visible Whether the dataset is visible
     * @param selected Whether the dataset is selected
     */
    DataHierarchyItem(QObject* parent, DataSet& dataset, DataSet* parentDataset = nullptr, const bool& visible = true, const bool& selected = false);

    /** Destructor */
    ~DataHierarchyItem();

    /** Get the dataset GUI name */
    QString getGuiName() const;

    /**
     * Set the dataset GUI name
     * @param guiName GUI name of the dataset
     */
    void setGuiName(const QString& guiName);

    /**
     * Renames the GUI name of the dataset
     * @param newGuiName New GUI name of the dataset
     */
    void renameDataset(const QString& newGuiName);

    /** Gets the parent hierarchy item */
    DataHierarchyItem* getParent() const;

    /** Returns whether the data hierarchy item has a parent */
    bool hasParent() const;

    /** Gets the names of the children name */
    DataHierarchyItems getChildren() const;

    /** Gets the number of children */
    std::uint32_t getNumberOfChildren() const;

    /** Establishes whether the item has any children */
    bool hasChildren() const;

    /** Gets whether the dataset is visible */
    bool getVisible() const;

    /** Gets whether the dataset is hidden */
    bool isHidden() const;

    /** Gets whether the hierarchy item is selected */
    bool isSelected() const;

    /**
     * Sets the item selection status
     * @param selected Whether the hierarchy item is selected
     */
    void setSelected(const bool& selected);

    /** Selects the hierarchy item */
    void select();

    /** De-selects the hierarchy item */
    void deselect();

    /** Gets list of named icons */
    IconList getIcons() const;

    /**
     * Add named icon
     * @param name Name of the icon
     * @param icon Icon
     */
    void addIcon(const QString& name, const QIcon& icon);

    /**
     * Remove icon by name
     * @param name Name of the icon
     */
    void removeIcon(const QString& name);

    /**
     * Get icon by name
     * @param name Name of the icon
     * @return Icon
     */
    QIcon getIconByName(const QString& name) const;

protected:

    /** Sets the parent hierarchy item */
    void setParent(DataHierarchyItem* parent);

public: // Hierarchy

    /**
     * Add a child
     * @param child Reference to child data hierarchy item
     */
    void addChild(DataHierarchyItem& child);

    /**
     * Removes a child (name reference to data hierarchy item)
     * @param dataHierarchyItem Pointer to data hierarchy item
     */
    void removeChild(DataHierarchyItem* dataHierarchyItem);

public: // Miscellaneous

    /** Gets the string representation of the hierarchy item */
    QString toString() const;

    /** Get the dataset */
    DataSet& getDataset();

    /** Get the dataset */
    template<typename DatasetType>
    DatasetType& getDataset() {
        return dynamic_cast<DatasetType&>(getDataset());
    };

    /** Get the dataset type */
    DataType getDataType() const;

    /** Let's subscribers know that the data changed (through the core) */
    void notifyDataChanged();

    /**
     * Analyze the dataset
     * @param pluginName Name of the analysis plugin
     */
    void analyzeDataset(const QString& pluginName);

    /**
     * Export the dataset
     * @param pluginName Name of the exporter plugin
     */
    void exportDataset(const QString& pluginName);

public: // Actions

    /** Add action */
    void addAction(hdps::gui::WidgetAction& widgetAction);

    /** Returns list of shared action widgets*/
    hdps::gui::WidgetActions getActions() const;

    /**
     * Get the context menu
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr);;

    /**
     * Populates existing menu with actions menus
     * @param contextMenu Context menu to populate
     */
    void populateContextMenu(QMenu* contextMenu);;

public: // Task

    /** Get task name */
    QString getTaskName() const;

    /**
     * Set task name
     * @param taskName Name of the task
     */
    void setTaskName(const QString& taskName);

    /** Get task status */
    TaskStatus getTaskStatus() const;

    /** Gets the task description */
    QString getTaskDescription() const;

    /**
     * Sets the task description
     * @param taskDescription Task description
     */
    void setTaskDescription(const QString& taskDescription);

    /** Gets the task progress */
    float getTaskProgress() const;

    /**
     * Sets the task progress
     * @param taskProgress Task progress
     */
    void setTaskProgress(const float& taskProgress);

    /** Convenience functions for status checking */
    bool isIdle() const;
    bool isRunning() const;
    bool isFinished() const;
    bool isAborted() const;

    /** Set task status */
    void setTaskIdle();
    void setTaskRunning();
    void setTaskFinished();
    void setTaskAborted();

signals:

    /**
     * Signals that the task description changed
     * @param taskDescription Task description
     */
    void taskDescriptionChanged(const QString& taskDescription);

    /**
     * Signals that the task progress changed
     * @param taskProgress Task progress
     */
    void taskProgressChanged(const float& taskProgress);

    /**
     * Signals that the item got selected
     * @param selected Whether the item is selected
     */
    void selectionChanged(const bool& selected);

    /**
     * Signals that a widget action was added
     * @param widgetAction Widget action that was added
     */
    void actionAdded(hdps::gui::WidgetAction& widgetAction);

    /**
     * Signals that the dataset names changed
     * @param datasetName New name of the dataset
     */
    void datasetNameChanged(const QString& datasetName);

protected:
    util::DatasetRef<DataSet>   _dataset;           /** Dataset reference */
    DataHierarchyItem*          _parent;            /** Pointer to parent data hierarchy item */
    DataHierarchyItems          _children;          /** Pointers to child items (if any) */
    bool                        _visible;           /** Whether the dataset is visible */
    bool                        _selected;          /** Whether the hierarchy item is selected */
    IconList                    _namedIcons;        /** Named icons */
    QString                     _taskDescription;   /** Task description */
    float                       _taskProgress;      /** Task progress */
    QString                     _taskName;          /** Name of the current task */
    TaskStatus                  _taskStatus;        /** Status of the current task */
    hdps::gui::WidgetActions    _actions;           /** Widget actions */

protected:
    friend class DataHierarchyManager;
    friend class DataManager;
};

/**
 * Print to console
 * @param debug Debug
 * @param dataHierarchyItem Data hierarchy item
 */
inline QDebug operator << (QDebug debug, const DataHierarchyItem& dataHierarchyItem)
{
    debug.nospace() << dataHierarchyItem.toString();

    return debug.space();
}

}

/**
 * Compares to named icons
 * @param lhs Left hand side icon
 * @param rhs Right hand side icon
 * @param dataHierarchyItem Data hierarchy item
 */
inline bool operator == (const hdps::DataHierarchyItem::NamedIcon& lhs, const hdps::DataHierarchyItem::NamedIcon& rhs)
{
    return lhs.first == rhs.first;
}

#endif // HDPS_DATA_HIERARCHY_ITEM_H
