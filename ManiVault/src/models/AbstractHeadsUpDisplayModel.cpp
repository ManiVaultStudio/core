// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractHeadsUpDisplayModel.h"

using namespace mv;

#ifdef _DEBUG
    #define HEADS_UP_DISPLAY_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

AbstractHeadsUpDisplayModel::Item::Item(const HeadsUpDisplayItemSharedPtr& headsUpDisplayItem) :
    _headsUpDisplayItem(headsUpDisplayItem)
{
}

HeadsUpDisplayItemSharedPtr AbstractHeadsUpDisplayModel::Item::getHeadsupDisplayItem() const
{
    return _headsUpDisplayItem;
}

QVariant AbstractHeadsUpDisplayModel::IdItem::data(int role) const
{
    if (!getHeadsupDisplayItem())
        return Item::data(role);

    switch (role) {
	    case Qt::EditRole:
        case Qt::DisplayRole:
	        return getHeadsupDisplayItem()->getId();

	    case Qt::ToolTipRole:
	        return QString("Global unique identifier: %1").arg(data(Qt::DisplayRole).toString());

	    default:
	        break;
    }

	return Item::data(role);
}

QVariant AbstractHeadsUpDisplayModel::TitleItem::data(int role) const
{
    if (!getHeadsupDisplayItem())
        return Item::data(role);

    switch (role) {
	    case Qt::EditRole:
	    case Qt::DisplayRole:
	        return getHeadsupDisplayItem()->getTitle();

	    case Qt::ToolTipRole:
	        return QString("Title: %1").arg(data(Qt::DisplayRole).toString());

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant AbstractHeadsUpDisplayModel::ValueItem::data(int role) const
{
    if (!getHeadsupDisplayItem())
        return Item::data(role);

    switch (role) {
	    case Qt::EditRole:
	    case Qt::DisplayRole:
	        return getHeadsupDisplayItem()->getValue();

	    case Qt::ToolTipRole:
	        return QString("Value: %1").arg(data(Qt::DisplayRole).toString());

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant AbstractHeadsUpDisplayModel::DescriptionItem::data(int role) const
{
    if (!getHeadsupDisplayItem())
        return Item::data(role);

    switch (role) {
	    case Qt::EditRole:
	    case Qt::DisplayRole:
	        return getHeadsupDisplayItem()->getDescription();

	    case Qt::ToolTipRole:
	        return QString("Description: %1").arg(data(Qt::DisplayRole).toString());

	    default:
	        break;
    }

    return Item::data(role);
}

AbstractHeadsUpDisplayModel::AbstractHeadsUpDisplayModel(QObject* parent) :
    StandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

QVariant AbstractHeadsUpDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (static_cast<Column>(section))
    {
	    case Column::Title:
	        return TitleItem::headerData(orientation, role);

	    case Column::Value:
	        return ValueItem::headerData(orientation, role);

        case Column::Description:
            return DescriptionItem::headerData(orientation, role);

        case Column::Id:
            return IdItem::headerData(orientation, role);

		case Column::Count:
            break;
    }

    return {};
}

void AbstractHeadsUpDisplayModel::addHeadsUpDisplayItem(HeadsUpDisplayItemSharedPtr& headsUpDisplayItem)
{
    try {
        Q_ASSERT(headsUpDisplayItem);

        if (!headsUpDisplayItem)
            throw std::runtime_error("Heads-up display item shared pointer not valid");

#ifdef HEADS_UP_DISPLAY_MODEL_VERBOSE
        qDebug() << __FUNCTION__ << headsUpDisplayItem->getTitle();
#endif

        const auto parentIndex = headsUpDisplayItem->getParent() ? headsUpDisplayItem->getParent()->getIndex() : QPersistentModelIndex();

        if (parentIndex.isValid()) {
            if (auto parentItem = dynamic_cast<Item*>(itemFromIndex(parentIndex.sibling(0, 0))))
                parentItem->appendRow(Row(headsUpDisplayItem));
            else
                throw std::runtime_error("Parent index is not a valid item");
        }
        else {
            appendRow(Row(headsUpDisplayItem));

            headsUpDisplayItem->setIndex(indexFromHeadsUpDisplayItem(headsUpDisplayItem));
        }
    }
    catch (std::exception& exception)
    {
        qCritical() << QString("Unable to add %1 to the heads-up display item model:").arg(headsUpDisplayItem ? headsUpDisplayItem->getTitle() : "untitled") << exception.what();
    }
    catch (...)
    {
        qCritical() << QString("Unable to add %1 to the heads-up display item model due to an unhandled exception").arg(headsUpDisplayItem ? headsUpDisplayItem->getTitle() : "untitled");
    }
}

void AbstractHeadsUpDisplayModel::removeHeadsUpDisplayItem(const HeadsUpDisplayItemSharedPtr& headsUpDisplayItem)
{
    try {
#ifdef HEADS_UP_DISPLAY_MODEL_VERBOSE
        qDebug() << __FUNCTION__ << headsUpDisplayItem->getIndex();
#endif

        const auto index = headsUpDisplayItem->getIndex();

        if (!index.isValid())
            throw std::runtime_error("Heads-up display item index not valid");

        if (!removeRow(index.row(), index.parent()))
            throw std::runtime_error(QString("Unable to remove heads-up display item at index %1").arg(index.row()).toStdString());
    }
    catch (std::exception& exception)
    {
        qCritical() << "Unable to remove heads-up display item from the heads-up display item model:" << exception.what();
    }
    catch (...)
    {
        qCritical() << "Unable to remove heads-up display item from the heads-up display item model";
    }
}

QModelIndex AbstractHeadsUpDisplayModel::indexFromHeadsUpDisplayItem(const util::HeadsUpDisplayItemSharedPtr& headsUpDisplayItem) const
{
    if (!headsUpDisplayItem)
        return {};

    const auto idMatches = match(index(0, static_cast<std::int32_t>(Column::Id)), Qt::DisplayRole, headsUpDisplayItem->getId(), 1, Qt::MatchExactly | Qt::MatchRecursive);

    if (!idMatches.isEmpty())
        return idMatches.first();

    return {};
}

}
