#include "Project.h"
#include "AbstractProjectManager.h"
#include "CoreInterface.h"

#include "util/Serialization.h"

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

Project::Project(QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Project"),
    _filePath(),
    _titleAction(this, "Title"),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments"),
    _compressionEnabledAction(this, "Compression", DEFAULT_ENABLE_COMPRESSION, DEFAULT_ENABLE_COMPRESSION),
    _compressionLevelAction(this, "Compression level", 1, 9, DEFAULT_COMPRESSION_LEVEL, DEFAULT_COMPRESSION_LEVEL)
{
    initialize();
}

Project::Project(const QString& filePath, QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Project"),
    _filePath(filePath),
    _titleAction(this, "Title"),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments"),
    _compressionEnabledAction(this, "Compression enabled", DEFAULT_ENABLE_COMPRESSION, DEFAULT_ENABLE_COMPRESSION),
    _compressionLevelAction(this, "Compression enabled", 1, 9, DEFAULT_COMPRESSION_LEVEL, DEFAULT_COMPRESSION_LEVEL)
{
    initialize();

    try {
        if (!QFileInfo(_filePath).exists())
            throw std::runtime_error("File does not exist");

        QFile projectJsonFile(_filePath);

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
    variantMapMustContain(variantMap, "Title");
    variantMapMustContain(variantMap, "Description");
    variantMapMustContain(variantMap, "Tags");
    variantMapMustContain(variantMap, "Comments");

    _titleAction.fromVariantMap(variantMap["Title"].toMap());
    _descriptionAction.fromVariantMap(variantMap["Description"].toMap());
    _tagsAction.fromVariantMap(variantMap["Tags"].toMap());
    _commentsAction.fromVariantMap(variantMap["Comments"].toMap());

    variantMapMustContain(variantMap, "Compression");

    const auto compressionMap = variantMap["Compression"].toMap();

    variantMapMustContain(compressionMap, "Enabled");
    variantMapMustContain(compressionMap, "Level");

    _compressionEnabledAction.fromVariantMap(compressionMap["Enabled"].toMap());
    _compressionLevelAction.fromVariantMap(compressionMap["Level"].toMap());

    plugins().fromVariantMap(variantMap[plugins().getSerializationName()].toMap());
    dataHierarchy().fromVariantMap(variantMap[dataHierarchy().getSerializationName()].toMap());
    actions().fromVariantMap(variantMap[actions().getSerializationName()].toMap());
}

QVariantMap Project::toVariantMap() const
{
    QVariantMap variantMap;

    QVariantMap compressionMap{
        { "Enabled", _compressionEnabledAction.toVariantMap() },
        { "Level", _compressionLevelAction.toVariantMap() }
    };

    return {
        { plugins().getSerializationName(), plugins().toVariantMap() },
        { dataHierarchy().getSerializationName(), dataHierarchy().toVariantMap() },
        { actions().getSerializationName(), actions().toVariantMap() },
        { "Title", _titleAction.toVariantMap() },
        { "Description", _descriptionAction.toVariantMap() },
        { "Tags", _tagsAction.toVariantMap() },
        { "Comments", _commentsAction.toVariantMap() },
        { "Compression", compressionMap },
    };
}

void Project::initialize()
{
    _titleAction.setPlaceHolderString("Enter project title here...");
    _titleAction.setConnectionPermissionsToNone();
    _titleAction.setClearable(true);

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

    _compressionEnabledAction.setConnectionPermissionsToNone();

    _compressionLevelAction.setConnectionPermissionsToNone();
    _compressionLevelAction.setPrefix("Level: ");

    const auto updateCompressionLevelReadOnly = [this]() -> void {
        _compressionLevelAction.setEnabled(_compressionEnabledAction.isChecked());
    };

    connect(&_compressionEnabledAction, &ToggleAction::toggled, this, updateCompressionLevelReadOnly);

    updateCompressionLevelReadOnly();
}

}
