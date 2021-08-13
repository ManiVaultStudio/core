#ifndef HDPS_DATA_HIERARCHY_ITEM_H
#define HDPS_DATA_HIERARCHY_ITEM_H

#include "DataType.h"

#include "actions/WidgetAction.h"

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>
#include <QIcon>

namespace hdps
{

class DataSet;
class Core;

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
        Idle,           /** Analysis is idle */
        Running,        /** An analysis is currently running */
        Finished,       /** Analysis has finished successfully */
        Aborted         /** Analysis has been aborted */
    };

public:

    /** Named icon */
    using NamedIcon = QPair<QString, QIcon>;

    /** List of named icons */
    using IconList = QList<NamedIcon>;

public:

    /**
     * Constructor
     * @param core Pointer to the core
     * @param parent Pointer to parent object
     * @param datasetName Name of the dataset
     * @param parentDatasetName Name of the parent dataset
     * @param visible Whether the dataset is visible
     * @param selected Whether the dataset is selected
     */
    DataHierarchyItem(Core* core, QObject* parent = nullptr, const QString& datasetName = "", const QString& parentDatasetName = "", const bool& visible = true, const bool& selected = false);

    /** Gets the dataset name */
    QString getDatasetName() const;

    /**
     * Renames the dataset
     * @param intendedDatasetName Intended new name of the dataset
     */
    void renameDataset(const QString& intendedDatasetName);

    /** Gets the parent dataset name */
    QString getParent() const;

    /** Gets the names of the children name */
    QStringList getChildren() const;

    /** Gets the number of children */
    std::uint32_t getNumberOfChildren() const;

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

public: // Hierarchy

    /**
     * Adds a child (name reference to data hierarchy item)
     * @param name Name of the child
     */
    void addChild(const QString& name);

    /**
     * Removes a child (name reference to data hierarchy item)
     * @param name Name of the child
     */
    void removeChild(const QString& name);

public: // Miscellaneous

    /** Gets the string representation of the hierarchy item */
    QString toString() const;

    /** Get the dataset */
    DataSet& getDataset() const;

    /** Get the dataset */
    template<typename DatasetType>
    DatasetType& getDataset() const {
        return dynamic_cast<DatasetType&>(getDataset());
    };

    /** Get the dataset type */
    DataType getDataType() const;

    /**
     * Analyze the dataset
     * @param analysisKind Type (kind) of analysis
     */
    void analyzeDataset(const QString& analysisKind);

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

public: // Operators

    /**
     * Assignment operator
     * @param other Data hierarchy item to copy from
     */
    DataHierarchyItem& operator= (const DataHierarchyItem& other)
    {
        _core               = other._core;
        _datasetName        = other._datasetName;
        _parent             = other._parent;
        _children           = other._children;
        _visible            = other._visible;
        _taskDescription    = other._taskDescription;
        _taskProgress       = other._taskProgress;
        _selected           = other._selected;
        _namedIcons         = other._namedIcons;
        _taskName           = other._taskName;
        _taskStatus         = other._taskStatus;
        _actions            = other._actions;

        return *this;
    }

protected:
    /** Sets the dataset name
     * @param datasetName Name of the dataset
     */
    void setDatasetName(const QString& datasetName);

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

protected:
    Core*                       _core;              /** Pointer to core */
    QString                     _datasetName;       /** Name of the dataset */
    QString                     _parent;            /** Parent item */
    QStringList                 _children;          /** Child items (if any) */
    bool                        _visible;           /** Whether the dataset is visible */
    bool                        _selected;          /** Whether the hierarchy item is selected */
    IconList                    _namedIcons;        /** Named icons */
    QString                     _taskDescription;   /** Task description */
    float                       _taskProgress;      /** Task progress */
    QString                     _taskName;          /** Name of the current task */
    TaskStatus                  _taskStatus;        /** Status of the current task */
    hdps::gui::WidgetActions    _actions;           /** Widget actions */

protected:
    friend class DataManager;
};

/** Shared pointer of data hierarchy item */
using SharedDataHierarchyItem = QSharedPointer<DataHierarchyItem>;

/** Maps string to shared data hierarchy item */
using DataHierarchyItemsMap = QMap<QString, SharedDataHierarchyItem>;

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
