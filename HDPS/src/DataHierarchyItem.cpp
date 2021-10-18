#include "DataHierarchyItem.h"
#include "Core.h"
#include "DataManager.h"
#include "Set.h"

#include <QTimer>

#include <stdexcept>

using namespace hdps::gui;

namespace hdps
{

DataHierarchyItem::DataHierarchyItem(QObject* parent /*= nullptr*/, const QString& datasetName /*= ""*/, const QString& parentDatasetName /*= ""*/, const bool& visible /*= true*/, const bool& selected /*= false*/) :
    QObject(parent),
    _dataset(datasetName),
    _parent(),
    _children(),
    _visible(visible),
    _selected(false),
    _namedIcons(),
    _taskDescription(""),
    _taskProgress(0.0),
    _taskName(""),
    _taskStatus(TaskStatus::Idle),
    _actions()
{
    if (!parentDatasetName.isEmpty())
        _parent = Application::core()->getDataHierarchyItem(parentDatasetName);

    addIcon("data", getDataset().getIcon());
}

DataHierarchyItem::~DataHierarchyItem()
{
    if (_parent == nullptr)
        return;

    _parent->removeChild(this);

    /*
    for (auto child : _children)
        delete child;
    */
}

QString DataHierarchyItem::getDatasetName() const
{
    return _dataset.getDatasetName();
}

void DataHierarchyItem::setDatasetName(const QString& datasetName)
{
    _dataset.setDatasetName(datasetName);
}

void DataHierarchyItem::renameDataset(const QString& intendedDatasetName)
{
    try {
        if (intendedDatasetName.isEmpty())
            throw std::runtime_error("Intended dataset name is empty");

        if (intendedDatasetName == _dataset.getDatasetName())
            return;

        Application::core()->renameDataset(_dataset.getDatasetName(), intendedDatasetName);
    }
    catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Unable to rename dataset", e.what());
    }
}

DataHierarchyItem* DataHierarchyItem::getParent() const
{
    return _parent;
}

void DataHierarchyItem::setParent(DataHierarchyItem* parent)
{
    _parent = parent;
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
    for (const auto& namedIcon : _namedIcons)
        if (name == namedIcon.first)
            _namedIcons.removeOne(namedIcon);
}

QIcon DataHierarchyItem::getIconByName(const QString& name) const
{
    for (const auto& namedIcon : _namedIcons)
        if (name == namedIcon.first)
            return namedIcon.second;

    return QIcon();
}

void DataHierarchyItem::addChild(const QString& name)
{
    _children << Application::core()->getDataHierarchyItem(name);
}

void DataHierarchyItem::removeChild(DataHierarchyItem* dataHierarchyItem)
{
    _children.removeOne(dataHierarchyItem);
}

QString DataHierarchyItem::toString() const
{
    return QString("DataHierarchyItem[name=%1, parent=%2, children=[%3], visible=%4, description=%5, progress=%6]").arg(_dataset.getDatasetName(), _parent->getDatasetName(), QString::number(_children.count()), _visible ? "true" : "false", _taskDescription, QString::number(_taskProgress, 'f', 1));
}

DataSet& DataHierarchyItem::getDataset()
{
    return *_dataset;
}

DataType DataHierarchyItem::getDataType() const
{
    return _dataset->getDataType();
}

void DataHierarchyItem::notifyDataChanged()
{
    if (!_dataset.isValid())
        return;

    _dataset.notifyDataChanged();
}

void DataHierarchyItem::analyzeDataset(const QString& pluginName)
{
    Q_ASSERT(!pluginName.isEmpty());

    if (pluginName.isEmpty())
        return;

    Application::core()->analyzeDataset(pluginName, _dataset.getDatasetName());
}

void DataHierarchyItem::exportDataset(const QString& pluginName)
{
    Q_ASSERT(!pluginName.isEmpty());

    if (pluginName.isEmpty())
        return;

    Application::core()->exportDataset(pluginName, _dataset.getDatasetName());
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

        if (contextMenu == nullptr)
            continue;

        menu->addMenu(contextMenu);
    }

    return menu;
}

void DataHierarchyItem::populateContextMenu(QMenu* contextMenu)
{
    for (auto action : _actions) {
        auto actionContextMenu = action->getContextMenu();

        if (actionContextMenu == nullptr)
            continue;

        contextMenu->addMenu(actionContextMenu);
    }
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
    if (_taskStatus == TaskStatus::Aborted)
        return;

    _taskStatus = TaskStatus::Finished;

    setTaskDescription(QString("%1 finished").arg(_taskName));

    auto timer = QSharedPointer<QTimer>::create();

    connect(timer.get(), &QTimer::timeout, this, [this, timer]() {
        setTaskProgress(0.0f);
        setTaskDescription("");
    });

    timer->setSingleShot(true);
    timer->start(1500);
}

void DataHierarchyItem::setTaskAborted()
{
    _taskStatus = TaskStatus::Aborted;

    setTaskDescription(QString("%1 aborted").arg(_taskName));

    auto timer = QSharedPointer<QTimer>::create();

    connect(timer.get(), &QTimer::timeout, this, [this, timer]() {
        setTaskProgress(0.0f);
        setTaskDescription("");
    });

    timer->setSingleShot(true);
    timer->start(1500);
}

}
