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

DataHierarchyItem::DataHierarchyItem(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible /*= true*/, const bool& selected /*= false*/) :
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

    setIcon(getDataset()->getIcon());
    setVisible(visible);
}

bool DataHierarchyItem::hasParent() const
{
    return parent();
}

DataHierarchyItem* DataHierarchyItem::getParent() const
{
    return static_cast<DataHierarchyItem*>(parent());
}

void DataHierarchyItem::setParent(DataHierarchyItem* parent)
{
    WidgetAction::setParent(parent);

    emit parentChanged(getParent());
}

DataHierarchyItems DataHierarchyItem::getParents() const
{
    DataHierarchyItems parentDataHierarchyItems;

    if (hasParent()) {
        parentDataHierarchyItems << getParent();
        parentDataHierarchyItems << getParent()->getParents();
    }

    return parentDataHierarchyItems;
}

bool DataHierarchyItem::isChildOf(DataHierarchyItem& dataHierarchyItem) const
{
    return getParents().contains(&dataHierarchyItem);
}

bool DataHierarchyItem::isChildOf(DataHierarchyItems dataHierarchyItems) const
{
    for (auto dataHierarchyItem : dataHierarchyItems)
        if (dataHierarchyItem != this && isChildOf(*dataHierarchyItem))
            return true;

    return false;
}

DataHierarchyItems DataHierarchyItem::getChildren(bool recursively /*= false*/) const
{
    DataHierarchyItems childDataHierarchyItems;

    for (auto childObject : findChildren<QObject*>(recursively ? Qt::FindChildrenRecursively : Qt::FindDirectChildrenOnly)) {
        auto childDataHierarchyItem = dynamic_cast<DataHierarchyItem*>(childObject);

        if (childDataHierarchyItem)
            childDataHierarchyItems << childDataHierarchyItem;
    }

    return childDataHierarchyItems;
}

std::uint32_t DataHierarchyItem::getNumberOfChildren(bool recursively /*= false*/) const
{
    return getChildren(recursively).count();
}

bool DataHierarchyItem::hasChildren() const
{
    return getNumberOfChildren() > 0;
}

std::int32_t DataHierarchyItem::getDepth() const
{
    return getParents().count();
}

void DataHierarchyItem::setVisible(bool visible, bool recursive /*= true*/)
{
    if (visible == isVisible())
        return;

    WidgetAction::setVisible(visible);

    if (visible) {
        if (hasParent())
            getParent()->setVisible(visible, false);

        if (recursive)
            for (auto child : getChildren())
                child->setVisible(visible);
    }
    else {
        if (recursive)
            for (auto child : getChildren())
                child->setVisible(visible, recursive);
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
