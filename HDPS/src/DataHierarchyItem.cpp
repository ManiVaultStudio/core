#include "DataHierarchyItem.h"
#include "Core.h"
#include "DataManager.h"
#include "Set.h"
#include "Dataset.h"

#include <QTimer>

#include <stdexcept>

using namespace hdps::gui;

namespace hdps
{

DataHierarchyItem::DataHierarchyItem(QObject* parent, Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible /*= true*/, const bool& selected /*= false*/) :
    QObject(parent),
    _dataset(dataset),
    _parent(),
    _children(),
    _visible(visible),
    _selected(false),
    _locked(false),
    _namedIcons(),
    _taskDescription(""),
    _taskProgress(0.0),
    _taskName(""),
    _taskStatus(TaskStatus::Idle),
    _actions()
{
    // Set parent item
    if (parentDataset.isValid())
        _parent = &parentDataset->getDataHierarchyItem();

    // Add data icon
    addIcon("data", getDataset()->getIcon());
}

DataHierarchyItem::~DataHierarchyItem()
{
    // Only proceed if we have a valid parent
    if (_parent == nullptr)
        return;

    // Remove child from parent
    _parent->removeChild(this);
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
    }
    catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Unable to rename dataset", e.what());
    }
}

DataHierarchyItem& DataHierarchyItem::getParent() const
{
    return *_parent;
}

void DataHierarchyItem::getParents(DataHierarchyItems& parents) const
{
    // Add ourselves
    parents << const_cast<DataHierarchyItem*>(this);

    // And possibly our parents
    if (hasParent())
        getParents(parents);
}

void DataHierarchyItem::setParent(DataHierarchyItem& parent)
{
    _parent = &parent;
}

bool DataHierarchyItem::hasParent() const
{
    return _parent != nullptr;
}

DataHierarchyItems DataHierarchyItem::getChildren() const
{
    return _children;
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

hdps::DataHierarchyItem::IconList DataHierarchyItem::getIcons() const
{
    return _namedIcons;
}

void DataHierarchyItem::addIcon(const QString& name, const QIcon& icon)
{
    _namedIcons << NamedIcon(name, icon);
}

void DataHierarchyItem::removeIcon(const QString& name)
{
    // Loop over all icons and remove them from the list if it matches the name
    for (const auto& namedIcon : _namedIcons)
        if (name == namedIcon.first)
            _namedIcons.removeOne(namedIcon);
}

QIcon DataHierarchyItem::getIconByName(const QString& name) const
{
    // Loop over all icons and return it if it matches the name
    for (const auto& namedIcon : _namedIcons)
        if (name == namedIcon.first)
            return namedIcon.second;

    return QIcon();
}

QString DataHierarchyItem::getFullPathName() const
{
    DataHierarchyItems parents;

    // Walk up the tree and fetch all parents
    getParents(parents);

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

void DataHierarchyItem::removeChild(DataHierarchyItem* dataHierarchyItem)
{
    _children.removeOne(dataHierarchyItem);
}

QString DataHierarchyItem::toString() const
{
    return QString("DataHierarchyItem[name=%1, parent=%2, children=[%3], visible=%4, description=%5, progress=%6]").arg(_dataset->getGuiName(), _parent->getGuiName(), QString::number(_children.count()), _visible ? "true" : "false", _taskDescription, QString::number(_taskProgress, 'f', 1));
}

Dataset<DatasetImpl> DataHierarchyItem::getDataset() const
{
    return _dataset;
}

DataType DataHierarchyItem::getDataType() const
{
    return _dataset->getDataType();
}

void DataHierarchyItem::notifyDataChanged()
{
    // Do not notify if we don't have a valid dataset
    if (!_dataset.isValid())
        return;

    // Notify others that the dataset data changed
    Application::core()->notifyDataChanged(_dataset);
}

void DataHierarchyItem::analyzeDataset(const QString& pluginName)
{
    Q_ASSERT(!pluginName.isEmpty());

    // Do not analyze if the plugin name is invalid
    if (pluginName.isEmpty())
        return;

    // Instruct the core to analyze the dataset
    Application::core()->analyzeDataset(pluginName, _dataset);
}

void DataHierarchyItem::exportDataset(const QString& pluginName)
{
    Q_ASSERT(!pluginName.isEmpty());

    // Do not export if the plugin name is invalid
    if (pluginName.isEmpty())
        return;

    // Instruct the core to export the dataset
    Application::core()->exportDataset(pluginName, _dataset);
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

    // Notify others that the data got locked/unlocked
    if (_locked)
        Application::core()->notifyDataLocked(_dataset);
    else
        Application::core()->notifyDataUnlocked(_dataset);
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

    // Notify others that the task description changed
    emit taskDescriptionChanged(_taskDescription);
}

float DataHierarchyItem::getTaskProgress() const
{
    return _taskProgress;
}

void DataHierarchyItem::setTaskProgress(const float& taskProgress)
{
    if (taskProgress == _taskProgress)
        return;

    _taskProgress = taskProgress;

    // Notify others that the task progress changed
    emit taskProgressChanged(_taskProgress);
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
    _taskStatus = TaskStatus::Idle;
}

void DataHierarchyItem::setTaskRunning()
{
    _taskStatus = TaskStatus::Running;
}

void DataHierarchyItem::setTaskFinished()
{
    // Do not proceed if already aborted
    if (_taskStatus == TaskStatus::Aborted)
        return;

    // Set task status to finished
    _taskStatus = TaskStatus::Finished;

    // Adjust the task description
    setTaskDescription(QString("%1 finished").arg(_taskName));

    // Create timer to let the message disappear after a while
    auto timer = QSharedPointer<QTimer>::create();

    // Reset the progress and task description when the timer times out
    connect(timer.get(), &QTimer::timeout, this, [this, timer]() {
        setTaskProgress(0.0f);
        setTaskDescription("");
    });

    // Start the timer
    timer->setSingleShot(true);
    timer->start(1500);
}

void DataHierarchyItem::setTaskAborted()
{
    _taskStatus = TaskStatus::Aborted;

    // Set task description to aborted
    setTaskDescription(QString("%1 aborted").arg(_taskName));

    // Create timer to let the message disappear after a while
    auto timer = QSharedPointer<QTimer>::create();

    // Reset the progress and task description when the timer times out
    connect(timer.get(), &QTimer::timeout, this, [this, timer]() {
        setTaskProgress(0.0f);
        setTaskDescription("");
    });

    // Start the timer
    timer->setSingleShot(true);
    timer->start(1500);
}

}
