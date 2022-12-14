#include "Project.h"

#include <Application.h>

using namespace hdps;

void Project::fromVariantMap(const QVariantMap& variantMap)
{
    auto core = Application::core();

    //core->getPluginManager()->fromVariantMap(variantMap[core->getPluginManager()->getSerializationName()].toMap());
    //_dataHierarchyManager->fromVariantMap(variantMap[_dataHierarchyManager->getSerializationName()].toMap());
}

QVariantMap Project::toVariantMap() const
{
    QVariantMap variantMap;

    //variantMap[_pluginManager->getSerializationName()] = _pluginManager->toVariantMap();
    //variantMap[_dataHierarchyManager->getSerializationName()] = _dataHierarchyManager->toVariantMap();

    return variantMap;
}
