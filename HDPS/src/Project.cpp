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
    _applicationVersion(Application::current()->getVersion()),
    _applicationVersionAction(this),
    _projectVersionAction(this),
    _readOnlyAction(this, "Read-only"),
    _titleAction(this, "Title"),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments"),
    _contributorsAction(this, "Contributors"),
    _splashScreenAction(this, *this)
{
    initialize();
}

Project::Project(const QString& filePath, bool preview, QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Project"),
    _filePath(filePath),
    _applicationVersion(Application::current()->getVersion()),
    _applicationVersionAction(this),
    _projectVersionAction(this),
    _readOnlyAction(this, "Read-only"),
    _titleAction(this, "Title"),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments"),
    _contributorsAction(this, "Contributors"),
    _splashScreenAction(this, *this)
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

    _applicationVersion.fromParentVariantMap(variantMap);
    _projectVersionAction.fromParentVariantMap(variantMap);
    _readOnlyAction.fromParentVariantMap(variantMap);
    _titleAction.fromParentVariantMap(variantMap);
    _descriptionAction.fromParentVariantMap(variantMap);
    _tagsAction.fromParentVariantMap(variantMap);
    _commentsAction.fromParentVariantMap(variantMap);
    _contributorsAction.fromParentVariantMap(variantMap);
    _compressionAction.fromParentVariantMap(variantMap);
    _splashScreenAction.fromParentVariantMap(variantMap);

    if (!preview) {
        plugins().fromParentVariantMap(variantMap);
        dataHierarchy().fromParentVariantMap(variantMap);
        actions().fromParentVariantMap(variantMap);
    }
}

QVariantMap Project::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    _applicationVersion.insertIntoVariantMap(variantMap);
    _projectVersionAction.insertIntoVariantMap(variantMap);
    _readOnlyAction.insertIntoVariantMap(variantMap);
    _titleAction.insertIntoVariantMap(variantMap);
    _descriptionAction.insertIntoVariantMap(variantMap);
    _tagsAction.insertIntoVariantMap(variantMap);
    _commentsAction.insertIntoVariantMap(variantMap);
    _contributorsAction.insertIntoVariantMap(variantMap);
    _compressionAction.insertIntoVariantMap(variantMap);
    _splashScreenAction.insertIntoVariantMap(variantMap);

    plugins().insertIntoVariantMap(variantMap);
    dataHierarchy().insertIntoVariantMap(variantMap);
    actions().insertIntoVariantMap(variantMap);

    return variantMap;
}

void Project::initialize()
{
    _applicationVersion.setSerializationName("Application Version");
    _projectVersionAction.setSerializationName("Project Version");

    _readOnlyAction.setToolTip("Whether the project is in read-only mode or not");
    _readOnlyAction.setSerializationName("ReadOnly");

    _titleAction.setPlaceHolderString("Enter project title here...");
    _titleAction.setClearable(true);
    _titleAction.setSerializationName("Title");

    _descriptionAction.setPlaceHolderString("Enter project description here...");
    _descriptionAction.setClearable(true);
    _descriptionAction.setSerializationName("Description");

    _tagsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("tag"));
    _tagsAction.setCategory("Tag");
    _tagsAction.setStretch(2);
    _tagsAction.setSerializationName("Tags");

    _commentsAction.setPlaceHolderString("Enter project comments here...");
    _commentsAction.setClearable(true);
    _commentsAction.setStretch(2);
    _commentsAction.setDefaultWidgetFlags(StringAction::TextEdit);
    _commentsAction.setSerializationName("Comments");

    _contributorsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("user"));
    _contributorsAction.setCategory("Contributor");
    _contributorsAction.setEnabled(false);
    _contributorsAction.setStretch(1);
    _contributorsAction.setDefaultWidgetFlags(StringsAction::ListView);
    _contributorsAction.setSerializationName("Contributors");
    
    updateContributors();
}

util::Version Project::getVersion() const
{
    return _applicationVersion;
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

Project::CompressionAction::CompressionAction(QObject* parent /*= nullptr*/) :
    WidgetAction(parent),
    _enabledAction(this, "Compression", DEFAULT_ENABLE_COMPRESSION, DEFAULT_ENABLE_COMPRESSION),
    _levelAction(this, "Compression level", 1, 9, DEFAULT_COMPRESSION_LEVEL, DEFAULT_COMPRESSION_LEVEL)
{
    setSerializationName("Compression");

    _enabledAction.setSerializationName("Enabled");
    _levelAction.setSerializationName("Level");

    _levelAction.setPrefix("Level: ");

    const auto updateCompressionLevelReadOnly = [this]() -> void {
        _levelAction.setEnabled(_enabledAction.isChecked());
    };

    connect(&_enabledAction, &ToggleAction::toggled, this, updateCompressionLevelReadOnly);

    updateCompressionLevelReadOnly();
}

void Project::CompressionAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    _levelAction.fromParentVariantMap(variantMap);
}

QVariantMap Project::CompressionAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _levelAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
