#include "Cluster.h"

#include <QUuid>

using namespace hdps::util;

Cluster::Cluster() :
    _name(""),
    _id(QUuid::createUuid().toString()),
    _color(Qt::gray),
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
