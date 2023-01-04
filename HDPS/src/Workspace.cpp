#include "Workspace.h"
#include "AbstractWorkspaceManager.h"

#include "Application.h"

using namespace hdps::gui;

namespace hdps {

Workspace::Workspace(QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Workspace"),
    _filePath(),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments")
{
    initialize();
}

Workspace::Workspace(const QString& filePath, QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Workspace"),
    _filePath(filePath),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments")
{
    initialize();

    try {
        if (!QFileInfo(getFilePath()).exists())
            throw std::runtime_error("File does not exist");

        QFile workspaceJsonFile(getFilePath());

        if (!workspaceJsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray workspaceByteArray = workspaceJsonFile.readAll();

        QJsonDocument jsonDocument;

        jsonDocument = QJsonDocument::fromJson(workspaceByteArray);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        fromVariantMap(jsonDocument.toVariant().toMap());
    }
    catch (std::exception& e)
    {
        qDebug() << "Unable to load a workspace from file" << e.what();
    }
    catch (...)
    {
        qDebug() << "Unable to load a workspace from file";
    }
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

QImage Workspace::getPreviewImage(const QString& filePath)
{
    QImage previewImage;

    try {
        if (!QFileInfo(filePath).exists())
            throw std::runtime_error("File does not exist");

        QFile workspaceJsonFile(filePath);

        if (!workspaceJsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray workspaceByteArray = workspaceJsonFile.readAll();

        QJsonDocument jsonDocument;

        jsonDocument = QJsonDocument::fromJson(workspaceByteArray);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        const auto workspaceVariantMap = jsonDocument.toVariant().toMap()["Workspace"].toMap();

        previewImage.loadFromData(QByteArray::fromBase64(workspaceVariantMap["PreviewImage"].toByteArray()));

        return previewImage;
    }
    catch (std::exception& e)
    {
        qDebug() << "Unable to retrieve preview image from workspace file" << e.what();
    }
    catch (...)
    {
        qDebug() << "Unable to retrieve preview image from workspace file";
    }

    return previewImage;
}

void Workspace::initialize()
{
    _descriptionAction.setPlaceHolderString("Enter description here...");
    _descriptionAction.setConnectionPermissionsToNone();
    _descriptionAction.setClearable(true);

    _tagsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("tag"));
    _tagsAction.setCategory("Tag");
    _tagsAction.setConnectionPermissionsToNone();

    _commentsAction.setPlaceHolderString("Enter comments here...");
    _commentsAction.setConnectionPermissionsToNone();
    _commentsAction.setClearable(true);
}

}
