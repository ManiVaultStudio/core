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

        auto contextMenu = action->getContextMenu();

        if (contextMenu)
            menu->addMenu(contextMenu);
    }

    return menu;
}

void DataHierarchyItem::populateContextMenu(QMenu* contextMenu)
{
    for (auto action : _actions) {
        if (action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu))
            continue;

        auto actionContextMenu = action->getContextMenu();

        if (actionContextMenu)
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

QVariantMap DataHierarchyItem::toVariantMap() const
{
    QVariantMap variantMap, children;

    try
    {
        variantMap = WidgetAction::toVariantMap();
    }
    catch (...)
    {
    }

    std::uint32_t childSortIndex = 0;

    for (auto child : getChildren()) {
        auto dataHierarchyItemMap = child->toVariantMap();

        dataHierarchyItemMap["SortIndex"] = childSortIndex;

        children[child->getDataset()->getId()] = dataHierarchyItemMap;

        childSortIndex++;
    }

    variantMap["Name"]      = _dataset->text();
    variantMap["Expanded"]  = QVariant::fromValue(_expanded);
    variantMap["Visible"]   = QVariant::fromValue(isVisible());
    variantMap["Selected"]  = QVariant::fromValue(isSelected());
    variantMap["Dataset"]   = _dataset->toVariantMap();
    variantMap["Children"]  = children;

    return variantMap;
}

}
