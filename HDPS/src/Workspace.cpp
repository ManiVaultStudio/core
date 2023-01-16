#include "Workspace.h"
#include "AbstractWorkspaceManager.h"

#include "Application.h"

#include "util/Serialization.h"

#include <QBuffer>

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

Workspace::Workspace(QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Workspace"),
    _filePath(),
    _titleAction(this, "Title"),
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
    _titleAction(this, "Title"),
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

        fromVariantMap(jsonDocument.toVariant().toMap()["Workspace"].toMap());
    }
    catch (std::exception& e)
    {
        qDebug() << "Unable to load workspace from file" << e.what();
    }
    catch (...)
    {
        qDebug() << "Unable to load workspace from file";
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
    Serializable::fromVariantMap(variantMap);

    Serializable::fromVariantMap(_titleAction, variantMap, "Title");
    Serializable::fromVariantMap(_descriptionAction, variantMap, "Description");
    Serializable::fromVariantMap(_tagsAction, variantMap, "Tags");
    Serializable::fromVariantMap(_commentsAction, variantMap, "Comments");
}

QVariantMap Workspace::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    Serializable::insertIntoVariantMap(_titleAction, variantMap, "Title");
    Serializable::insertIntoVariantMap(_descriptionAction, variantMap, "Description");
    Serializable::insertIntoVariantMap(_tagsAction, variantMap, "Tags");
    Serializable::insertIntoVariantMap(_commentsAction, variantMap, "Comments");

    return variantMap;
}

QImage Workspace::getPreviewImage(const QString& workspaceFilePath)
{
    QImage previewImage;

    try {
        if (!QFileInfo(workspaceFilePath).exists())
            throw std::runtime_error("File does not exist");

        QFile workspaceJsonFile(workspaceFilePath);

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

QString Workspace::getPreviewImageHtml(const QString workspaceFilePath, const QSize& targetSize/*= QSize(500, 500)*/)
{
    QBuffer buffer;

    buffer.open(QIODevice::WriteOnly);
    
    const auto previewImage = getPreviewImage(workspaceFilePath);

    QPixmap::fromImage(targetSize.isValid() ? previewImage.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation) : previewImage).save(&buffer, "JPG");

    auto image = buffer.data().toBase64();

    return QString("<img src='data:image/jpg;base64,%1'></p>").arg(image);
}

void Workspace::initialize()
{
    _titleAction.setPlaceHolderString("Enter workspace title here...");
    _titleAction.setConnectionPermissionsToNone();
    _titleAction.setClearable(true);

    _descriptionAction.setPlaceHolderString("Enter workspace description here...");
    _descriptionAction.setConnectionPermissionsToNone();
    _descriptionAction.setClearable(true);

    _tagsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("tag"));
    _tagsAction.setCategory("Tag");
    _tagsAction.setConnectionPermissionsToNone();

    _commentsAction.setPlaceHolderString("Enter workspace comments here...");
    _commentsAction.setConnectionPermissionsToNone();
    _commentsAction.setClearable(true);
    _commentsAction.setDefaultWidgetFlags(StringAction::TextEdit);
}

}
