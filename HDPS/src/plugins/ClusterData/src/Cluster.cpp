#include "Cluster.h"

#include <QUuid>

#include <stdexcept>

using namespace hdps::util;

Cluster::Cluster() :
    _name(""),
    _id(QUuid::createUuid().toString()),
    _color(Qt::gray),
    _indices(),
    _median(),
    _mean(),
    _stddev()
{
}

void Cluster::accept(ClusterDataVisitor* visitor) const
{
    visitor->visitCluster(this);
}

QString Cluster::getName() const
{
    return _name;
}

void Cluster::setName(const QString& name)
{
    Q_ASSERT(!name.isEmpty());

    _name = name;
}

QString Cluster::getId() const
{
    return _id;
}

QColor Cluster::getColor() const
{
    return _color;
}

void Cluster::setColor(const QColor& color)
{
    _color = color;
}

const std::vector<unsigned int>& Cluster::getIndices() const
{
    return const_cast<Cluster*>(this)->getIndices();
}

std::vector<unsigned int>& Cluster::getIndices()
{
    return _indices;
}

std::uint32_t Cluster::getNumberOfIndices() const
{
    return static_cast<std::uint32_t>(_indices.size());
}

void Cluster::setIndices(const std::vector<unsigned int>& indices)
{
    Q_ASSERT(!indices.empty());

    _indices = indices;
}

std::vector<float>& Cluster::getMedian()
{
    return _median;
}

std::vector<float>& Cluster::getMean()
{
    return _mean;
}

std::vector<float>& Cluster::getStandardDeviation()
{
    return _stddev;
}

void Cluster::fromVariant(const QVariant& variant)
{
    if (variant.type() != QVariant::Type::Map)
        throw std::runtime_error("Cluster variant is not a map");

    const auto variantMap = variant.toMap();

    // Serializable attributes
    QStringList attributeNames({ "name" , "id", "color", "indices"});

    // Check if all attributes are present
    for (auto attributeName : attributeNames)
        if (!variantMap.contains(attributeName))
            throw std::runtime_error(QString("Attribute %1 not found").arg(attributeName).toLatin1());

    // Populate cluster with attributes
    _name       = variantMap["name"].toString();
    _id         = variantMap["id"].toString();
    _color      = variantMap["color"].value<QColor>();

    // Convert to standard vector and assign
    const auto indicesList = variantMap["indices"].toList();

    // Reshape the indices
    _indices.clear();
    _indices.reserve(indicesList.count());

    // Populate indices
    for (auto index : indicesList)
        _indices.push_back(index.toInt());
}

QVariant Cluster::toVariant() const
{
    QVariantMap clusterVariantMap;

    clusterVariantMap["name"]       = _name;
    clusterVariantMap["id"]         = _id;
    clusterVariantMap["color"]      = _color;
    clusterVariantMap["indices"]    = QVariantList(_indices.begin(), _indices.end());

    return clusterVariantMap;
}
