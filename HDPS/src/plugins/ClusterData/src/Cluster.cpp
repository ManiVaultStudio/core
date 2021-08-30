#include "Cluster.h"

Cluster::Cluster() :
    _name(""),
    _id(QUuid::createUuid().toString()),
    _color(Qt::gray),
    _median(),
    _mean(),
    _stddev()
{
}

void Cluster::accept(ClusterVisitor& clusterVisitor)
{
    clusterVisitor.visit(*this);
}
