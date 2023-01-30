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
    _version(Application::current()->getVersion()),
    _titleAction(this, "Title"),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments"),
    _contributorsAction(this, "Contributors"),
    _compressionEnabledAction(this, "Compression", DEFAULT_ENABLE_COMPRESSION, DEFAULT_ENABLE_COMPRESSION),
    _compressionLevelAction(this, "Compression level", 1, 9, DEFAULT_COMPRESSION_LEVEL, DEFAULT_COMPRESSION_LEVEL)
{
    initialize();
}

Project::Project(const QString& filePath, bool preview, QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Project"),
    _filePath(filePath),
    _version(Application::current()->getVersion()),
    _titleAction(this, "Title"),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments"),
    _contributorsAction(this, "Contributors"),
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

        fromVariantMap(jsonDocument.toVariant().toMap()["Project"].toMap(), preview);
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
    fromVariantMap(variantMap, false);
}

void Project::fromVariantMap(const QVariantMap& variantMap, bool preview)
{
    Serializable::fromVariantMap(variantMap);

    Serializable::fromVariantMap(_version, variantMap, "Version");
    Serializable::fromVariantMap(_titleAction, variantMap, "Title");
    Serializable::fromVariantMap(_descriptionAction, variantMap, "Description");
    Serializable::fromVariantMap(_tagsAction, variantMap, "Tags");
    Serializable::fromVariantMap(_commentsAction, variantMap, "Comments");
    Serializable::fromVariantMap(_contributorsAction, variantMap, "Contributors");

    variantMapMustContain(variantMap, "Compression");

    const auto compressionMap = variantMap["Compression"].toMap();

    variantMapMustContain(compressionMap, "Enabled");
    variantMapMustContain(compressionMap, "Level");

    _compressionEnabledAction.fromVariantMap(compressionMap["Enabled"].toMap());
    _compressionLevelAction.fromVariantMap(compressionMap["Level"].toMap());

    if (!preview) {
        plugins().fromVariantMap(variantMap[plugins().getSerializationName()].toMap());
        dataHierarchy().fromVariantMap(variantMap[dataHierarchy().getSerializationName()].toMap());
        actions().fromVariantMap(variantMap[actions().getSerializationName()].toMap());
    }
}

QVariantMap Project::toVariantMap() const
{
    QVariantMap variantMap = Serializable::toVariantMap();

    const QVariantMap compressionMap{
        { "Enabled", _compressionEnabledAction.toVariantMap() },
        { "Level", _compressionLevelAction.toVariantMap() }
    };

    Serializable::insertIntoVariantMap(_version, variantMap, "Version");
    Serializable::insertIntoVariantMap(_titleAction, variantMap, "Title");
    Serializable::insertIntoVariantMap(_descriptionAction, variantMap, "Description");
    Serializable::insertIntoVariantMap(_tagsAction, variantMap, "Tags");
    Serializable::insertIntoVariantMap(_commentsAction, variantMap, "Comments");
    Serializable::insertIntoVariantMap(_contributorsAction, variantMap, "Contributors");

    variantMap.insert({
        { "Compression", compressionMap },
        { plugins().getSerializationName(), plugins().toVariantMap() },
        { dataHierarchy().getSerializationName(), dataHierarchy().toVariantMap() },
        { actions().getSerializationName(), actions().toVariantMap() }
    });

    return variantMap;
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
    _tagsAction.setStretch(2);
    _tagsAction.setConnectionPermissionsToNone();

    _commentsAction.setPlaceHolderString("Enter project comments here...");
    _commentsAction.setConnectionPermissionsToNone();
    _commentsAction.setClearable(true);
    _commentsAction.setStretch(2);
    _commentsAction.setDefaultWidgetFlags(StringAction::TextEdit);

    _contributorsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("user"));
    _contributorsAction.setConnectionPermissionsToNone();
    _contributorsAction.setCategory("Contributor");
    _contributorsAction.setEnabled(false);
    _contributorsAction.setStretch(1);
    _contributorsAction.setDefaultWidgetFlags(StringsAction::ListView);
    
    _compressionEnabledAction.setConnectionPermissionsToNone();

    _compressionLevelAction.setConnectionPermissionsToNone();
    _compressionLevelAction.setPrefix("Level: ");

    const auto updateCompressionLevelReadOnly = [this]() -> void {
        _compressionLevelAction.setEnabled(_compressionEnabledAction.isChecked());
    };

    connect(&_compressionEnabledAction, &ToggleAction::toggled, this, updateCompressionLevelReadOnly);

    updateCompressionLevelReadOnly();

    updateContributors();
}

util::Version Project::getVersion() const
{
    return _version;
}

void Project::updateContributors()
{
    QString currentUserName;

#ifdef __APPLE__
    currentUserName = getenv("USER");
#else
    currentUserName = getenv("USERNAME");
#endif

    if (!currentUserName.isEmpty() && !_contributorsAction.getStrings().contains(currentUserName))
        _contributorsAction.addString(currentUserName);
}

}
