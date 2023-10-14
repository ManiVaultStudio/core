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

using namespace mv::gui;
using namespace mv::util;

namespace mv
{

DataHierarchyItem::DataHierarchyItem(QObject* parent, Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible /*= true*/, const bool& selected /*= false*/) :
    WidgetAction(parent, "Data Hierarchy Item"),
    _dataset(dataset),
    _parent(),
    _children(),
    _selected(false),
    _expanded(true),
    _icon(),
    _actions()
{
    const auto synchronizeText = [this]() -> void {
        setText(_dataset->text());
    };

    synchronizeText();

    connect(_dataset.get(), &DatasetImpl::textChanged, this, synchronizeText);

    if (parentDataset.isValid())
        _parent = &parentDataset->getDataHierarchyItem();

    setIcon(getDataset()->getIcon());
    setVisible(visible);
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

void DataHierarchyItem::addChild(DataHierarchyItem& child)
{
    _children << &child;
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

        children[child->getDataset()->getId()] = dataHierarchyItemMap;

        childSortIndex++;
    }

    emit const_cast<DataHierarchyItem*>(this)->saved();

    return {
        { "Name", _dataset->text() },
        { "Expanded", QVariant::fromValue(_expanded) },
        { "Visible", QVariant::fromValue(isVisible()) },
        { "Dataset", _dataset->toVariantMap() },
        { "Children", children }
    };
}

}
