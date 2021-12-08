#include "DataHierarchyModelItem.h"
#include "Set.h"
#include "Application.h"
#include "DataHierarchyManager.h"

#include <QDebug>
#include <QPainter>

namespace hdps
{

QRandomGenerator DataHierarchyModelItem::rng = QRandomGenerator();

DataHierarchyModelItem::DataHierarchyModelItem(DataHierarchyItem* dataHierarchyItem, DataHierarchyModelItem* parent /*= nullptr*/) :
    QObject(parent),
    _parent(parent),
    _children(),
    _dataHierarchyItem(dataHierarchyItem),
    _progressSection(),
    _progressPercentage(0.0f)
{
}

DataHierarchyModelItem::~DataHierarchyModelItem()
{
    if (_parent)
        _parent->removeChild(this);

    qDeleteAll(_children);
}

void DataHierarchyModelItem::addChild(DataHierarchyModelItem* item)
{
    item->setParent(this);
    _children.append(item);
}

DataHierarchyModelItem* DataHierarchyModelItem::getParent()
{
    return _parent;
}

void DataHierarchyModelItem::setParent(DataHierarchyModelItem* parent)
{
    _parent = parent;
}

DataHierarchyModelItem* DataHierarchyModelItem::getChild(const std::int32_t& row)
{
    if (row < 0 || row >= _children.size())
        return nullptr;

    return _children[row];
}

std::int32_t DataHierarchyModelItem::row() const
{
    if (_parent)
        return _parent->_children.indexOf(const_cast<DataHierarchyModelItem*>(this));

    return 0;
}

std::int32_t DataHierarchyModelItem::getNumChildren() const
{
    return _children.count();
}

std::int32_t DataHierarchyModelItem::getNumColumns() const
{
    return static_cast<std::int32_t>(Column::_end) + 1;
}

QString DataHierarchyModelItem::serialize() const
{
    if (_dataHierarchyItem == nullptr)
        return "";

    return _dataHierarchyItem->getGuiName() + "\n" + _dataHierarchyItem->getDataset()->getGuid() + "\n" + _dataHierarchyItem->getDataType();
}

QVariant DataHierarchyModelItem::getDataAtColumn(const std::uint32_t& column, int role /*= Qt::DisplayRole*/) const
{
    // Only proceed if we have a valid data hierarchy item
    if (_dataHierarchyItem == nullptr)
        return QVariant();

    switch (role)
    {
        // String representation
        case Qt::DisplayRole:
        {
            // Get edit value role for column
            const auto editValue = getDataAtColumn(column, Qt::EditRole);

            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    return editValue;

                case Column::GUID:
                    return editValue;

                case Column::GroupIndex:
                    return editValue.toInt() >= 0 ? editValue : "";

                case Column::Description:
                    return editValue;

                case Column::Analysis:
                    return editValue;

                case Column::Progress:
                    return _dataHierarchyItem->isRunning() ? QString("%1%").arg(QString::number(editValue.toFloat(), 'f', 1)) : "";

                case Column::Analyzing:
                    return editValue;

                case Column::Locked:
                    return "";

                default:
                    break;
            }

            break;
        }

        // Actual value
        case Qt::EditRole:
        {
            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    return _dataHierarchyItem->getGuiName();

                case Column::GUID:
                    return _dataHierarchyItem->getDataset()->getGuid();

                case Column::GroupIndex:
                    return _dataHierarchyItem->getDataset()->getGroupIndex();

                case Column::Description:
                    return _progressSection;

                case Column::Analysis:
                    return "";

                case Column::Progress:
                    return _dataHierarchyItem->isRunning() ? 100.0f * _progressPercentage : 0.0f;

                case Column::Analyzing:
                    return "";

                case Column::Locked:
                    return _dataHierarchyItem->getLocked();

                default:
                    break;
            }

            break;
        }

        // For icons
        case Qt::DecorationRole:
        {
            // Get reference to Font Awesome icon font
            auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    return _dataHierarchyItem->getIconByName("data");

                case Column::GUID:
                    break;

                case Column::GroupIndex:
                {
                    break;

                    // Get the group index of the dataset
                    const auto groupIndex = getDataAtColumn(column, Qt::EditRole).toInt();

                    if (groupIndex < 0)
                        break;

                    const auto size = QSize(100, 100);

                    QPixmap pixmap(size);

                    pixmap.fill(Qt::transparent);

                    const auto iconRectangle = QRect(0, 0, size.width(), size.height());

                    QPainter painter(&pixmap);

                    painter.setRenderHint(QPainter::Antialiasing);

                    // Seed the random number generator with the group index
                    rng.seed(groupIndex);

                    const auto randomHue        = rng.bounded(360);
                    const auto randomSaturation = rng.bounded(150, 255);
                    const auto randomLightness  = rng.bounded(50, 200);

                    painter.setBrush(QColor::fromHsl(randomHue, randomSaturation, randomLightness));
                    painter.drawEllipse(iconRectangle);

                    painter.setPen(Qt::white);
                    painter.setFont(QFont("Arial", 50, 200));
                    painter.drawText(iconRectangle, QString::number(groupIndex), QTextOption(Qt::AlignCenter));

                    return QIcon(pixmap);
                }

                case Column::Analysis:
                    return _dataHierarchyItem->getIconByName("analysis");

                case Column::Progress:
                case Column::Description:
                    break;

                case Column::Analyzing:
                {
                    if (_dataHierarchyItem->isRunning())
                        return fontAwesome.getIcon("microchip");

                    break;
                }

                case Column::Locked:
                {
                    if (_dataHierarchyItem->getDataset()->isLocked())
                        return fontAwesome.getIcon("lock");

                    break;
                }

                default:
                    break;
            }

            break;
        }

        // Grayed out text when locked
        case Qt::TextAlignmentRole:
        {
            switch (static_cast<Column>(column))
            {
                case Column::Name:
                case Column::GUID:
                    break;

                case Column::GroupIndex:
                    return static_cast<std::int32_t>(Qt::AlignVCenter | Qt::AlignRight);

                case Column::Analysis:
                case Column::Progress:
                case Column::Description:
                case Column::Analyzing:
                case Column::Locked:
                    break;

                default:
                    break;
            }

            break;
        }

        // Grayed out text when locked
        case Qt::ForegroundRole:
            return _dataHierarchyItem->getLocked() ? QColor(Qt::gray) : QColor(Qt::black);

        default:
            break;
    }

    return QVariant();
}

QMenu* DataHierarchyModelItem::getContextMenu()
{
    if (_dataHierarchyItem == nullptr || _dataHierarchyItem->getGuiName().isEmpty())
        return new QMenu();

    return _dataHierarchyItem->getContextMenu();
}

void DataHierarchyModelItem::renameDataset(const QString& intendedDatasetName)
{
    _dataHierarchyItem->renameDataset(intendedDatasetName);
}

void DataHierarchyModelItem::setGroupIndex(const std::int32_t& groupIndex)
{
    _dataHierarchyItem->getDataset()->setGroupIndex(groupIndex);
}

void DataHierarchyModelItem::removeChild(DataHierarchyModelItem* dataHierarchyModelItem)
{
    _children.removeOne(dataHierarchyModelItem);
}

void DataHierarchyModelItem::setProgressPercentage(const float& progressPercentage)
{
    _progressPercentage = progressPercentage;
}

void DataHierarchyModelItem::setProgressSection(const QString& progressSection)
{
    _progressSection = progressSection;
}

}
