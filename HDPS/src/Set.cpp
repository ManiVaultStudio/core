#include "Set.h"
#include "DataHierarchyItem.h"
#include "AnalysisPlugin.h"

#include <util/Serialization.h>
#include <util/Icon.h>

#include <QPainter>

using namespace hdps::gui;

namespace hdps
{

void DatasetImpl::makeSubsetOf(Dataset<DatasetImpl> fullDataset)
{
    _rawDataName = fullDataset->_rawDataName;

    if (!_rawDataName.isEmpty())
        _rawData = &_core->requestRawData(getRawDataName());

    setAll(false);
}

QString DatasetImpl::getRawDataKind() const
{
    return _rawData->getKind();
}

const DataHierarchyItem& DatasetImpl::getDataHierarchyItem() const
{
    return const_cast<DatasetImpl*>(this)->getDataHierarchyItem();
}

DataHierarchyItem& DatasetImpl::getDataHierarchyItem()
{
    return _core->getDataHierarchyItem(_guid);
}

hdps::Dataset<hdps::DatasetImpl> DatasetImpl::getParent() const
{
    return getDataHierarchyItem().getParent().getDataset();
}

QVector<Dataset<DatasetImpl>> DatasetImpl::getChildren(const QVector<DataType>& dataTypes /*= QVector<DataType>()*/) const
{
    // Found children
    QVector<Dataset<DatasetImpl>> children;

    // Loop over all data hierarchy children and add to the list if occur in the data types
    for (auto dataHierarchyChild : getDataHierarchyItem().getChildren())
        if (dataTypes.contains(dataHierarchyChild->getDataType()))
            children << dataHierarchyChild->getDataset();

    return children;
}

std::int32_t DatasetImpl::getSelectionSize() const
{
    return static_cast<std::int32_t>(const_cast<DatasetImpl*>(this)->getSelectionIndices().size());
}

void DatasetImpl::lock()
{
    setLocked(true);
}

void DatasetImpl::unlock()
{
    setLocked(false);
}

bool DatasetImpl::isLocked() const
{
    return getDataHierarchyItem().getLocked();
}

void DatasetImpl::setLocked(bool locked)
{
    getDataHierarchyItem().setLocked(locked);
}

void DatasetImpl::setAnalysis(plugin::AnalysisPlugin* analysis)
{
    _analysis = analysis;
}

hdps::plugin::AnalysisPlugin* DatasetImpl::getAnalysis()
{
    return _analysis;
}

void DatasetImpl::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "Name");
    variantMapMustContain(variantMap, "Derived");
    variantMapMustContain(variantMap, "HasAnalysis");
    variantMapMustContain(variantMap, "Analysis");

    setGuiName(variantMap["Name"].toString());

    if (variantMap.contains("Derived")) {
        _derived = variantMap["Derived"].toBool();

        if (_derived)
            _sourceDataset = getParent();
    }

    auto analysisMap = variantMap["Analysis"];

}

QVariantMap DatasetImpl::toVariantMap() const
{
    QVariantMap analysisMap;

    if (_analysis)
        analysisMap = _analysis->toVariantMap();

    return {
        { "Name", getGuiName() },
        { "DataType", getDataType().getTypeString() },
        { "PluginKind", _rawData->getKind() },
        { "PluginVersion", _rawData->getVersion() },
        { "Derived", isDerivedData() },
        { "GroupIndex", getGroupIndex() },
        { "HasAnalysis", _analysis != nullptr },
        { "Analysis", analysisMap }
    };
}

std::int32_t DatasetImpl::getGroupIndex() const
{
    return _groupIndex;
}

void DatasetImpl::setGroupIndex(const std::int32_t& groupIndex)
{
    _groupIndex = groupIndex;

    _core->notifyDatasetSelectionChanged(this);
}

hdps::Datasets DatasetImpl::getProxyDatasets() const
{
    return _proxyDatasets;
}

void DatasetImpl::setProxyDatasets(const Datasets& proxyDatasets)
{
    try
    {
        if (proxyDatasets.isEmpty())
            throw std::runtime_error("Proxy dataset requires at least one input dataset");

        for (auto proxyDataset : proxyDatasets)
            if (proxyDataset->getDataType() != getDataType())
                throw std::runtime_error("All datasets should be of the same data type");

        _proxyDatasets = proxyDatasets;

        setStorageType(StorageType::Proxy);

        _core->notifyDatasetChanged(this);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to set proxy datasets for " + getGuiName(), e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to set proxy datasets for " + getGuiName());
    }
}

bool DatasetImpl::mayProxy(const Datasets& proxyDatasets) const
{
    for (auto proxyDataset : proxyDatasets)
        if (proxyDataset->getDataType() != getDataType())
            return false;

    return true;
}

bool DatasetImpl::isProxy() const
{
    return !_proxyDatasets.isEmpty();
}

void DatasetImpl::addAction(hdps::gui::WidgetAction& widgetAction)
{
    // Re-parent the widget action
    widgetAction.setParent(this);

   // And add to the data hierarchy item
    getDataHierarchyItem().addAction(widgetAction);
}

hdps::gui::WidgetActions DatasetImpl::getActions() const
{
    return getDataHierarchyItem().getActions();
}

QMenu* DatasetImpl::getContextMenu(QWidget* parent /*= nullptr*/)
{
    return getDataHierarchyItem().getContextMenu(parent);
}

void DatasetImpl::populateContextMenu(QMenu* contextMenu)
{
    return getDataHierarchyItem().populateContextMenu(contextMenu);
}

hdps::DatasetImpl::StorageType DatasetImpl::getStorageType() const
{
    return _storageType;
}

void DatasetImpl::setStorageType(const StorageType& storageType)
{
    if (storageType == _storageType)
        return;

    _storageType = storageType;

    getDataHierarchyItem().setIconByName("data", getIcon(_storageType));
}

QIcon DatasetImpl::getIcon(StorageType storageType, const QColor& color /*= Qt::black*/) const
{
    switch (storageType)
    {
        case StorageType::Owner:
            return getIcon(color);

        case StorageType::Proxy:
        {
            auto backgroundColor = color, foregroundColor = color;

            const auto maxContrast = 0.6f;

            backgroundColor.setHslF(color.hueF(), color.saturationF(), 0.4f);
            foregroundColor.setHslF(color.hueF(), color.saturationF(), 0.1f);

            
            const QSize size(64, 64);

            QPixmap pixmap(size);

            pixmap.fill(Qt::transparent);

            QPainter painter(&pixmap);

            const auto rectangle = QRect(QPoint(), QSize(64, 64));

            /**/
            //const auto badgeLocation = QPoint(20, 20);

            QVector<QPoint> points {
                rectangle.center()
            };

            //painter.setPen(QPen(foregroundColor, 40.0, Qt::SolidLine, Qt::RoundCap));
            //painter.drawPoints(points);
            
            //painter.drawPixmap(0, 0, Application::getIconFont("FontAwesome").getIcon("long-arrow-alt-right", foregroundColor).pixmap(QSize(64, 64)));

            auto font = QFont("Arial", 36);

            font.setBold(true);

            painter.setFont(font);
            painter.setPen(foregroundColor);
            //painter.drawText(rectangle, "*", QTextOption(Qt::AlignCenter));

            return createOverlayIcon(getIcon(backgroundColor), pixmap);
            //return createOverlayIcon(getIcon(backgroundColor), );
        }

        default:
            break;
    }

    return QIcon();
}

}
