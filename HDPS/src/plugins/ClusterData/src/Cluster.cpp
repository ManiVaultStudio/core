// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Cluster.h"

#include <QUuid>
#include <QImage>

#include <stdexcept>

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
