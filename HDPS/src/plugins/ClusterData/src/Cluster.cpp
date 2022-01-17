#include "Cluster.h"

#include <QUuid>
#include <QImage>

#include <stdexcept>

Cluster::Cluster(const QString& name /*= ""*/, const QColor& color /*= Qt::gray*/, const std::vector<std::uint32_t>& indices /*= std::vector<std::uint32_t>()*/) :
    _name(name),
    _id(QUuid::createUuid().toString()),
    _color(color),
    _indices(indices),
    _median(),
    _mean(),
    _stddev()
{

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

void Cluster::colorizeClusters(QVector<Cluster>& clusters, std::int32_t randomSeed /*= 0*/)
{
    QRandomGenerator rng;

    // Seed the random number generator
    rng.seed(randomSeed);

    // Generate pseudo-random cluster colors
    for (auto& cluster : clusters) {
        const auto randomHue        = rng.bounded(360);
        const auto randomSaturation = rng.bounded(150, 255);
        const auto randomLightness  = rng.bounded(50, 200);

        // Create random color from hue, saturation and lightness
        cluster.setColor(QColor::fromHsl(randomHue, randomSaturation, randomLightness));
    }
}

void Cluster::colorizeClusters(QVector<Cluster>& clusters, const QImage& colorMapImage)
{
    // Get scaled version of the color map image that matches the width to the number of clusters
    const auto& scaledColorMapImage = colorMapImage.scaled(static_cast<std::int32_t>(clusters.size()), 1);

    // Color clusters according to the color map image
    for (auto& cluster : clusters)
        cluster.setColor(scaledColorMapImage.pixel(clusters.indexOf(cluster), 0));
}