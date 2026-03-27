// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "clusterdata_export.h"

#include <QString>
#include <QDebug>
#include <QColor>
#include <QRandomGenerator>

/**
 * Clusters class
 *
 * Storage class for cluster data
 *
 */
#pragma once

#include <QColor>
#include <QDataStream>
#include <QMetaType>
#include <QVector>
#include <QString>
#include <vector>

class CLUSTERDATA_EXPORT Cluster
{
public:
    Cluster(const QString& name = "",
        const QColor& color = Qt::gray,
        const std::vector<std::uint32_t>& indices = std::vector<std::uint32_t>());

public:
    QString getName() const;
    void setName(const QString& name);

    QString getId() const;
    void setId(const QString& id);

    QColor getColor() const;
    void setColor(const QColor& color);

    const std::vector<std::uint32_t>& getIndices() const;
    std::vector<std::uint32_t>& getIndices();

    std::uint32_t getNumberOfIndices() const;
    void setIndices(const std::vector<unsigned int>& indices);

    std::vector<float>& getMedian() { return _median; }
    const std::vector<float>& getMedian() const { return _median; }

    std::vector<float>& getMean() { return _mean; }
    const std::vector<float>& getMean() const { return _mean; }

    std::vector<float>& getStandardDeviation() { return _stddev; }
    const std::vector<float>& getStandardDeviation() const { return _stddev; }

    static void colorizeClusters(QVector<Cluster>& clusters, std::int32_t randomSeed = 0);
    static void colorizeClusters(QVector<Cluster>& clusters, const QImage& colorMapImage);

    Cluster copy() const;
    bool operator==(const Cluster& rhs) const
    {
	    if (rhs._name != _name)
	    	return false;

    	if (rhs._id != _id)
    		return false;

    	if (rhs._color != _color)
    		return false;

    	if (rhs._indices != _indices)
    		return false;

    	if (rhs._median != _median)
    		return false;

    	if (rhs._mean != _mean)
    		return false;

    	if (rhs._stddev != _stddev)
    		return false;

    	return true;
    }

    friend CLUSTERDATA_EXPORT QDataStream& operator<<(QDataStream& out, const Cluster& cluster);
    friend CLUSTERDATA_EXPORT QDataStream& operator>>(QDataStream& in, Cluster& cluster);

protected:
    QString                     _name;
    QString                     _id;
    QColor                      _color;
    std::vector<unsigned int>   _indices;
    std::vector<float>          _median;
    std::vector<float>          _mean;
    std::vector<float>          _stddev;
};



Q_DECLARE_METATYPE(Cluster)

/**
 * Print reference to cluster to console
 * @param debug Debug
 * @param cluster Reference to cluster
 */
inline QDebug operator << (QDebug debug, const Cluster& cluster)
{
    debug.nospace() << cluster.getName() << cluster.getColor().name(QColor::HexRgb) << cluster.getIndices();

    return debug.space();
}

/**
 * Print pointer to cluster to console
 * @param debug Debug
 * @param cluster Pointer to cluster
 */
inline QDebug operator << (QDebug debug, const Cluster* cluster)
{
    debug.nospace() << *cluster;

    return debug.space();
}
