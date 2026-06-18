// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PropertiesSerializer.h"

#include "util/Serialization.h"

#include "workflow/WorkflowExecutionContext.h"

#include "exception/ManiVaultException.h"

#ifdef _DEBUG
	#define PROPERTIES_SERIALIZER_VERBOSE
#endif

using namespace mv::util;
using namespace mv::workflow;

namespace 
{
    template<typename T>
    QVariantList loadTypedList(const QByteArray& bytes, qsizetype count)
    {
        const qsizetype expectedBytes = count * qsizetype(sizeof(T));

        if (bytes.size() != expectedBytes)
            throw mv::ManiVaultException(
                SeverityLevel::Error,
                "Invalid optimized QVariantList byte size",
                QString("Invalid optimized QVariantList byte size: expected %1, got %2").arg(expectedBytes).arg(bytes.size()),
                __FUNCTION__,
                {
                    { "ExpectedBytes", QString::number(expectedBytes) },
                    { "ActualBytes", QString::number(bytes.size()) }
                }
            );

        const auto* src = reinterpret_cast<const T*>(bytes.constData());

        QVariantList list;
        list.resize(count);

        for (qsizetype i = 0; i < count; ++i)
            list[i] = QVariant::fromValue(src[i]);

        return list;
    }

    QVariantList loadBoolList(const QByteArray& bytes, qsizetype count)
    {
        if (bytes.size() != count)
            throw mv::ManiVaultException(
                SeverityLevel::Error,
                "Invalid optimized bool QVariantList byte size",
                QString("Invalid optimized bool QVariantList byte size: expected %1, got %2").arg(count).arg(bytes.size()),
                __FUNCTION__,
                {
                    { "ExpectedCount", QString::number(count) },
                    { "ActualCount", QString::number(bytes.size()) }
                }
            );

        QVariantList list;
        list.reserve(count);

        for (qsizetype i = 0; i < count; ++i)
            list.append(bytes[i] != char(0));

        return list;
    }

    QVariant loadStringList(const QVariantMap& block, const SharedWorkflowExecutionContext& context)
    {
        if (!block.value("__OptimizedVariantList").toBool())
            return block;

        if (block.value("Type").toString() != "QStringList")
            return block;

        const auto expectedCount =
            static_cast<qsizetype>(block.value("Count").toULongLong());

        const QVariantMap dataMap = block.value("Data").toMap();
        QByteArray bytes = bytesFromBlobVariantMap(dataMap);

        QDataStream stream(bytes);
        stream.setVersion(QDataStream::Qt_6_8);

        QStringList strings;
        stream >> strings;

        if (stream.status() != QDataStream::Ok)
            throw mv::ManiVaultException(
                SeverityLevel::Error,
                "Failed to load optimized QStringList block",
                "Failed to load optimized QStringList block",
                __FUNCTION__,
                {
                    { "Block", block }
                }
            );

        if (strings.size() != expectedCount)
            throw mv::ManiVaultException(
                SeverityLevel::Error,
                "Optimized QStringList count mismatch",
                QString("Optimized QStringList count mismatch: expected %1, got %2").arg(expectedCount).arg(strings.size()),
                __FUNCTION__,
                {
                    { "Block", block }
                }
            );

        QVariantList list;
        list.reserve(strings.size());

        for (const QString& string : strings)
            list.append(string);

        return list;
    }

    template<typename T>
    QVariant saveTypedVector(const QVariantList& list, const QString& typeName, const SharedWorkflowExecutionContext& executionContext)
    {
        QVector<T> values;
        values.reserve(list.size());

        for (const QVariant& item : list)
            values.append(item.value<T>());

        QVariantMap raw;

        raw["__OptimizedVariantList"]   = true;
        raw["Type"]                     = typeName;
        raw["Count"]                    = static_cast<qulonglong>(values.size());
        raw["Data"]                     = bytesToBlobVariantMap(reinterpret_cast<const char*>(values.constData()), static_cast<std::uint64_t>(values.size() * sizeof(T)));

        return raw;
    }

    QVariant saveBoolVector(const QVariantList& list, const SharedWorkflowExecutionContext& executionContext)
    {
        QByteArray bytes;

        bytes.resize(list.size());

        for (qsizetype i = 0; i < list.size(); ++i)
            bytes[i] = list[i].toBool() ? char(1) : char(0);

        QVariantMap raw;

        raw["__OptimizedVariantList"]   = true;
        raw["Type"]                     = "BoolArray";
        raw["Count"]                    = static_cast<qulonglong>(list.size());
        raw["Data"]                     = bytesToBlobVariantMap(bytes.constData(), static_cast<std::uint64_t>(bytes.size()));

        return raw;
    }

    QVariant saveStringList(const QVariantList& list, const SharedWorkflowExecutionContext& executionContext)
    {
        QStringList strings;

        strings.reserve(list.size());

        for (const QVariant& item : list)
            strings.append(item.toString());

        QByteArray bytes;
        QDataStream stream(&bytes, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_6_8);

        stream << strings;

        QVariantMap block;

        block["__OptimizedVariantList"]   = true;
        block["Type"]                     = "QStringList";
        block["Count"]                    = static_cast<qulonglong>(strings.size());
        block["Data"]                     = bytesToBlobVariantMap(bytes.constData(), static_cast<std::uint64_t>(bytes.size()));

        return block;
    }

    QMetaType::Type getVariantListHomogenousType(const QVariantList& list)
    {
        if (list.isEmpty())
            return QMetaType::UnknownType;

        auto classifyOne = [](const QVariant& value) -> QMetaType::Type {
            switch (value.metaType().id()) {
            case QMetaType::Int:
            case QMetaType::UInt:
            case QMetaType::LongLong:
            case QMetaType::ULongLong:
            case QMetaType::Double:
            case QMetaType::Bool:
            case QMetaType::QString:
                return static_cast<QMetaType::Type>(value.metaType().id());

            case QMetaType::QVariantMap:
                return QMetaType::QVariantMap;

            case QMetaType::QByteArray:
                return QMetaType::UnknownType;

            default:
                return QMetaType::UnknownType;
            }
            };

        const auto firstKind = classifyOne(list.front());

        for (const QVariant& item : list) {
            if (classifyOne(item) != firstKind)
                return QMetaType::UnknownType;
        }

        return firstKind;
    }
}

namespace mv
{

UniqueWorkflowPlan PropertiesSerializer::fromVariantMapWorkflow(const QVariantMap& propertiesMap, QVariantMap& destinationPropertiesMap, SharedWorkflowExecutionContext parentContext)
{
#ifdef PROPERTIES_SERIALIZER_VERBOSE
    qDebug() << "Deserializing properties: " << propertiesMap.keys();
#endif

    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    const auto version = propertiesMap.value("PropertiesFormatVersion").toUInt();

    if (version != FormatVersion)
        throw std::runtime_error("Unsupported properties serialization format version");

    WorkflowPlan::Jobs jobs;

    jobs.emplace_back("Process headers", [propertiesMap, &destinationPropertiesMap](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) {
        destinationPropertiesMap = loadOptimizedVariant(propertiesMap, jobExecutionContext).toMap();
    }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, WorkflowPlan::JobProgressMode::Atomic);

    plan->addParallelStage("Preprocessing", jobs);

    return plan;
}

UniqueWorkflowPlan PropertiesSerializer::toVariantMapWorkflow(const QVariantMap& propertiesMap)
{
#ifdef PROPERTIES_SERIALIZER_VERBOSE
    qDebug() << "Serializing properties: " << propertiesMap.keys();
#endif

    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    const auto serializeStage = plan->addSequentialStage("Serialize", [propertiesMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
        auto optimizedPropertiesMap = saveOptimizedVariant(propertiesMap, executionContext).toMap();

        optimizedPropertiesMap["PropertiesFormatVersion"] = FormatVersion;

        executionContext->setOutput(optimizedPropertiesMap);
    });

    return plan;
}

QVariant PropertiesSerializer::loadOptimizedVariant(const QVariant& source, const SharedWorkflowExecutionContext& context)
{
    if (source.metaType().id() == QMetaType::QVariantMap) {
        const QVariantMap map = source.toMap();

        if (map.value("__OptimizedVariantList").toBool())
            return loadOptimizedVariantList(map, context);

        return loadOptimizedVariantMap(map, context);
    }

    if (source.metaType().id() == QMetaType::QVariantList) {
        const QVariantList list = source.toList();

        QVariantList target;
        target.reserve(list.size());

        for (const QVariant& item : list)
            target.append(loadOptimizedVariant(item, context));

        return target;
    }

    return source;
}

QVariantMap PropertiesSerializer::loadOptimizedVariantMap(const QVariantMap& source, const SharedWorkflowExecutionContext& context)
{
    QVariantMap target;

    for (auto it = source.cbegin(); it != source.cend(); ++it)
        target.insert(it.key(), PropertiesSerializer::loadOptimizedVariant(it.value(), context));

    return target;
}

QVariant PropertiesSerializer::loadOptimizedVariantList(const QVariantMap& map, const SharedWorkflowExecutionContext& context)
{
    const QString type = map.value("Type").toString();

    if (type == "QStringList")
        return loadStringList(map, context);

    const auto count =
        static_cast<qsizetype>(map.value("Count").toULongLong());

    const QVariantMap dataMap = map.value("Data").toMap();

    QByteArray bytes = bytesFromBlobVariantMap(dataMap);

    if (type == "BoolArray")
        return loadBoolList(bytes, count);

    if (type == "Int32Array")
        return loadTypedList<int>(bytes, count);

    if (type == "UInt32Array")
        return loadTypedList<uint>(bytes, count);

    if (type == "Int64Array")
        return loadTypedList<qlonglong>(bytes, count);

    if (type == "UInt64Array")
        return loadTypedList<qulonglong>(bytes, count);

    if (type == "Float32Array")
        return loadTypedList<float>(bytes, count);

    if (type == "Float64Array")
        return loadTypedList<double>(bytes, count);

    return map;
}

QVariantMap PropertiesSerializer::saveOptimizedVariantMap(const QVariantMap& source, const SharedWorkflowExecutionContext& executionContext)
{
    QVariantMap target;
    //target.reserve(source.size());

    for (auto it = source.cbegin(); it != source.cend(); ++it)
        target.insert(it.key(), saveOptimizedVariant(it.value(), executionContext));

    return target;
}

QVariant PropertiesSerializer::saveOptimizedVariant(const QVariant& source, const SharedWorkflowExecutionContext& executionContext)
{
    if (source.metaType().id() == QMetaType::QVariantMap)
        return PropertiesSerializer::saveOptimizedVariantMap(source.toMap(), executionContext);

    if (source.metaType().id() == QMetaType::QVariantList)
        return PropertiesSerializer::saveOptimizedVariantList(source.toList(), executionContext);

    return source;
}

QVariant PropertiesSerializer::saveOptimizedVariantList(const QVariantList& list, const SharedWorkflowExecutionContext& executionContext)
{
    if (list.isEmpty())
        return list;

    constexpr qsizetype MinimumOptimizedListSize = 1024;

    if (list.size() < MinimumOptimizedListSize) {
        QVariantList target;
        target.reserve(list.size());

        for (const QVariant& item : list)
            target.append(saveOptimizedVariant(item, executionContext));

        return target;
    }

    const auto homogeneousType = getVariantListHomogenousType(list);

    if (homogeneousType == QMetaType::UnknownType) {
        QVariantList target;
        target.reserve(list.size());

        for (const QVariant& item : list)
            target.append(saveOptimizedVariant(item, executionContext));

        return target;
    }

    if (homogeneousType == QMetaType::QVariantMap) {
        QVariantList target;
        target.reserve(list.size());

        for (const QVariant& item : list)
            target.append(saveOptimizedVariantMap(item.toMap(), executionContext));

        return target;
    }

    if (homogeneousType == QMetaType::QString)
        return saveStringList(list, executionContext);

    if (homogeneousType == QMetaType::Int)
        return saveTypedVector<int>(list, "Int32Array", executionContext);

    if (homogeneousType == QMetaType::UInt)
        return saveTypedVector<uint>(list, "UInt32Array", executionContext);

    if (homogeneousType == QMetaType::LongLong)
        return saveTypedVector<qlonglong>(list, "Int64Array", executionContext);

    if (homogeneousType == QMetaType::ULongLong)
        return saveTypedVector<qulonglong>(list, "UInt64Array", executionContext);

    if (homogeneousType == QMetaType::Float)
        return saveTypedVector<float>(list, "Float32Array", executionContext);

    if (homogeneousType == QMetaType::Double)
        return saveTypedVector<double>(list, "Float64Array", executionContext);

    if (homogeneousType == QMetaType::Bool)
        return saveBoolVector(list, executionContext);

    QVariantList target;

    target.reserve(list.size());

    for (const QVariant& item : list)
        target.append(saveOptimizedVariant(item, executionContext));

    return target;
}

}