// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Cluster.h"

#include <QUuid>
#include <QImage>

#include <stdexcept>

namespace
{

    template <typename T>
    QDataStream& writeStdVector(QDataStream& out, const std::vector<T>& values)
    {
        static_assert(std::is_arithmetic_v<T>, "writeStdVector only supports arithmetic types here.");

        out << static_cast<quint32>(values.size());

        for (const auto& value : values)
            out << value;

        return out;
    }

    template <typename T>
    QDataStream& readStdVector(QDataStream& in, std::vector<T>& values)
    {
        static_assert(std::is_arithmetic_v<T>, "readStdVector only supports arithmetic types here.");

        quint32 size = 0;
        in >> size;

        if (in.status() != QDataStream::Ok) {
            values.clear();
            return in;
        }

        values.resize(static_cast<std::size_t>(size));

        for (quint32 i = 0; i < size; ++i)
            in >> values[static_cast<std::size_t>(i)];

        return in;
    }

} // namespace

QDataStream& operator<<(QDataStream& out, const Cluster& cluster)
{
    // Simple format version for forward compatibility
    //constexpr quint32 kClusterStreamVersion = 1;
    //out << kClusterStreamVersion;

    out << cluster._name;
    out << cluster._id;
    out << cluster._color;

    writeStdVector(out, cluster._indices);
    writeStdVector(out, cluster._median);
    writeStdVector(out, cluster._mean);
    writeStdVector(out, cluster._stddev);

    return out;

}

QDataStream& operator>>(QDataStream& in, Cluster& cluster)
{
    //quint32 version = 0;
    //in >> version;

    if (in.status() != QDataStream::Ok)
        return in;

    //qDebug() << "=====" << version;
    //switch (version) {
    //case 1:
    in >> cluster._name;
    in >> cluster._id;
    in >> cluster._color;

    readStdVector(in, cluster._indices);
    readStdVector(in, cluster._median);
    readStdVector(in, cluster._mean);
    readStdVector(in, cluster._stddev);
    //    break;

    //default:
    //    in.setStatus(QDataStream::ReadCorruptData);
    //    break;
    //}

    //qDebug() << "=====" << in.status();

    return in;
}

Cluster::Cluster(const QString& name /*= ""*/, const QColor& color /*= Qt::gray*/, const std::vector<std::uint32_t>& indices /*= std::vector<std::uint32_t>()*/) :
    _name(name),
    _id(QUuid::createUuid().toString(QUuid::WithoutBraces)),
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

void Cluster::setId(const QString& id)
{
    _id = id;
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
    _indices = indices;
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

Cluster Cluster::copy() const
{
    return Cluster(_name, _color, _indices);
}
