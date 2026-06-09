// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Serializable.h"
#include "Application.h"
#include "CoreInterface.h"
#include "WorkflowExecutionContext.h"

#include "actions/WidgetAction.h"

#include "util/Serialization.h"
#include "util/Exception.h"

#include "exception/SerializationException.h"

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

void Serializable::fromVariantMapScoped(const QVariantMap& variantMap, const SharedWorkflowExecutionContext& parentExecutionContext)
{
    auto plan = fromVariantMapWorkflow(variantMap, parentExecutionContext);

    WorkflowRuntimeScoped::executeBlocking(std::move(plan), parentExecutionContext);
}

UniqueWorkflowPlan Serializable::fromVariantMapWorkflow(const QVariantMap& variantMap, const SharedWorkflowExecutionContext& parentExecutionContext /*= nullptr*/)
{
    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(QString("%1::fromVariantMap").arg(getSerializationName()));

    plan->addSequentialStage("Load", {
        WorkflowPlan::Job("Load", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
            fromVariantMap(variantMap);
        })
    });

    return plan;
}

QVariantMap Serializable::toVariantMap() const
{
    const_cast<Serializable*>(this)->_serializationCounter[static_cast<int>(Direction::To)]++;

    return {
        { "ID", _id }
    };
}

QVariantMap Serializable::toVariantMapScoped(const SharedWorkflowExecutionContext& parentExecutionContext) const
{
    auto plan = toVariantMapWorkflow();

    const auto result = WorkflowRuntimeScoped::executeBlocking(std::move(plan), parentExecutionContext);

    return result->value<QVariantMap>();
}

UniqueWorkflowPlan Serializable::toVariantMapWorkflow() const
{
    UniqueWorkflowPlan workflowPlan = std::make_unique<WorkflowPlan>(QString("%1::toVariantMap").arg(getSerializationName()));

    workflowPlan->addSequentialStage("Serialize", {
        WorkflowPlan::Job("Serialize", [this](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
                executionContext->setOutput(toVariantMap());
            }, WorkflowPlan::JobThreadAffinity::GuiThread)
        });

    return workflowPlan;
}

void Serializable::fromJsonDocument(const QJsonDocument& jsonDocument)
{
    fromJsonDocumentScoped(jsonDocument);
}

void Serializable::fromJsonDocumentScoped(const QJsonDocument& jsonDocument, const SharedWorkflowExecutionContext& parentExecutionContext)
{
    const auto rootMap = jsonDocument.toVariant().toMap();

    variantMapMustContain(rootMap, getSerializationName());

    const auto objectMap = rootMap[getSerializationName()].toMap();

    fromVariantMapScoped(objectMap, parentExecutionContext);
}

QJsonDocument Serializable::toJsonDocumentScoped(const SharedWorkflowExecutionContext& parentExecutionContext /*= nullptr*/) const
{
    auto plan = toVariantMapWorkflow();

    auto result = WorkflowRuntimeScoped::executeBlocking(std::move(plan), parentExecutionContext);

    const auto variantMap = result->value<QVariantMap>();

    return QJsonDocument::fromVariant(variantMap);
}

void Serializable::fromJsonFile(const QString& filePath /*= ""*/)
{
    if (!QFileInfo(filePath).exists())
        throw std::runtime_error("File does not exist");

    QFile jsonFile(filePath);

    if (!jsonFile.open(QIODevice::ReadOnly))
        throw std::runtime_error("Unable to open file for reading");

    QByteArray data = jsonFile.readAll();

    if (data.isEmpty())
        throw std::runtime_error("No data read");

    auto jsonDocument = QJsonDocument::fromJson(data);

    if (jsonDocument.isNull() || jsonDocument.isEmpty())
        throw std::runtime_error("JSON document is invalid");

    fromJsonDocument(jsonDocument);
}

void Serializable::fromJsonFileScoped(const QString& filePath, const SharedWorkflowExecutionContext& parentExecutionContext)
{
    if (!QFileInfo(filePath).exists())
        throw std::runtime_error("File does not exist");

    QFile jsonFile(filePath);

    if (!jsonFile.open(QIODevice::ReadOnly))
        throw std::runtime_error("Unable to open file for reading");

    const QByteArray data = jsonFile.readAll();

    if (data.isEmpty())
        throw std::runtime_error("No data read");

    const auto jsonDocument = QJsonDocument::fromJson(data);

    if (jsonDocument.isNull() || jsonDocument.isEmpty())
        throw std::runtime_error("JSON document is invalid");

    fromJsonDocumentScoped(jsonDocument, parentExecutionContext);
}

void Serializable::toJsonFile(const QString& filePath) const
{
    toJsonFileScoped(filePath);
}

void Serializable::toJsonFileScoped(const QString& filePath /*= ""*/, const SharedWorkflowExecutionContext& parentExecutionContext /*= nullptr*/) const
{
    QFile jsonFile(filePath);

    if (!jsonFile.open(QFile::WriteOnly))
        throw std::runtime_error("Unable to open file for writing");

    auto jsonDocument = toJsonDocumentScoped(parentExecutionContext);

    if (jsonDocument.isNull() || jsonDocument.isEmpty())
        throw std::runtime_error("JSON document is invalid");

#ifdef SERIALIZABLE_VERBOSE
    qDebug().noquote() << jsonDocument.toJson(QJsonDocument::Indented);
#endif

    jsonFile.write(jsonDocument.toJson());

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

QPointer<Task> Serializable::getTask() const
{
    return _task;
}

bool Serializable::hasTask() const
{
    return !_task.isNull();
}

void Serializable::setTask(Task* task)
{
    _task = task;
}

void Serializable::fromVariantMap(Serializable* serializable, const QVariantMap& variantMap)
{
	Q_ASSERT(serializable);

    if (!serializable)
        throw std::runtime_error("Serializable pointer is null");

    serializable->fromVariantMap(variantMap);
}

void Serializable::fromVariantMap(Serializable& serializable, const QVariantMap& variantMap, const QString& key)
{
    if (!variantMap.contains(key)) {
        serializable.handleKeyNotFoundInVariantMap(serializable, variantMap, key);
    }
    else {
        serializable.fromVariantMap(variantMap[key].toMap());
    }
}

void Serializable::fromParentVariantMap(const QVariantMap& parentVariantMap, bool ignoreLoadingErrors /*= false*/)
{
    if (getSerializationName().isEmpty())
        throw std::runtime_error("Serialization name may not be empty");

    if (!parentVariantMap.contains(getSerializationName())) {
        handleKeyNotFoundInVariantMap(*this, parentVariantMap, getSerializationName());
    }
    else {
        fromVariantMap(parentVariantMap[getSerializationName()].toMap());
    }
}

void Serializable::fromParentVariantMapScoped(const QVariantMap& parentVariantMap, const SharedWorkflowExecutionContext& parentExecutionContext)
{
    if (getSerializationName().isEmpty())
        throw std::runtime_error("Serialization name may not be empty");

    if (!parentVariantMap.contains(getSerializationName())) {
        handleKeyNotFoundInVariantMap(*this, parentVariantMap, getSerializationName());
        return;
    }

    fromVariantMapScoped(parentVariantMap[getSerializationName()].toMap(), parentExecutionContext);
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

void Serializable::insertIntoVariantMapScoped(QVariantMap& variantMap, const SharedWorkflowExecutionContext& parentExecutionContext) const
{
    if (getSerializationName().isEmpty())
        throw std::runtime_error("Serialization name may not be empty");

    if (variantMap.contains(getSerializationName()))
        throw std::runtime_error(QString("%1 already exists in variant map (%2)").arg(getSerializationName()).toLatin1());

    variantMap.insert(getSerializationName(), toVariantMapScoped(parentExecutionContext));
}

void Serializable::handleKeyNotFoundInVariantMap(const Serializable& serializable, const QVariantMap& map, const QString& key)
{
    const auto errorMessage = QString(
        "Required key '%1' was not found in QVariantMap. "
        "Available keys (%2 total): %3"
    ).arg(
        key,
        QString::number(map.size()),
        describeVariantMapKeys(map)
    );

    if (settings().getMiscellaneousSettings().getIgnoreLoadingErrorsAction().isChecked()) {
        throw ManiVaultException(SeverityLevel::Warning, "Missing key in QVariantMap", errorMessage, __FUNCTION__, {
            { "Key", key },
            { "SerializationName", serializable.getSerializationName() },
            { "VariantMap", describeVariantMapKeys(map) }
        });
    }

	throw ManiVaultException(SeverityLevel::Error, "Missing key in QVariantMap", errorMessage, __FUNCTION__, {
		{ "Key", key },
        { "SerializationName", serializable.getSerializationName() },
        { "VariantMap", describeVariantMapKeys(map) }
	});
}

}
