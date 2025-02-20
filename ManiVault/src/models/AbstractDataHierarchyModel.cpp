// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractDataHierarchyModel.h"
#include "DatasetsMimeData.h"

#include "util/Icon.h"

#include <QDebug>
#include <QIcon>
#include <QPainter>
#include <QPainterPath>

#ifdef _DEBUG
    #define ABSTRACT_DATA_HIERARCHY_MODEL_VERBOSE
#endif

using namespace mv::gui;

namespace mv {

using namespace util;

AbstractDataHierarchyModel::Item::Item(Dataset<DatasetImpl> dataset, bool editable /*= false*/) :
    QStandardItem(),
    QObject(),
    _dataset(dataset)
{
    Q_ASSERT(_dataset.isValid());

    setEditable(editable);

    connect(&getDataset()->getDataHierarchyItem(), &DataHierarchyItem::lockedChanged, this, [this](bool locked) -> void {
        emitDataChanged();
    });
}

QVariant AbstractDataHierarchyModel::Item::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::ForegroundRole:
        {
            if (_dataset.isValid())
                break;

            return _dataset->isLocked() ? QApplication::palette().color(QPalette::Disabled, QPalette::Text).name() : QApplication::palette().color(QPalette::Normal, QPalette::Text).name();
        }

        default:
            break;
    }

    return QStandardItem::data(role);
}

Dataset<DatasetImpl>& AbstractDataHierarchyModel::Item::getDataset()
{
    return _dataset;
}

const Dataset<DatasetImpl>& AbstractDataHierarchyModel::Item::getDataset() const
{
    return _dataset;
}

AbstractDataHierarchyModel::NameItem::NameItem(Dataset<DatasetImpl> dataset) :
    Item(dataset, true)
{
    connect(&getDataset(), &Dataset<DatasetImpl>::guiNameChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant AbstractDataHierarchyModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getDataset().isValid() ? getDataset()->getGuiName() : "";

        case Qt::ToolTipRole:
            return QString("Dataset name: %1").arg(data(Qt::DisplayRole).toString());

        case Qt::DecorationRole:
        {
            if (!getDataset().isValid())
                break;

            return getDataset()->getIcon();
        }

        default:
            break;
    }

    return Item::data(role);
}

void AbstractDataHierarchyModel::NameItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
        {
            if (getDataset().isValid())
                getDataset()->setText(value.toString());

            break;
        }

        default:
            Item::setData(value, role);
    }
}

AbstractDataHierarchyModel::LocationItem::LocationItem(Dataset<DatasetImpl> dataset) :
    Item(dataset, true)
{
    connect(getDataset().get(), &WidgetAction::locationChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant AbstractDataHierarchyModel::LocationItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getDataset().isValid() ? getDataset()->getDataHierarchyItem().getLocation(true) : "";

        case Qt::ToolTipRole:
            return QString("Dataset location: %1").arg(data(Qt::DisplayRole).toString());

        case Qt::DecorationRole:
        {
            if (!getDataset().isValid())
                break;

            return getDataset()->getIcon();
        }

        default:
            break;
    }

    return Item::data(role);
}

AbstractDataHierarchyModel::DatasetIdItem::DatasetIdItem(Dataset<DatasetImpl> dataset) :
    Item(dataset)
{
    connect(getDataset().get(), &gui::WidgetAction::idChanged, this, [this](const QString& id) -> void {
        emitDataChanged();
    });

    connect(&mv::settings().getMiscellaneousSettings().getShowSimplifiedGuidsAction(), &gui::ToggleAction::toggled, this, [this](bool toggled) -> void {
        emitDataChanged();
    });
}

QVariant AbstractDataHierarchyModel::DatasetIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getDataset().isValid() ? getDataset()->getId() : "";

        case Qt::DisplayRole:
            return getDataset().isValid() ? getDataset()->getId(mv::settings().getMiscellaneousSettings().getShowSimplifiedGuidsAction().isChecked()) : "";

        case Qt::ToolTipRole:
            return "Dataset identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

AbstractDataHierarchyModel::SourceDatasetIdItem::SourceDatasetIdItem(Dataset<DatasetImpl> dataset) :
    Item(dataset)
{
    connect(&mv::settings().getMiscellaneousSettings().getShowSimplifiedGuidsAction(), &gui::ToggleAction::toggled, this, [this](bool toggled) -> void {
        emitDataChanged();
    });
}

QVariant AbstractDataHierarchyModel::SourceDatasetIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    const auto sourceDataset = getDataset()->getSourceDataset<DatasetImpl>();

    switch (role) {
        case Qt::EditRole:
            return getDataset()->isDerivedData() ? sourceDataset->getId() : "";

        case Qt::DisplayRole:
            return getDataset()->isDerivedData() ? sourceDataset->getId(mv::settings().getMiscellaneousSettings().getShowSimplifiedGuidsAction().isChecked()) : "";

        case Qt::ToolTipRole:
            return "Source dataset identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractDataHierarchyModel::RawDataNameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getDataset().isValid() ? getDataset()->getRawDataName() : "";

        case Qt::ToolTipRole:
            return "Raw data identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractDataHierarchyModel::RawDataSizeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getDataset().isValid() ? QVariant::fromValue(getDataset()->getRawDataSize()) : 0;

        case Qt::DisplayRole:
            return getDataset().isValid() ? getDataset()->getRawDataSizeHumanReadable() : "";

        case Qt::ToolTipRole:
            return "Raw data identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

AbstractDataHierarchyModel::ProgressItem::ProgressItem(Dataset<DatasetImpl> dataset) :
    Item(dataset),
    _taskAction(this, "Task")
{
    _taskAction.setTask(&getDatasetTask());

    connect(&getDatasetTask(), &Task::progressChanged, this, [this]() -> void {
        emitDataChanged();
    });

    connect(&getDatasetTask(), &Task::progressDescriptionChanged, this, [this]() -> void {
        emitDataChanged();
    });

    connect(&getDatasetTask(), &Task::statusChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant AbstractDataHierarchyModel::ProgressItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getDataset().isValid() ? const_cast<Dataset<DatasetImpl>&>(getDataset())->getTask().getProgress() : .0f;

        case Qt::DisplayRole:
            break;

        case Qt::ToolTipRole:
            return "Dataset task progress: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QWidget* AbstractDataHierarchyModel::ProgressItem::createDelegateEditorWidget(QWidget* parent)
{
    return _taskAction.getProgressAction().createWidget(parent);
}

AbstractDataHierarchyModel::SelectionGroupIndexItem::SelectionGroupIndexItem(Dataset<DatasetImpl> dataset) :
    Item(dataset, true)
{
}

QVariant AbstractDataHierarchyModel::SelectionGroupIndexItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getDataset().isValid() ? getDataset()->getGroupIndex() : -1;

        case Qt::DisplayRole:
            return QString::number(data(Qt::EditRole).toInt());

        case Qt::ToolTipRole:
            return "Selection group index: " + data(Qt::DisplayRole).toString();

        case Qt::TextAlignmentRole:
            return static_cast<std::int32_t>(Qt::AlignVCenter | Qt::AlignRight);

        default:
            break;
    }

    return Item::data(role);
}

void AbstractDataHierarchyModel::SelectionGroupIndexItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
        {
            if (getDataset().isValid())
                getDataset()->setGroupIndex(value.toInt());

            break;
        }

        default:
            Item::setData(value, role);
    }
}

AbstractDataHierarchyModel::IsVisibleItem::IsVisibleItem(Dataset<DatasetImpl> dataset) :
    Item(dataset)
{
    connect(&getDataset()->getDataHierarchyItem(), &DataHierarchyItem::visibilityChanged, this, [this](bool visibility) -> void {
        emitDataChanged();
    });
}

QVariant AbstractDataHierarchyModel::IsVisibleItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        {
            if (!getDataset().isValid())
                break;

            return getDataset()->getDataHierarchyItem().isVisible();
        }

        case Qt::DisplayRole:
            break;

        case Qt::DecorationRole:
            return StyledIcon(data(Qt::EditRole).toBool() ? "eye" : "eye-slash");

        case Qt::ToolTipRole:
            return QString("Dataset is visible: %1").arg(data(Qt::EditRole).toBool() ? "yes" : "no");

        default:
            break;
    }

    return Item::data(role);
}

void AbstractDataHierarchyModel::IsVisibleItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
        {
            if (getDataset().isValid())
                getDataset()->getDataHierarchyItem().setVisible(value.toBool());

            break;
        }

        default:
            Item::setData(value, role);
    }
}

AbstractDataHierarchyModel::IsGroupItem::IsGroupItem(Dataset<DatasetImpl> dataset) :
    Item(dataset)
{
}

QVariant AbstractDataHierarchyModel::IsGroupItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        {
            if (!getDataset().isValid())
                break;

            return getDataset()->isProxy();
        }

        case Qt::DisplayRole:
            break;

        case Qt::ToolTipRole:
            return QString("Dataset is a group: %1").arg(data(Qt::EditRole).toBool() ? "yes" : "no");

        case Qt::DecorationRole:
        {
            if (!getDataset().isValid())
                break;

            if (getDataset()->isProxy())
                return StyledIcon("object-group");

            break;
        }

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractDataHierarchyModel::IsDerivedItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        {
            if (!getDataset().isValid())
                break;

            return getDataset()->isDerivedData();
        }

        case Qt::DisplayRole:
            break;

        case Qt::ToolTipRole:
            return QString("Dataset %1 derived").arg(data(Qt::EditRole).toBool() ? "is" : "is not");

        case Qt::DecorationRole:
            return data(Qt::EditRole).toBool() ? StyledIcon("square-root-alt") : QIcon();

        default:
            break;
    }

    return Item::data(role);
}

AbstractDataHierarchyModel::IsSubsetItem::IsSubsetItem(Dataset<DatasetImpl> dataset) :
    Item(dataset)
{
    const auto pieRadius = 40.f;

    createFullIcon(pieRadius);
    createSubsetIcon(pieRadius);
}

QVariant AbstractDataHierarchyModel::IsSubsetItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        {
            if (!getDataset().isValid())
                break;

            return !getDataset()->isFull();
        }

        case Qt::DisplayRole:
            break;

        case Qt::ToolTipRole:
            return data(Qt::EditRole).toBool() ? "Subset" : "Full dataset";

        case Qt::DecorationRole:
            return data(Qt::EditRole).toBool() ? _subsetIcon : _fullIcon;

        default:
            break;
    }

    return Item::data(role);
}

void AbstractDataHierarchyModel::IsSubsetItem::createFullIcon(float pieRadius /*= 35.f*/)
{
    QPixmap pixmap(QSize(100, 100));

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    //drawExtents(painter);

    painter.setPen(QPen(Qt::black, 2.f * pieRadius, Qt::SolidLine, Qt::RoundCap));
    painter.drawPoint(QPointF(50.f, 50.f));

    _fullIcon = createIcon(pixmap);
}

void AbstractDataHierarchyModel::IsSubsetItem::createSubsetIcon(float pieRadius /*= 35.f*/)
{
    QPixmap pixmap(QSize(100, 100));

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    //drawExtents(painter);

    const auto margin = 50.f - pieRadius;

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawPie(QRectF(margin, margin, 100.f - (2.f * margin), 100.f - (2.f * margin)), 90 * 16, 270 * 16);

    _subsetIcon = createIcon(pixmap);
}

void AbstractDataHierarchyModel::IsSubsetItem::drawExtents(QPainter& painter)
{
    painter.setPen(QPen(Qt::black, 15.0, Qt::SolidLine, Qt::FlatCap));

    const auto extents  = std::vector<float>({ 0.f, 100.f });
    const auto length   = 15.f;

    for (const auto& e : extents) {
        painter.drawLine(0.f, e, length, e);
        painter.drawLine(100.f - length, e, 100.f, e);

        painter.drawLine(e, 0, e, length);
        painter.drawLine(e, 100.f - length, e, 100.f);
    }
}

QVariant AbstractDataHierarchyModel::IsLockedItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        {
            if (!getDataset().isValid())
                break;

            return getDataset()->isLocked();
        }

        case Qt::DisplayRole:
            break;

        case Qt::ToolTipRole:
            return QString("Dataset is %1").arg(data(Qt::EditRole).toBool() ? "locked" : "not locked");

        case Qt::DecorationRole:
            return data(Qt::EditRole).toBool() ? StyledIcon("lock") : StyledIcon("lock-open");

        default:
            break;
    }

    return Item::data(role);
}

AbstractDataHierarchyModel::AbstractDataHierarchyModel(QObject* parent) :
    StandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

Qt::DropActions AbstractDataHierarchyModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QVariant AbstractDataHierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Name:
            return NameItem::headerData(orientation, role);

        case Column::Location:
            return LocationItem::headerData(orientation, role);

        case Column::DatasetId:
            return DatasetIdItem::headerData(orientation, role);

        case Column::RawDataName:
            return RawDataNameItem::headerData(orientation, role);

        case Column::RawDataSize:
            return RawDataSizeItem::headerData(orientation, role);

        case Column::SourceDatasetId:
            return SourceDatasetIdItem::headerData(orientation, role);

        case Column::Progress:
            return ProgressItem::headerData(orientation, role);

        case Column::SelectionGroupIndex:
            return SelectionGroupIndexItem::headerData(orientation, role);

        case Column::IsVisible:
            return IsVisibleItem::headerData(orientation, role);

        case Column::IsGroup:
            return IsGroupItem::headerData(orientation, role);

        case Column::IsDerived:
            return IsDerivedItem::headerData(orientation, role);

        case Column::IsSubset:
            return IsSubsetItem::headerData(orientation, role);

        case Column::IsLocked:
            return IsLockedItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

QMimeData* AbstractDataHierarchyModel::mimeData(const QModelIndexList& indexes) const
{
    Datasets datasets;

    for (const auto& index : indexes)
        if (index.column() == 0)
            datasets << static_cast<Item*>(itemFromIndex(index))->getDataset();

    return new DatasetsMimeData(datasets);
}

QModelIndex AbstractDataHierarchyModel::getModelIndex(const QString& datasetId, Column column /*= Column::Name*/) const
{
    const auto matches = match(index(0, static_cast<int>(AbstractDataHierarchyModel::Column::Name)), Qt::EditRole, datasetId, 1, Qt::MatchRecursive | Qt::MatchExactly);

    if (matches.isEmpty())
        return {};

    return matches.first().siblingAtColumn(static_cast<int>(column));
}

void AbstractDataHierarchyModel::hideItem(const QModelIndex& index)
{
    setData(index.siblingAtColumn(static_cast<int>(AbstractDataHierarchyModel::Column::IsVisible)), false, Qt::EditRole);

    for (int rowIndex = 0; rowIndex < rowCount(index.parent().siblingAtColumn(0)); rowIndex++)
        hideItem(this->index(rowIndex, 0, index.parent()));
}

void AbstractDataHierarchyModel::unhideItem(const QModelIndex& index)
{
    setData(index.siblingAtColumn(static_cast<int>(AbstractDataHierarchyModel::Column::IsVisible)), true, Qt::EditRole);

    if (index.parent().isValid())
        unhideItem(index.parent());
}

}
