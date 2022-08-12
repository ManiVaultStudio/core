#pragma once

#include "DataType.h"
#include "CoreInterface.h"
#include "actions/WidgetAction.h"
#include "event/Event.h"
#include "Set.h"
#include "actions/DataRemoveAction.h"
#include "actions/DataCopyAction.h"

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>
#include <QIcon>
#include <QTimer>
#include <QBitArray>

namespace hdps
{

class DatasetImpl;

/** Vector of data hierarchy item pointers */
using DataHierarchyItems = QVector<DataHierarchyItem*>;

/**
 * Data hierarchy item class
 *
 * Represents a dataset as an item in a dataset hierarchy
 *
 * @author Thomas Kroes
 */
class DataHierarchyItem : public hdps::gui::WidgetAction
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

    /** Map of named icons */
    using IconMap = QMap<QString, QIcon>;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param dataset Smart pointer to dataset
     * @param parentDataset Smart pointer to parent dataset (if any)
     * @param visible Whether the dataset is visible
     * @param selected Whether the dataset is selected
     */
    DataHierarchyItem(QObject* parent, Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible = true, const bool& selected = false);

    /** Destructor */
    ~DataHierarchyItem() = default;

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

    /** Get reference to parent hierarchy item */
    DataHierarchyItem& getParent() const;

    /**
     * Walks up the hierarchy of the data hierarchy item and returns all parents
     * @param dataHierarchyItem Item to fetch the parents for
     * @param parents Item parents
     */
    static void getParents(DataHierarchyItem& dataHierarchyItem, DataHierarchyItems& parents);

    /** Returns whether the data hierarchy item has a parent */
    bool hasParent() const;

    /**
     * Gets the names of the children name
     * @param recursive Recursive
     */
    DataHierarchyItems getChildren(const bool& recursive = false) const;

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

    /** Get the full path name of the data hierarchy item (separated by forward slashes) */
    QString getFullPathName() const;

protected:

    /** Set reference to parent hierarchy item */
    void setParent(DataHierarchyItem& parent);

public: // Hierarchy

    /**
     * Add a child
     * @param child Reference to child data hierarchy item
     */
    void addChild(DataHierarchyItem& child);

public: // Miscellaneous

    /** Gets the string representation of the hierarchy item */
    QString toString() const;

    /** Get the dataset */
    Dataset<DatasetImpl> getDataset();

    /** Get the dataset */
    template<typename DatasetType>
    Dataset<DatasetType> getDataset() const {
        return Dataset<DatasetType>(const_cast<DataHierarchyItem*>(this)->getDataset().get<DatasetType>());
    };

    /**
     * Get reference to dataset smart pointer
     * @return Reference to dataset smart pointer
     */
    Dataset<DatasetImpl>& getDatasetReference();

    /**
     * Get reference to dataset smart pointer of a specific type
     * @return Reference to dataset smart pointer of a specific type
     */
    template<typename DatasetType>
    Dataset<DatasetType>& getDatasetReference() const {
        return Dataset<DatasetType>(const_cast<DataHierarchyItem*>(this)->getDataset().get<DatasetType>());
    };

    /** Get the dataset type */
    DataType getDataType() const;

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
    QMenu* getContextMenu(QWidget* parent = nullptr) override;

    /**
     * Populates existing menu with actions menus
     * @param contextMenu Context menu to populate
     */
    void populateContextMenu(QMenu* contextMenu);;

public: // Locked

    /** Get locked status */
    bool getLocked() const;

    /**
     * Set locked status
     * @param locked Whether the dataset is locked
     */
    void setLocked(const bool& locked);

public: // Expanded

    /**
     * Get expanded status
     * @return Boolean indicating whether the item is expanded or not
     */
    bool isExpanded() const;

    /**
     * Set expanded status
     * @param expanded Whether the dataset is expanded
     */
    void setExpanded(const bool& expanded);

public: // Tasks

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
     * @param taskDescription Task description [0, 1]
     */
    void setTaskDescription(const QString& taskDescription);

    /** Gets the task progress [0, 1] */
    float getTaskProgress() const;

    /**
     * Sets the task progress
     * @param taskProgress Task progress
     */
    void setTaskProgress(const float& taskProgress);

    /**
     * Set the number of sub tasks
     * @param numberOfSubTasks Number of sub tasks
     */
    void setNumberOfSubTasks(const float& numberOfSubTasks);

    /**
     * Flag sub task as finished
     * @param subTaskIndex Index of the sub task
     */
    void setSubTaskFinished(const float& subTaskIndex);

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

public: // Named icons

    /** Gets map of named icons */
    IconMap getIcons() const;

    /**
     * Add named icon
     * @param name Name of the icon
     * @param icon Icon
     */
    void addIcon(const QString& name, const QIcon& icon);

    /**
     * Set icon by \p name
     * @param name Name of the icon
     * @param icon Icon
     */
    void setIconByName(const QString& name, const QIcon& icon);

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

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

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

    /** Signals that the set icon has changed */
    void iconsChanged();

    /**
     * Signals that the dataset name changed
     * @param datasetName New name of the dataset
     */
    void datasetNameChanged(const QString& datasetName);

    /**
     * Signals that the locked status changed
     * @param locked Locked
     */
    void lockedChanged(const bool& locked);

    /**
     * Signals that the expansion status changed
     * @param expanded Whether the item is expanded or not
     */
    void expandedChanged(const bool& expanded);

    /** Signals that the data hierarchy item is being loaded */
    void loading();

    /** Signals that the data hierarchy item has been loaded */
    void loaded();

    /** Signals that the data hierarchy item is being saved */
    void saving();

    /** Signals that the data hierarchy item has been saved */
    void saved();

protected:
    Dataset<DatasetImpl>        _dataset;               /** Smart pointer to dataset */
    DataHierarchyItem*          _parent;                /** Pointer to parent data hierarchy item */
    DataHierarchyItems          _children;              /** Pointers to child items (if any) */
    bool                        _visible;               /** Whether the dataset is visible */
    bool                        _selected;              /** Whether the hierarchy item is selected */
    bool                        _locked;                /** Whether the dataset is locked */
    bool                        _expanded;              /** Whether the item is expanded or not (when it has children) */
    QString                     _taskDescription;       /** Task description */
    float                       _taskProgress;          /** Task progress */
    QBitArray                   _subTasks;              /** Sub-tasks bit array */
    QString                     _taskName;              /** Name of the current task */
    TaskStatus                  _taskStatus;            /** Status of the current task */
    QTimer                      _taskDescriptionTimer;  /** Task description timer which prevents excessive successive GUI updates */
    QTimer                      _taskProgressTimer;     /** Task progress timer which prevents excessive GUI updates */
    IconMap                     _namedIcons;            /** Named icons */
    hdps::gui::WidgetActions    _actions;               /** Widget actions */
    DataRemoveAction            _dataRemoveAction;      /** Data remove action */
    DataCopyAction              _dataCopyAction;        /** Data copy action */

protected:
    friend class DataHierarchyManager;
    friend class DataManager;

    /** Single shot task update timer interval */
    static constexpr std::uint32_t TASK_UPDATE_TIMER_INTERVAL = 100;

    /** Single shot message disappear timer interval */
    static constexpr std::uint32_t MESSAGE_DISAPPEAR_INTERVAL = 1500;
};

/**
 * Print data hierarchy item to the console
 * @param debug Debug
 * @param dataHierarchyItem Data hierarchy item
 */
inline QDebug operator << (QDebug debug, const DataHierarchyItem& dataHierarchyItem)
{
    debug.nospace() << dataHierarchyItem.toString();

    return debug.space();
}

}
