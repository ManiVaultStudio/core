#include "Project.h"

#include "Application.h"

#include "util/Serialization.h"

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

Project::Project(QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Project"),
    _filePath(),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments")
{
    initialize();
}

Project::Project(const QString& filePath, QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Project"),
    _filePath(filePath),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments")
{
    initialize();

    try {
        if (!QFileInfo(getFilePath()).exists())
            throw std::runtime_error("File does not exist");

        QFile projectJsonFile(getFilePath());

        if (!projectJsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray projectByteArray = projectJsonFile.readAll();

        QJsonDocument jsonDocument;

        jsonDocument = QJsonDocument::fromJson(projectByteArray);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        fromVariantMap(jsonDocument.toVariant().toMap()["Project"].toMap());
    }
    catch (std::exception& e)
    {
        qDebug() << "Unable to load project from file" << e.what();
    }
    catch (...)
    {
        qDebug() << "Unable to load project from file";
    }
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
    variantMapMustContain(variantMap, "Description");
    variantMapMustContain(variantMap, "Tags");
    variantMapMustContain(variantMap, "Comments");

    _descriptionAction.fromVariantMap(variantMap["Description"].toMap());
    _tagsAction.fromVariantMap(variantMap["Tags"].toMap());
    _commentsAction.fromVariantMap(variantMap["Comments"].toMap());

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

    return {
        { pluginManager.getSerializationName(), pluginManager.toVariantMap() },
        { dataHierarchyManager.getSerializationName(), dataHierarchyManager.toVariantMap() },
        { actionsManager.getSerializationName(), actionsManager.toVariantMap() },
        { "Description", _descriptionAction.toVariantMap() },
        { "Tags", _tagsAction.toVariantMap() },
        { "Comments", _commentsAction.toVariantMap() },
    };
}

void Project::initialize()
{
    _descriptionAction.setPlaceHolderString("Enter project description here...");
    _descriptionAction.setConnectionPermissionsToNone();
    _descriptionAction.setClearable(true);

    _tagsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("tag"));
    _tagsAction.setCategory("Tag");
    _tagsAction.setConnectionPermissionsToNone();

    _commentsAction.setPlaceHolderString("Enter project comments here...");
    _commentsAction.setConnectionPermissionsToNone();
    _commentsAction.setClearable(true);
    _commentsAction.setDefaultWidgetFlags(StringAction::TextEdit);
}

}
