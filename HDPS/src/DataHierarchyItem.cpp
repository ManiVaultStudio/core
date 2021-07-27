#include "DataHierarchyItem.h"
#include "Set.h"
#include "ExportDataAction.h"
#include "AnalyzeDataAction.h"

#include <QDebug>

namespace hdps
{

Core* DataHierarchyItem::core = nullptr;

DataHierarchyItem::DataHierarchyItem(const QString& datasetName /*= ""*/, DataHierarchyItem* parent /*= nullptr*/) :
    QObject(parent),
    _parent(parent),
    _children(),
    _datasetName(datasetName),
    _dataset(nullptr),
    _analyzing(false),
    _progressSection(),
    _progressPercentage(0.0f)
{
    if (!datasetName.isEmpty()) {
        try
        {
            _dataset = dynamic_cast<DataSet*>(&core->requestData(_datasetName));
        }
        catch (const std::exception&)
        {
        }
    }
}

DataHierarchyItem::~DataHierarchyItem()
{
    qDeleteAll(_children);
}

void DataHierarchyItem::addChild(DataHierarchyItem* item)
{
    item->setParent(this);
    _children.append(item);
}

DataHierarchyItem* DataHierarchyItem::getParent()
{
    return _parent;
}

void DataHierarchyItem::setParent(DataHierarchyItem* parent)
{
    _parent = parent;
}

DataHierarchyItem* DataHierarchyItem::getChild(const std::int32_t& row)
{
    if (row < 0 || row >= _children.size())
        return nullptr;

    return _children[row];
}

std::int32_t DataHierarchyItem::row() const
{
    if (_parent)
        return _parent->_children.indexOf(const_cast<DataHierarchyItem*>(this));

    return 0;
}

std::int32_t DataHierarchyItem::getNumChildren() const
{
    return _children.count();
}

std::int32_t DataHierarchyItem::getNumColumns() const
{
    return static_cast<std::int32_t>(Column::_end) + 1;
}

QString DataHierarchyItem::serialize() const
{
    return _dataset->getName() + "\n" + _dataset->getDataType();
}

QString DataHierarchyItem::getDataAtColumn(const std::uint32_t& column) const
{
    if (_datasetName.isEmpty())
        return "Data";

    Q_ASSERT(_dataset != nullptr);

    switch (static_cast<Column>(column))
    {
        case Column::Name:
            return _dataset->getName();

        case Column::Description:
            return _progressSection;

        case Column::Analyzing:
            return "";

        case Column::Progress:
            return _analyzing ? QString("%1%").arg(QString::number(100.0f * _progressPercentage, 'f', 1)) : "";

        default:
            break;
    }

    return "";
}

QIcon DataHierarchyItem::getIconAtColumn(const std::uint32_t& column) const
{
    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    switch (static_cast<Column>(column))
    {
        case Column::Name:
            return fontAwesome.getIcon(_datasetName.isEmpty() ? "home" : "database");

        case Column::Progress:
        case Column::Description:
            break;

        case Column::Analyzing:
        {
            if (_analyzing)
                return fontAwesome.getIcon("microchip");

            break;
        }

        default:
            break;
    }

    return QIcon();
}

QMenu* DataHierarchyItem::getContextMenu()
{
    if (_datasetName.isEmpty())
        return new QMenu();

    Q_ASSERT(_dataset != nullptr);

    return _dataset->getContextMenu();
}

QString DataHierarchyItem::getDatasetName() const
{
    return _datasetName;
}

void DataHierarchyItem::setAnalyzing(const bool& analyzing)
{
    _analyzing = analyzing;

    if (!_analyzing)
        _progressSection = "";
}

void DataHierarchyItem::setProgressPercentage(const float& progressPercentage)
{
    _progressPercentage = progressPercentage;
}

void DataHierarchyItem::setProgressSection(const QString& progressSection)
{
    _progressSection = progressSection;
}

}