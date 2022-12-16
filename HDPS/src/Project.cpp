#include "Project.h"

#include <Application.h>

namespace hdps {

Project::Project(QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Project")
{
}

QString Project::getFilePath() const
{
    return _filePath;
}

void Project::setFilePath(const QString& filePath)
{
    _filePath = filePath;

    emit filePathChanged(_filePath);
}

void Project::fromVariantMap(const QVariantMap& variantMap)
{
    auto core = Application::core();

    auto& pluginManager         = core->getPluginManager();
    auto& dataHierarchyManager  = core->getDataHierarchyManager();
    auto& actionsManager        = core->getActionsManager();

    pluginManager.fromVariantMap(variantMap[pluginManager.getSerializationName()].toMap());
    dataHierarchyManager.fromVariantMap(variantMap[dataHierarchyManager.getSerializationName()].toMap());
    actionsManager.fromVariantMap(variantMap[actionsManager.getSerializationName()].toMap());
}

QVariantMap Project::toVariantMap() const
{
    QVariantMap variantMap;

    auto core = Application::core();

    auto& pluginManager         = core->getPluginManager();
    auto& dataHierarchyManager  = core->getDataHierarchyManager();
    auto& actionsManager        = core->getActionsManager();

    variantMap[pluginManager.getSerializationName()]        = pluginManager.toVariantMap();
    variantMap[dataHierarchyManager.getSerializationName()] = dataHierarchyManager.toVariantMap();
    variantMap[actionsManager.getSerializationName()]       = actionsManager.toVariantMap();

    return variantMap;
}

}
