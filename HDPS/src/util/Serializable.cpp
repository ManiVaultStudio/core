#include "Serializable.h"
#include "Application.h"

#include "actions/WidgetAction.h"
#include "util/Serialization.h"
#include "util/Exception.h"

#include <QDebug>
#include <QFileDialog>
#include <QJsonArray>
#include <QUuid>

#ifdef _DEBUG
    #define SERIALIZABLE_VERBOSE
#endif

using namespace hdps::gui;

namespace hdps::util {

Serializable::Serializable(const QString& name /*= ""*/) :
    _id(QUuid::createUuid().toString(QUuid::WithoutBraces)),
    _serializationName(name)
{
}

QString Serializable::getId() const
{
    return _id;
}

QString Serializable::getSerializationName() const
{
    return _serializationName;
}

void Serializable::setSerializationName(const QString& serializationName)
{
    _serializationName = serializationName;
}

void Serializable::fromVariantMap(const QVariantMap& variantMap)
{
    variantMapMustContain(variantMap, "ID");

    _id = variantMap["ID"].toString();
}

QVariantMap Serializable::toVariantMap() const
{
    return {
        { "ID", _id }
    };
}

void Serializable::fromJsonDocument(const QJsonDocument& jsonDocument)
{
    const auto variantMap = jsonDocument.toVariant().toMap();

    fromVariantMap(const_cast<Serializable*>(this), variantMap[getSerializationName()].toMap());
}

QJsonDocument Serializable::toJsonDocument() const
{
    QVariantMap variantMap;

    variantMap[getSerializationName()] = const_cast<Serializable*>(this)->toVariantMap(this);

    return QJsonDocument::fromVariant(variantMap);
}

void Serializable::fromJsonFile(const QString& filePath /*= ""*/)
{
    if (Application::isSerializationAborted())
        return;

    try
    {
        if (!QFileInfo(filePath).exists())
            throw std::runtime_error("File does not exist");

        QFile jsonFile(filePath);

        if (!jsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray data = jsonFile.readAll();

        QJsonDocument jsonDocument;

        jsonDocument = QJsonDocument::fromJson(data);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        fromJsonDocument(jsonDocument);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to load data from JSON file", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to load data from JSON file");
    }
}

void Serializable::toJsonFile(const QString& filePath /*= ""*/)
{
    if (Application::isSerializationAborted())
        return;

    try
    {
        QFile jsonFile(filePath);

        if (!jsonFile.open(QFile::WriteOnly))
            throw std::runtime_error("Unable to open file for writing");

        auto jsonDocument = toJsonDocument();

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

#ifdef SERIALIZABLE_VERBOSE
        qDebug().noquote() << jsonDocument.toJson(QJsonDocument::Indented);
#endif

        jsonFile.write(jsonDocument.toJson());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to save data to JSON file", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to save data to JSON file");
    }
}

void Serializable::fromVariantMap(Serializable* serializable, const QVariantMap& variantMap)
{
#ifdef SERIALIZABLE_VERBOSE
    qDebug().noquote() << QString("From variant map: %1").arg(serializable->getSerializationName());
#endif

    serializable->fromVariantMap(variantMap);

    /*
    auto object = dynamic_cast<const QObject*>(serializable);

    if (object == nullptr)
        return;

    for (auto child : object->children()) {
        auto childSerializable = dynamic_cast<Serializable*>(child);

        if (!childSerializable)
            continue;

        fromVariantMap(childSerializable, variantMap[childSerializable->getSerializationName()].toMap());
    }
    */
}

void Serializable::fromVariantMap(Serializable& serializable, const QVariantMap& variantMap, const QString& key)
{
    if (!variantMap.contains(key))
        throw std::runtime_error(QString("%1 not found in map").arg(key).toLatin1());

    serializable.fromVariantMap(variantMap[key].toMap());
}

void Serializable::fromParentVariantMap(const QVariantMap& parentVariantMap)
{
    try
    {
        if (getSerializationName().isEmpty())
            throw std::runtime_error("Serialization name may not be empty");

        if (!parentVariantMap.contains(getSerializationName()))
            throw std::runtime_error(QString("%1 not found in map: %2").arg(getSerializationName()).toLatin1());

        fromVariantMap(parentVariantMap[getSerializationName()].toMap());
    }
    catch (std::exception& e)
    {
#ifdef _DEBUG
        qDebug() << QString("%1 failed: %2").arg(__FUNCTION__, e.what());
#else
        throw e;
#endif
    }
    catch (...) {
#ifdef _DEBUG
        qDebug() << "";
#else
        throw std::runtime_error(QString("%1 failed due to unhandled exception").arg(__FUNCTION__, getSerializationName()).toLatin1());
#endif
    }
}

QVariantMap Serializable::toVariantMap(const Serializable* serializable)
{
#ifdef SERIALIZABLE_VERBOSE
    qDebug().noquote() << QString("To variant map: %1").arg(serializable->getSerializationName());
#endif

    return serializable->toVariantMap();
}

void Serializable::insertIntoVariantMap(const Serializable& serializable, QVariantMap& variantMap, const QString& key)
{
    variantMap.insert(key, serializable.toVariantMap());
}

void Serializable::insertIntoVariantMap(const Serializable& serializable, QVariantMap& variantMap)
{
    serializable.insertIntoVariantMap(variantMap);
}

void Serializable::insertIntoVariantMap(QVariantMap& variantMap) const
{
    if (getSerializationName().isEmpty())
        throw std::runtime_error("Serialization name may not be empty");

    variantMap.insert(getSerializationName(), toVariantMap());
}

}
