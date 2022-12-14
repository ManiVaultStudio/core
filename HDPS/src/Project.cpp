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

}
