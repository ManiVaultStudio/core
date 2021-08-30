#pragma once

#include "clusterdata_export.h"

#include "ClusterVisitor.h"

#include <QString>
#include <QColor>
#include <QUuid>

#include <vector>

struct CLUSTERDATA_EXPORT Cluster
{
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

    virtual void accept(ClusterVisitor& clusterVisitor);

    QString                     _name;          /** GUI name */
    QString                     _id;            /** Unique cluster name */
    QColor                      _color;         /** Cluster color */
    std::vector<unsigned int>   _indices;       /** Indices contained by the cluster */
    std::vector<float>          _median;        /** Median values */
    std::vector<float>          _mean;          /** Mean values */
    std::vector<float>          _stddev;        /** Standard deviation values */
};
