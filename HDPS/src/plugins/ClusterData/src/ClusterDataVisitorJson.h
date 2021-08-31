#pragma once

#include "ClusterDataVisitor.h"

/**
 * JSON visitor data visitor class
 *
 * Concrete class for JSON cluster visiting
 *
 * @author Thomas Kroes
 */
class ClusterDataJsonVisitor : public ClusterDataVisitor {
public:

    /**
     * Visit cluster
     * @param cluster Cluster to visit
     */
    void visitCluster(const Cluster& cluster) const override;
};
