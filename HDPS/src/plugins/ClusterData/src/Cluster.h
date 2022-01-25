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
class CLUSTERDATA_EXPORT Cluster
{
public:

    /**
     * Constructor
     * @param name Name of the cluster
     * @param color Color of the cluster
     * @param indices Indices of the cluster
     */
    Cluster(const QString& name = "", const QColor& color = Qt::gray, const std::vector<std::uint32_t>& indices = std::vector<std::uint32_t>());

public: // Getters/setters

    /** Get cluster name */
    QString getName() const;

    /** Set cluster name
     * @param name Name of the cluster
     */
    void setName(const QString& name);

    /** Get unique identifier */
    QString getId() const;

    /** Get cluster color */
    QColor getColor() const;

    /**
     * Set cluster color
     * @param color Cluster color
     */
    void setColor(const QColor& color);

    /** Get contained indices */
    const std::vector<std::uint32_t>& getIndices() const;

    /** Get contained indices */
    std::vector<std::uint32_t>& getIndices();

    /** Get number of indices */
    std::uint32_t getNumberOfIndices() const;

    /**
     * Set indices
     * @param indices Indices
     */
    void setIndices(const std::vector<unsigned int>& indices);

    /** Get median values for every dimension of the cluster */
    std::vector<float>& getMedian() { return _median; }
    const std::vector<float>& getMedian() const { return _median; }

    /** Get mean values for every dimension of the cluster */
    std::vector<float>& getMean() { return _mean; }
    const std::vector<float>& getMean() const { return _mean; }

    /** Get standard deviation values for every dimension of the cluster */
    std::vector<float>& getStandardDeviation() { return _stddev; }
    const std::vector<float>& getStandardDeviation() const { return _stddev; }

    /** Loads cluster from a variant */
    void fromVariant(const QVariant& variant);

    /** Returns a variant representation of the cluster */
    QVariant toVariant() const;

    /**
     * Colorize clusters by pseudo-random colors
     * @param clusters Vector of clusters to colorize
     * @param randomSeed Random seed for pseudo-random colors
     */
    static void colorizeClusters(QVector<Cluster>& clusters, std::int32_t randomSeed = 0);

    /**
     * Colorize clusters by color map
     * @param clusters Vector of clusters to colorize
     * @param colorMapImage Color map image
     */
    static void colorizeClusters(QVector<Cluster>& clusters, const QImage& colorMapImage);

    /**
     * Copy cluster
     * @return Copy of the cluster (copies all members, except the _id, which is re-generated)
     */
    Cluster copy() const;

    /**
     * Comparison operator for two clusters
     * @param rhs Right hand sign of the comparison
     */
    bool operator==(const Cluster& rhs) const {
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

protected:
    QString                     _name;          /** GUI name */
    QString                     _id;            /** Unique cluster name */
    QColor                      _color;         /** Cluster color */
    std::vector<unsigned int>   _indices;       /** Indices contained by the cluster */
    std::vector<float>          _median;        /** Median dimension values */
    std::vector<float>          _mean;          /** Mean dimension values */
    std::vector<float>          _stddev;        /** Standard deviation dimension values */
};

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
