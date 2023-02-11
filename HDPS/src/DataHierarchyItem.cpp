#include "DataHierarchyItem.h"
#include "Set.h"
#include "Dataset.h"
#include "Application.h"

#include "util/Serialization.h"

#include <QMenu>

#include <stdexcept>

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps
{

DataHierarchyItem::DataHierarchyItem(QObject* parent, Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible /*= true*/, const bool& selected /*= false*/) :
    WidgetAction(parent, "Data Hierarchy Item"),
    _dataset(dataset),
    _parent(),
    _children(),
    _fullPathName(),
    _selected(false),
    _expanded(true),
    _taskDescription(""),
    _taskProgress(0.0),
    _subTasks(),
    _taskName(""),
    _taskStatus(TaskStatus::Idle),
    _taskDescriptionTimer(),
    _taskProgressTimer(),
    _icon(),
    _actions()
{
    setText(dataset->getGuiName());

    if (parentDataset.isValid())
        _parent = &parentDataset->getDataHierarchyItem();

    setIcon(getDataset()->getIcon());

    _taskDescriptionTimer.setSingleShot(true);
    _taskProgressTimer.setSingleShot(true);

    connect(&_taskDescriptionTimer, &QTimer::timeout, [this]() {
        emit taskDescriptionChanged(_taskDescription);
    });

    connect(&_taskProgressTimer, &QTimer::timeout, [this]() {
        emit taskProgressChanged(getTaskProgress());
    });

    setVisible(visible);
    computeFullPathName();
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

        if (!_dataset.isValid())
            throw std::runtime_error("Dataset is invalid");

        if (newGuiName.isEmpty())
            throw std::runtime_error("New GUI name is empty");

        if (newGuiName == _dataset->getGuiName())
            return;

        _dataset->setGuiName(newGuiName);

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

        getParents(dataHierarchyItem.getParent(), parents);

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

void DataHierarchyItem::setVisible(bool visible)
{
    if (visible == isVisible())
        return;

    WidgetAction::setVisible(visible);

    emit visibilityChanged(isVisible());
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
    if (selected == _selected)
        return;

    _selected = selected;

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

void DataHierarchyItem::computeFullPathName()
{
    DataHierarchyItems parents;

    DataHierarchyItem::getParents(*const_cast<DataHierarchyItem*>(this), parents);

    QStringList dataHierarchyItemNames;

    for (const auto& parent : parents)
        dataHierarchyItemNames << parent->getDataset()->getGuiName();

    dataHierarchyItemNames << _dataset->getGuiName();

    _fullPathName = dataHierarchyItemNames.join("/");
}

QString DataHierarchyItem::getFullPathName() const
{
    return _fullPathName;
}

void DataHierarchyItem::addChild(DataHierarchyItem& child)
{
    _children << &child;
}

QString DataHierarchyItem::toString() const
{
    return QString("DataHierarchyItem[name=%1, parent=%2, children=[%3], visible=%4, description=%5, progress=%6]").arg(_dataset->getGuiName(), _parent->getGuiName(), QString::number(_children.count()), isVisible() ? "true" : "false", _taskDescription, QString::number(_taskProgress, 'f', 1));
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
    _actions << &widgetAction;

    emit actionAdded(widgetAction);
}

WidgetActions DataHierarchyItem::getActions() const
{
    return _actions;
}

QMenu* DataHierarchyItem::getContextMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu(parent);

    for (auto action : _actions) {
        auto contextMenu = action->getContextMenu();

        if (contextMenu)
            menu->addMenu(contextMenu);
    }

    return menu;
}

void DataHierarchyItem::populateContextMenu(QMenu* contextMenu)
{
    for (auto action : _actions) {
        auto actionContextMenu = action->getContextMenu();

        if (actionContextMenu)
            contextMenu->addMenu(actionContextMenu);
    }
}

bool DataHierarchyItem::getLocked() const
{
    return _dataset->isLocked();
}

void DataHierarchyItem::setLocked(const bool& locked)
{
    _dataset->setLocked(locked);
   
}

bool DataHierarchyItem::isExpanded() const
{
    return _expanded;
}

void DataHierarchyItem::setExpanded(bool expanded)
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
    if (taskName == _taskName)
        return;

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
    if (taskDescription == _taskDescription)
        return;

    _taskDescription = taskDescription;

    if (!_taskDescriptionTimer.isActive())
        _taskDescriptionTimer.start(TASK_UPDATE_TIMER_INTERVAL);
}

float DataHierarchyItem::getTaskProgress() const
{
    if (!_subTasks.isEmpty())
        return static_cast<float>(_subTasks.count(true)) / static_cast<float>(_subTasks.size());

    return _taskProgress;
}

void DataHierarchyItem::setTaskProgress(float taskProgress)
{
    if (taskProgress == _taskProgress)
        return;

    _taskProgress = taskProgress;

    if (!_taskProgressTimer.isActive())
        _taskProgressTimer.start(TASK_UPDATE_TIMER_INTERVAL);
}

void DataHierarchyItem::setNumberOfSubTasks(float numberOfSubTasks)
{
    _subTasks.resize(numberOfSubTasks);
}

void DataHierarchyItem::setSubTaskFinished(float subTaskIndex)
{
    try {
        _subTasks.setBit(subTaskIndex, true);

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
    _taskStatus = TaskStatus::Idle;
}

void DataHierarchyItem::setTaskRunning()
{
    _taskStatus = TaskStatus::Running;

    setLocked(true);
}

void DataHierarchyItem::setTaskFinished()
{
    if (_taskStatus == TaskStatus::Aborted)
        return;

    _taskStatus = TaskStatus::Finished;

    _subTasks.fill(false);

    setTaskDescription(QString("%1 finished").arg(_taskName));

    QTimer::singleShot(MESSAGE_DISAPPEAR_INTERVAL, [this]() {
        setTaskProgress(0.0f);
        setTaskDescription("");
    });

    setLocked(false);
}

void DataHierarchyItem::setTaskAborted()
{
    _taskStatus = TaskStatus::Aborted;

    _subTasks.fill(false);

    setTaskDescription(QString("%1 aborted").arg(_taskName));

    QTimer::singleShot(MESSAGE_DISAPPEAR_INTERVAL, [this]() {
        setTaskProgress(0.0f);
        setTaskDescription("");
    });

    setLocked(false);
}

QIcon DataHierarchyItem::getIcon() const
{
    return _icon;
}

void DataHierarchyItem::setIcon(const QIcon& icon)
{
    _icon = icon;
}

void DataHierarchyItem::fromVariantMap(const QVariantMap& variantMap)
{
    emit loading();
    {
        variantMapMustContain(variantMap, "Expanded");
        variantMapMustContain(variantMap, "Visible");

        setExpanded(variantMap["Expanded"].toBool());
        setVisible(variantMap["Visible"].toBool());
    }
    emit loaded();
}

QVariantMap DataHierarchyItem::toVariantMap() const
{
    emit const_cast<DataHierarchyItem*>(this)->saving();

    QVariantMap variantMap, children;

    std::uint32_t childSortIndex = 0;

    for (auto child : getChildren()) {
        auto dataHierarchyItemMap = child->toVariantMap();

        dataHierarchyItemMap["SortIndex"] = childSortIndex;

        children[child->getDataset()->getGuid()] = dataHierarchyItemMap;

        childSortIndex++;
    }

    emit const_cast<DataHierarchyItem*>(this)->saved();

    return {
        { "Name", getGuiName() },
        { "Expanded", QVariant::fromValue(_expanded) },
        { "Visible", QVariant::fromValue(isVisible()) },
        { "Dataset", _dataset->toVariantMap() },
        { "Children", children }
    };
}

}
