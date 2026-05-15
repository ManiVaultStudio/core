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
#include "util/AsyncVariantMapResult.h"

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

WorkflowPlan Serializable::fromVariantMapWorkflow(const QVariantMap& variantMap)
{
    WorkflowPlan plan(QString("%1::fromVariantMap").arg(getSerializationName()));

    plan.addSequentialStage("Load", {
        WorkflowPlan::Job("Load", [this, variantMap](const WorkflowPlan::Job&) {
            fromVariantMap(variantMap);
            }, WorkflowPlan::JobThreadAffinity::GuiThread
        )
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

WorkflowPlan Serializable::toVariantMapWorkflow() const
{
    WorkflowPlan workflowPlan(QString("%1::toVariantMap").arg(getSerializationName()));

    workflowPlan.addSequentialStage("Serialize", {
        WorkflowPlan::Job(
            "Serialize",
            [this](WorkflowPlan::Job& job) {
                const QVariantMap variantMap = toVariantMap();
                //job.setResult("variantMap", variantMap);
            }, WorkflowPlan::JobThreadAffinity::GuiThread)
        });

    return workflowPlan;
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

void Serializable::toJsonFile(const QString& filePath /*= ""*/) const
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

void Serializable::reportSerializationWarning(const QString& scope, const QString& message)
{
    if (auto context = WorkflowExecutionContext::current()) {
	    context->getReportNode()->addMessage(SeverityLevel::Warning, "Serializable", message, scope);
    }

	qWarning() << "Warning: " << message << "(" << scope << ")";
}

void Serializable::reportSerializationError(QString scope, QString message)
{
    if (auto context = WorkflowExecutionContext::current()) {
        context->getReportNode()->addMessage(SeverityLevel::Error, "Serializable", message, scope);
    }

	qWarning() << "Error: " << message << "(" << scope << ")";
}

void Serializable::reportFatalSerializationError(QString scope, QString message)
{
    if (auto context = WorkflowExecutionContext::current()) {
	    context->getReportNode()->addMessage(SeverityLevel::Fatal, "Serializable", message, scope);
    }
    
    qWarning() << "Critical: " << message << "(" << scope << ")";
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

    if (!settings().getMiscellaneousSettings().getIgnoreLoadingErrorsAction().isChecked())
		throw SerializationException::missingKey(key, serializable.getSerializationName(), map);


    //if (settings().getMiscellaneousSettings().getIgnoreLoadingErrorsAction().isChecked())
    //    Serializable::reportSerializationWarning(serializable.getSerializationName(), errorMessage);
    //else
    //    Serializable::reportFatalSerializationError(serializable.getSerializationName(), errorMessage);
}

}
