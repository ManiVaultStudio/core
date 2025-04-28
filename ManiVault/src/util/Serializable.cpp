// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Serializable.h"
#include "Application.h"
#include "CoreInterface.h"

#include "actions/WidgetAction.h"

#include "util/Serialization.h"
#include "util/Exception.h"

#include <QDebug>
#include <QUuid>

#ifdef _DEBUG
    //#define SERIALIZABLE_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;

namespace mv::util {

Serializable::Serializable(const QString& name /*= ""*/) :
    _id(createId()),
    _serializationName(name),
    _serializationCounter{ 0, 0 }
{
}

QString Serializable::getId(bool truncated /*= false*/) const
{
    if (truncated)
        return _id.right(8);
        
    return _id;
}

void Serializable::setId(const QString& id)
{
    _id = id;
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

    _serializationCounter[static_cast<int>(Direction::From)]++;
}

QVariantMap Serializable::toVariantMap() const
{
    const_cast<Serializable*>(this)->_serializationCounter[static_cast<int>(Direction::To)]++;

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

        auto jsonDocument = QJsonDocument::fromJson(data);

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

void Serializable::toJsonFile(const QString& filePath /*= ""*/) const
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

void Serializable::makeUnique()
{
    _id = createId();
}

QString Serializable::createId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

std::int32_t Serializable::getSerializationCount(const Direction& direction) const
{
    return _serializationCounter[static_cast<int>(direction)];
}

std::int32_t Serializable::getSerializationCountFrom() const
{
    return getSerializationCount(Direction::From);
}

std::int32_t Serializable::getSerializationCountTo() const
{
    return getSerializationCount(Direction::To);
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
    if (!variantMap.contains(key)) {
        const auto errorMessage = QString("%1 not found in map: %2").arg(key);

        if (settings().getMiscellaneousSettings().getIgnoreLoadingErrorsAction().isChecked())
            qCritical() << errorMessage; 
        else
            throw std::runtime_error(errorMessage.toLatin1());
    }
    else {
        serializable.fromVariantMap(variantMap[key].toMap());
    }
}

void Serializable::fromParentVariantMap(const QVariantMap& parentVariantMap)
{
    try
    {
        if (getSerializationName().isEmpty())
            throw std::runtime_error("Serialization name may not be empty");

        if (!parentVariantMap.contains(getSerializationName())) {
            const auto errorMessage = QString("%1 not found in map: %2").arg(getSerializationName());

            if (core() != nullptr && settings().getMiscellaneousSettings().getIgnoreLoadingErrorsAction().isChecked())
                qCritical() << errorMessage;
            else
                throw std::runtime_error(errorMessage.toLatin1());
        }
        else {
            fromVariantMap(parentVariantMap[getSerializationName()].toMap());
        }
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

    if (variantMap.contains(getSerializationName()))
        throw std::runtime_error(QString("%1 already exists in variant map (%2)").arg(getSerializationName()).toLatin1());

    variantMap.insert(getSerializationName(), toVariantMap());
}

}
