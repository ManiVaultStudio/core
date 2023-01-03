#include "Workspace.h"

#include "Application.h"

namespace hdps {

Workspace::Workspace(QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Workspace"),
    _filePath(),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags")
{
    _descriptionAction.setPlaceHolderString("Enter description here...");
    _descriptionAction.setConnectionPermissionsToNone();
    _descriptionAction.setClearable(true);

    _tagsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("tag"));
    _tagsAction.setCategory("Tag");
    _tagsAction.setConnectionPermissionsToNone();
}

QString Workspace::getFilePath() const
{
    return _filePath;
}

void Workspace::setFilePath(const QString& filePath)
{
    _filePath = filePath;

    emit filePathChanged(_filePath);
}

void Workspace::fromVariantMap(const QVariantMap& variantMap)
{
    //auto core = Application::core();

    //auto& pluginManager         = core->getPluginManager();
    //auto& dataHierarchyManager  = core->getDataHierarchyManager();
    //auto& actionsManager        = core->getActionsManager();

    //pluginManager.fromVariantMap(variantMap[pluginManager.getSerializationName()].toMap());
    //dataHierarchyManager.fromVariantMap(variantMap[dataHierarchyManager.getSerializationName()].toMap());
    //actionsManager.fromVariantMap(variantMap[actionsManager.getSerializationName()].toMap());
}

QVariantMap Workspace::toVariantMap() const
{
    QVariantMap variantMap;

    //auto core = Application::core();

    //auto& pluginManager         = core->getPluginManager();
    //auto& dataHierarchyManager  = core->getDataHierarchyManager();
    //auto& actionsManager        = core->getActionsManager();

    //variantMap[pluginManager.getSerializationName()]        = pluginManager.toVariantMap();
    //variantMap[dataHierarchyManager.getSerializationName()] = dataHierarchyManager.toVariantMap();
    //variantMap[actionsManager.getSerializationName()]       = actionsManager.toVariantMap();

    return variantMap;
}

}
