#pragma once

#include "clusterdata_export.h"

#include "ClusterDataVisitor.h"

#include <QString>
#include <QDebug>
#include <QColor>

#include <vector>

/**
 * Clusters class
 *
 * Storage class for cluster data
 *
 */
class CLUSTERDATA_EXPORT Cluster : public hdps::util::Visitable<ClusterDataVisitor>
{
public:

    /** Constructor */
    Cluster();

    /**
     * Comparison operator for two clusters (compares the internal identifiers)
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

    /** Accept cluster data visitor for visiting
     * @param visitor Reference to visitor that will visit this component
     */
    void accept(ClusterDataVisitor* visitor) const override;

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
    const std::vector<unsigned int>& getIndices() const;

    /** Get contained indices */
    std::vector<unsigned int>& getIndices();

    /** Get number of indices */
    std::uint32_t getNumberOfIndices() const;

    /**
     * Set indices
     * @param indices Indices
     */
    void setIndices(const std::vector<unsigned int>& indices);

    /** Get median */
    std::vector<float>& getMedian();

    /** Get median */
    std::vector<float>& getMean();

    /** Get median */
    std::vector<float>& getStandardDeviation();

protected:
    QString                     _name;          /** GUI name */
    QString                     _id;            /** Unique cluster name */
    QColor                      _color;         /** Cluster color */
    std::vector<unsigned int>   _indices;       /** Indices contained by the cluster */
    std::vector<float>          _median;        /** Median values */
    std::vector<float>          _mean;          /** Mean values */
    std::vector<float>          _stddev;        /** Standard deviation values */
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
