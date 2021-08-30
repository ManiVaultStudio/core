#pragma once

struct Cluster;

class ClusterVisitor
{
public:
    virtual void visit(const Cluster& cluster) = 0;
};