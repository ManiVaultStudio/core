// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyItem.h"
#include "Set.h"
#include "Dataset.h"
#include "Application.h"

#include "util/Serialization.h"

#include <QMenu>

#include <stdexcept>

#ifdef _DEBUG
    #define DATA_HIERARCHY_ITEM_VERBOSE
#endif

using namespace mv::gui;
using namespace mv::util;

namespace mv
{

namespace 
{
    /** Utility class for managing workflow context in a thread-safe manner */
    class ToVariantMapWorkflowContext final : public WorkflowContextBase
    {
    public:

        /**
         * @brief Gets the indices from the context.
         * @return The indices stored in the context
         */
        QVariantMap getResult() const {
            QMutexLocker locker(&_mutex);
            return _result;
        }

        /**
         * @brief Gets a specific value from the result map of the context using the provided key.
         * @param key The key to look up in the result map
         * @return The value associated with the provided key in the result map, or an invalid QVariant if the key does not exist
         */
        QVariant getResultValue(const QString& key) const {
            QMutexLocker locker(&_mutex);
            return _result.value(key);
        }

        /**
         * @brief Sets a key-value pair in the result map of the context.
         * @param key The key to be set in the result map
         * @param value The value to be associated with the key in the result map
         */
        void setResultValue(const QString& key, const QVariant& value) {
            QMutexLocker locker(&_mutex);
            _result.insert(key, value);
        }

    private:
        mutable QMutex      _mutex;     /** Mutex for synchronizing access to the context */
        QVariantMap         _result;    /** The QVariantMap that will hold the final result of the workflow execution, containing the serialized headers and indices data. This map will be returned at the end of the workflow execution and can be used for further processing or storage. */
    };
}

DataHierarchyItem::DataHierarchyItem(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool visible /*= true*/, bool selected /*= false*/) :
    WidgetAction(nullptr, "Data Hierarchy Item"),
    _dataset(dataset),
    _selected(false),
    _expanded(true),
    _actions()
{
    const auto synchronizeText = [this]() -> void {
        setText(_dataset->text());
    };

    synchronizeText();

    connect(_dataset.get(), &DatasetImpl::textChanged, this, synchronizeText);

    if (parentDataset.isValid())
        setParent(&parentDataset->getDataHierarchyItem());
    
    setIcon(getDataset()->icon());
    setVisible(visible);
}

void DataHierarchyItem::setVisible(bool visible, bool recursively /*= true*/)
{
    if (visible == isVisible())
        return;

    WidgetAction::setVisible(visible);

    if (visible) {
        if (hasParent())
            getParent()->setVisible(visible, false);

        if (recursively)
            for (auto child : getChildren())
                child->setVisible(visible);
    }
    else {
        deselect();

        if (recursively)
            for (auto child : getChildren())
                child->setVisible(visible, recursively);
    }

    emit visibilityChanged(isVisible());
}

bool DataHierarchyItem::isSelected() const
{
    return _selected;
}

void DataHierarchyItem::setSelected(bool selected, bool clear /*= true*/)
{
    if (selected == _selected)
        return;

#ifdef DATA_HIERARCHY_ITEM_VERBOSE
    qDebug() << __FUNCTION__ << _dataset->getGuiName() << "selected:"  << selected << "clear:" << clear;
#endif

    if (clear)
        dataHierarchy().clearSelection();

    _selected = selected;

    emit selectedChanged(_selected);
}

void DataHierarchyItem::select(bool clear /*= true*/)
{
    setSelected(true, clear);
}

void DataHierarchyItem::deselect()
{
    setSelected(false, false);
}

Dataset<DatasetImpl> DataHierarchyItem::getDataset()
{
    return _dataset;
}

Dataset<mv::DatasetImpl>& DataHierarchyItem::getDatasetReference()
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
        if (action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu))
            continue;

        if (auto contextMenu = action->getContextMenu())
            menu->addMenu(contextMenu);
    }

    return menu;
}

void DataHierarchyItem::populateContextMenu(QMenu* contextMenu)
{
    for (auto action : _actions) {
        if (action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu))
            continue;

        if (auto actionContextMenu = action->getContextMenu())
            contextMenu->addMenu(actionContextMenu);
    }
}

void DataHierarchyItem::lock(bool cache /*= false*/)
{
    _dataset->lock(cache);
}

void DataHierarchyItem::unlock(bool cache /*= false*/)
{
    _dataset->unlock(cache);
}

bool DataHierarchyItem::isLocked() const
{
    return _dataset->isLocked();
}

void DataHierarchyItem::setLocked(bool locked, bool cache /*= false*/)
{
    _dataset->setLocked(locked, cache);
}

void DataHierarchyItem::restoreLockedFromCache()
{
    _dataset->restoreLockedFromCache();
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

void DataHierarchyItem::fromVariantMap(const QVariantMap& variantMap)
{
    try
    {
        WidgetAction::fromVariantMap(variantMap);
    }
    catch (...)
    {
    }

    variantMapMustContain(variantMap, "Expanded");
    variantMapMustContain(variantMap, "Visible");

    setExpanded(variantMap["Expanded"].toBool());
    setVisible(variantMap["Visible"].toBool());

    if (variantMap.contains("Selected"))
        setSelected(variantMap["Selected"].toBool(), false);
}

QVariantMap DataHierarchyItem::toVariantMapScoped(const SharedWorkflowExecutionContext& parentExecutionContext) const
{
    QVariantMap variantMap, children;

    try
    {
        variantMap = WidgetAction::toVariantMap();
    }
    catch (...)
    {
    }

    variantMap["Name"]      = _dataset->text();
    variantMap["Expanded"]  = QVariant::fromValue(_expanded);
    variantMap["Visible"]   = QVariant::fromValue(isVisible());
    variantMap["Selected"]  = QVariant::fromValue(isSelected());
    variantMap["Children"]  = QVariant::fromValue(children);
    variantMap["Dataset"]   = _dataset->toVariantMapScoped(parentExecutionContext);

    return variantMap;
}

}
