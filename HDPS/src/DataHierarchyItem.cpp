#include "DataHierarchyItem.h"
#include "Core.h"
#include "DataManager.h"
#include "Set.h"
#include "Dataset.h"

#include <QMenu>

#include <stdexcept>

using namespace hdps::gui;

namespace hdps
{

DataHierarchyItem::DataHierarchyItem(QObject* parent, Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible /*= true*/, const bool& selected /*= false*/) :
    WidgetAction(parent),
    _dataset(dataset),
    _parent(),
    _children(),
    _visible(visible),
    _selected(false),
    _locked(false),
    _expanded(true),
    _taskDescription(""),
    _taskProgress(0.0),
    _subTasks(),
    _taskName(""),
    _taskStatus(TaskStatus::Idle),
    _taskDescriptionTimer(),
    _taskProgressTimer(),
    _namedIcons(),
    _actions(),
    _dataRemoveAction(parent, dataset),
    _dataCopyAction(parent, dataset)
{
    setText(dataset->getGuiName());

    // Set parent item
    if (parentDataset.isValid())
        _parent = &parentDataset->getDataHierarchyItem();

    // Add dataset icon
    addIcon("data", getDataset()->getIcon());

    // Task description/progress timer should be a one-off timer
    _taskDescriptionTimer.setSingleShot(true);
    _taskProgressTimer.setSingleShot(true);

    // Notify others that the task description changed when the task description timer timed out
    connect(&_taskDescriptionTimer, &QTimer::timeout, [this]() {
        emit taskDescriptionChanged(_taskDescription);
    });

    // Notify others that the task progress changed when the task progress timer timed out
    connect(&_taskProgressTimer, &QTimer::timeout, [this]() {
        emit taskProgressChanged(getTaskProgress());
    });
}

QString DataHierarchyItem::getGuiName() const
{
    return _dataset->getGuiName();
}

void DataHierarchyItem::setGuiName(const QString& guiName)
{
    _dataset->setGuiName(guiName);
}

void DataHierarchyItem::renameDataset(const QString& newGuiName)
{
    try {

        // Except if the don't have a valid dataset
        if (!_dataset.isValid())
            throw std::runtime_error("Dataset is invalid");

        // Except if the new GUI name is empty
        if (newGuiName.isEmpty())
            throw std::runtime_error("New GUI name is empty");

        // Do not proceed if the new GUI name matches the old GUI name
        if (newGuiName == _dataset->getGuiName())
            return;

        // Set the GUI name of the dataset dataset
        _dataset->setGuiName(newGuiName);

        // Rename has an effect on the full path name of children, so trigger a GUI rename of the children
        for (const auto& child : getChildren())
            child->getDataset()->setGuiName(child->getDataset()->getGuiName());
    }
    catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Unable to rename dataset", e.what());
    }
}

DataHierarchyItem& DataHierarchyItem::getParent() const
{
    return *_parent;
}

void DataHierarchyItem::getParents(DataHierarchyItem& dataHierarchyItem, DataHierarchyItems& parents)
{
    if (dataHierarchyItem.hasParent()) {

        // Recurse up the tree
        getParents(dataHierarchyItem.getParent(), parents);

        // Add parent
        parents << &dataHierarchyItem.getParent();
    }
}

void DataHierarchyItem::setParent(DataHierarchyItem& parent)
{
    _parent = &parent;
}

bool DataHierarchyItem::hasParent() const
{ 
    return _parent != nullptr;
}

DataHierarchyItems DataHierarchyItem::getChildren(const bool& recursive /*= false*/) const
{
    auto children = _children;

    if (recursive)
        for (auto child : _children)
            children << child->getChildren(recursive);

    return children;
}

std::uint32_t DataHierarchyItem::getNumberOfChildren() const
{
    return _children.count();
}

bool DataHierarchyItem::hasChildren() const
{
    return getNumberOfChildren() > 0;
}

bool DataHierarchyItem::getVisible() const
{
    return _visible;
}

bool DataHierarchyItem::isHidden() const
{
    return !_visible;
}

QString DataHierarchyItem::getTaskDescription() const
{
    return _taskDescription;
}

bool DataHierarchyItem::isSelected() const
{
    return _selected;
}

void DataHierarchyItem::setSelected(const bool& selected)
{
    // Prevent unnecessary updates
    if (selected == _selected)
        return;

    // Assign selected status
    _selected = selected;

    // Notify others that the selection status changed
    emit selectionChanged(_selected);
}

void DataHierarchyItem::select()
{
    setSelected(true);
}

void DataHierarchyItem::deselect()
{
    setSelected(false);
}

QString DataHierarchyItem::getFullPathName() const
{
    DataHierarchyItems parents;

    // Walk up the tree and fetch all parents
    DataHierarchyItem::getParents(*const_cast<DataHierarchyItem*>(this), parents);

    QStringList dataHierarchyItemNames;

    // Add GUI names of the parents to the string list
    for (const auto& parent : parents)
        dataHierarchyItemNames << parent->getDataset()->getGuiName();

    // Add name of this data hierarchy item
    dataHierarchyItemNames << _dataset->getGuiName();

    return dataHierarchyItemNames.join("/");
}

void DataHierarchyItem::addChild(DataHierarchyItem& child)
{
    _children << &child;
}

QString DataHierarchyItem::toString() const
{
    return QString("DataHierarchyItem[name=%1, parent=%2, children=[%3], visible=%4, description=%5, progress=%6]").arg(_dataset->getGuiName(), _parent->getGuiName(), QString::number(_children.count()), _visible ? "true" : "false", _taskDescription, QString::number(_taskProgress, 'f', 1));
}

Dataset<DatasetImpl> DataHierarchyItem::getDataset()
{
    return _dataset;
}

Dataset<hdps::DatasetImpl>& DataHierarchyItem::getDatasetReference()
{
    return _dataset;
}

DataType DataHierarchyItem::getDataType() const
{
    return _dataset->getDataType();
}

void DataHierarchyItem::addAction(WidgetAction& widgetAction)
{
    // Add action to the vector
    _actions << &widgetAction;

    // Notify others that an action was added
    emit actionAdded(widgetAction);
}

WidgetActions DataHierarchyItem::getActions() const
{
    return _actions;
}

QMenu* DataHierarchyItem::getContextMenu(QWidget* parent /*= nullptr*/)
{
    // Create new context menu
    auto menu = new QMenu(parent);

    // Loop over all actions and ad their context menu (if there is one)
    for (auto action : _actions) {

        // Get pointer to action context menu
        auto contextMenu = action->getContextMenu();

        // Add context menu when it is valid
        if (contextMenu)
            menu->addMenu(contextMenu);
    }

    menu->addSeparator();

    _dataRemoveAction.setEnabled(!_locked);
    _dataRemoveAction.setEnabled(false);

    menu->addAction(&_dataRemoveAction);
    menu->addAction(&_dataCopyAction);

    return menu;
}

void DataHierarchyItem::populateContextMenu(QMenu* contextMenu)
{
    // Populate context menu with items from actions
    for (auto action : _actions) {

        // Get pointer to action context menu
        auto actionContextMenu = action->getContextMenu();

        // Add context menu when it is valid
        if (actionContextMenu)
            contextMenu->addMenu(actionContextMenu);
    }
}

bool DataHierarchyItem::getLocked() const
{
    return _locked;
}

void DataHierarchyItem::setLocked(const bool& locked)
{
    // Prevent unnecessary updates
    if (locked == _locked)
        return;

    // Assign new locked status
    _locked = locked;

    // Notify others that the data got locked/unlocked through the core
    if (_locked)
        Application::core()->notifyDatasetLocked(_dataset);
    else
        Application::core()->notifyDatasetUnlocked(_dataset);

    // Notify others that the data got locked/unlocked
    emit lockedChanged(_locked);
}

bool DataHierarchyItem::isExpanded() const
{
    return _expanded;
}

void DataHierarchyItem::setExpanded(const bool& expanded)
{
    _expanded = expanded;

    emit expandedChanged(_expanded);
}

QString DataHierarchyItem::getTaskName() const
{
    return _taskName;
}

void DataHierarchyItem::setTaskName(const QString& taskName)
{
    // Prevent unnecessary updates
    if (taskName == _taskName)
        return;

    // Assign the task name
    _taskName = taskName;
}

hdps::DataHierarchyItem::TaskStatus DataHierarchyItem::getTaskStatus() const
{
    return _taskStatus;
}

bool DataHierarchyItem::isIdle() const
{
    return _taskStatus == TaskStatus::Idle;
}

void DataHierarchyItem::setTaskDescription(const QString& taskDescription)
{
    // Prevent unnecessary updates
    if (taskDescription == _taskDescription)
        return;

    // Assign new task description
    _taskDescription = taskDescription;

    // Start the task description timer if it is not already active
    if (!_taskDescriptionTimer.isActive())
        _taskDescriptionTimer.start(TASK_UPDATE_TIMER_INTERVAL);
}

float DataHierarchyItem::getTaskProgress() const
{
    if (!_subTasks.isEmpty())
        return static_cast<float>(_subTasks.count(true)) / static_cast<float>(_subTasks.size());

    return _taskProgress;
}

void DataHierarchyItem::setTaskProgress(const float& taskProgress)
{
    if (taskProgress == _taskProgress)
        return;

    // Assign the new task progress
    _taskProgress = taskProgress;

    // Start the task progress timer if it is not already active
    if (!_taskProgressTimer.isActive())
        _taskProgressTimer.start(TASK_UPDATE_TIMER_INTERVAL);
}

void DataHierarchyItem::setNumberOfSubTasks(const float& numberOfSubTasks)
{
    _subTasks.resize(numberOfSubTasks);
}

void DataHierarchyItem::setSubTaskFinished(const float& subTaskIndex)
{
    try {
        // Flag sub task as finished
        _subTasks.setBit(subTaskIndex, true);

        // Start the task progress timer if it is not already active
        if (!_taskProgressTimer.isActive())
            _taskProgressTimer.start(TASK_UPDATE_TIMER_INTERVAL);

        QCoreApplication::processEvents();
    }
    catch (...) {
        qDebug() << "Unable to set flag sub task as finished";
    }
}

bool DataHierarchyItem::isRunning() const
{
    return _taskStatus == TaskStatus::Running;
}

bool DataHierarchyItem::isFinished() const
{
    return _taskStatus == TaskStatus::Finished;
}

bool DataHierarchyItem::isAborted() const
{
    return _taskStatus == TaskStatus::Aborted;
}

void DataHierarchyItem::setTaskIdle()
{
    // Set task status to idle
    _taskStatus = TaskStatus::Idle;
}

void DataHierarchyItem::setTaskRunning()
{
    // Set task status to running
    _taskStatus = TaskStatus::Running;

    // lock the item
    setLocked(true);
}

void DataHierarchyItem::setTaskFinished()
{
    // Do not proceed if already aborted
    if (_taskStatus == TaskStatus::Aborted)
        return;

    // Set task status to finished
    _taskStatus = TaskStatus::Finished;

    // Reset sub tasks (if any)
    _subTasks.fill(false);

    // Adjust the task description
    setTaskDescription(QString("%1 finished").arg(_taskName));

    // Reset the progress and task description after a while
    QTimer::singleShot(MESSAGE_DISAPPEAR_INTERVAL, [this]() {
        setTaskProgress(0.0f);
        setTaskDescription("");
    });

    // Unlock the item
    setLocked(false);
}

void DataHierarchyItem::setTaskAborted()
{
    // Set task status to aborted
    _taskStatus = TaskStatus::Aborted;

    // Reset sub tasks (if any)
    _subTasks.fill(false);

    // Set task description to aborted
    setTaskDescription(QString("%1 aborted").arg(_taskName));

    // Reset the progress and task description after a while
    QTimer::singleShot(MESSAGE_DISAPPEAR_INTERVAL, [this]() {
        setTaskProgress(0.0f);
        setTaskDescription("");
    });

    // Unlock the item
    setLocked(false);
}

DataHierarchyItem::IconMap DataHierarchyItem::getIcons() const
{
    return _namedIcons;
}

void DataHierarchyItem::addIcon(const QString& name, const QIcon& icon)
{
    _namedIcons[name] = icon;
}

void DataHierarchyItem::setIconByName(const QString& name, const QIcon& icon)
{
    if (!_namedIcons.contains(name))
        return;

    _namedIcons[name] = icon;

    emit iconChanged(name, icon);
}

void DataHierarchyItem::removeIcon(const QString& name)
{
    _namedIcons.remove(name);
}

QIcon DataHierarchyItem::getIconByName(const QString& name) const
{
    if (!_namedIcons.contains(name))
        return QIcon();

    return _namedIcons[name];
}

void DataHierarchyItem::fromVariantMap(const QVariantMap& variantMap)
{
    emit loading();

    if (variantMap.contains("Locked"))
        setLocked(variantMap["Locked"].toBool());

    if (variantMap.contains("Expanded"))
        setExpanded(variantMap["Expanded"].toBool());

    emit loaded();
}

QVariantMap DataHierarchyItem::toVariantMap() const
{
    emit const_cast<DataHierarchyItem*>(this)->saving();

    QVariantMap variantMap, children;

    // Child items sort index
    std::uint32_t childSortIndex = 0;

    for (auto child : getChildren()) {

        // Get map of data hierarchy item
        auto dataHierarchyItemMap = child->toVariantMap();

        // Add sort index
        dataHierarchyItemMap["SortIndex"] = childSortIndex;

        // Assign child data hierarchy item map
        children[child->getDataset()->getGuid()] = dataHierarchyItemMap;

        childSortIndex++;
    }

    emit const_cast<DataHierarchyItem*>(this)->saved();

    return {
        { "Name", getGuiName() },
        { "Locked", _locked },
        { "Expanded", _expanded },
        { "Dataset", _dataset->toVariantMap() },
        { "Children", children }
    };
}

}
