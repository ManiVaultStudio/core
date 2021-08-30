#pragma once

#include "ClusterVisitor.h"

class ClusterVisitorJson : public ClusterVisitor
{
public:
    virtual void visit(const Cluster& cluster) override
    {
        // Do something with cluster
    };
};