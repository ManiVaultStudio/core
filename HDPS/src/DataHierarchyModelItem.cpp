#include "DataHierarchyModelItem.h"
#include "Set.h"
#include "ExportDataAction.h"
#include "AnalyzeDataAction.h"

#include <QDebug>

namespace hdps
{

Core* DataHierarchyModelItem::core = nullptr;

DataHierarchyModelItem::DataHierarchyModelItem(const QString& datasetName /*= ""*/, DataHierarchyModelItem* parent /*= nullptr*/) :
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

DataHierarchyModelItem::~DataHierarchyModelItem()
{
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
    return _dataset->getName() + "\n" + _dataset->getDataType();
}

QString DataHierarchyModelItem::getDataAtColumn(const std::uint32_t& column) const
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

QIcon DataHierarchyModelItem::getIconAtColumn(const std::uint32_t& column) const
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

QMenu* DataHierarchyModelItem::getContextMenu()
{
    if (_datasetName.isEmpty())
        return new QMenu();

    Q_ASSERT(_dataset != nullptr);

    auto contextMenu = _dataset->getContextMenu();

    /*
    // Extract name of item that triggered the context menu action
    QAction* act = qobject_cast<QAction*>(sender());

    QString datasetName = act->data().toString();

    // Pop up a dialog where the user can enter a new name
    bool ok;

    QString newDatasetName = QInputDialog::getText(this, tr("Rename Dataset"), tr("Dataset name:"), QLineEdit::Normal, datasetName, &ok);

    if (ok && !newDatasetName.isEmpty())
        _core->getDataManager().renameSet(datasetName, newDatasetName);
    */

    return contextMenu;
}

QString DataHierarchyModelItem::getDatasetName() const
{
    return _datasetName;
}

void DataHierarchyModelItem::setAnalyzing(const bool& analyzing)
{
    _analyzing = analyzing;

    if (!_analyzing)
        _progressSection = "";
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