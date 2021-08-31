#pragma once

#include "util/Visitor.h"

class Cluster;

/**
 * Cluster data visitor class
 *
 * Abstract base class for cluster data visitor
 *
 * @author Thomas Kroes
 */
class ClusterDataVisitor : public hdps::util::Visitor {
public:

    /**
     * Visit cluster
     * @param cluster Cluster to visit
     */
    virtual void visitCluster(const Cluster& cluster) const = 0;
};
