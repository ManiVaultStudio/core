// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Serialization.h"
#include "CoreInterface.h"
#include "Application.h"
#include "CodecRegistry.h"
#include "WorkflowReporter.h"

#include <QUuid>

#include <exception>

#include <math.h>

namespace mv::util {

void saveRawDataToBinaryFile(const char* bytes, const std::uint64_t& numberOfBytes, const QString& filePath)
{
    // Output directory
    const auto outputDirectory = QFileInfo(filePath).dir();

    // Exit prematurely if the target directory does not exist
    if (!QFileInfo(filePath).dir().exists())
        throw std::runtime_error(QString("Unable to save data in %1, the directory does not exist").arg(outputDirectory.dirName()).toLatin1());

    // Create binary file
    QFile binaryFile(filePath);

    // And save it do disk
    if (!binaryFile.open(QIODevice::WriteOnly))
        throw std::runtime_error(QString("Unable to save binary file, cannot open file").toLatin1());

    binaryFile.write(QByteArray::fromRawData(bytes, numberOfBytes)); // usng QByteArray::fromRawData(const char *data, qsizetype size) so that the bytes are not copied.
    binaryFile.close();
}

void loadRawDataFromBinaryFile(char* bytes, const std::uint64_t& numberOfBytes, const QString& filePath)
{
    // Exit prematurely if the target file does not exist
    if (!QFileInfo(filePath).exists())
        throw std::runtime_error(QString("Unable to load binary file, %1 does not exist").arg(filePath).toLatin1());

    // Create binary file
    QFile binaryFile(filePath);

    // Open the file read-only
    if (!binaryFile.open(QIODevice::ReadOnly))
        throw std::runtime_error("Unable to load binary file, cannot open file");

    // Read the raw data from the file directly into bytes
    const auto numberOfBytesRead = binaryFile.read(bytes, numberOfBytes);

    // Except if the number of bytes in the file deviates from the number of requested bytes
    if (numberOfBytesRead != numberOfBytes)
        throw std::runtime_error("Unable to load binary file, number of requested bytes is not the same as in the file");
}

QByteArray readBinaryFileToByteArray(const QString& filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
        throw std::runtime_error(QString("Unable to open file %1").arg(filePath).toStdString());

    return file.readAll();
}

static EncodeBlockResult encodeBlock(const EncodeBlockJob& job, const QString& saveDir, const std::function<std::shared_ptr<BlobCodec>()>& createCodec)
{
    EncodeBlockResult result;

    try {
        QVariantMap blockVariantMap;

        blockVariantMap["Offset"] = QVariant::fromValue(job._offset);
        blockVariantMap["Size"]   = QVariant::fromValue(job._size);

        auto codec = createCodec();

        const auto fileName     = QUuid::createUuid().toString(QUuid::WithoutBraces) + codec->getFileExtension();
        const auto filePath     = QDir::cleanPath(saveDir + QDir::separator() + fileName);
        const auto encodeResult = codec->encodeToFile(job._rawData, filePath);

        if (!encodeResult.isSuccess()) {
            throw std::runtime_error(QString("Failed to encode block to file: %1").arg(filePath).toStdString());
        }

        blockVariantMap["CompressedSize"]   = QVariant::fromValue<std::uint64_t>(static_cast<std::uint64_t>(encodeResult._data.size()));
        blockVariantMap["URI"]              = fileName;

        result._block = std::move(blockVariantMap);
    }
    catch (const std::exception& e) {
        result._error = QString::fromUtf8(e.what());
    }

    return result;
}

QVariantMap rawDataToVariantMap(const char* bytes, const std::uint64_t& numberOfBytes, const BlobCodec* blobCodecOverride /*= nullptr*/)
{
    try {
        if (!mv::projects().hasProject())
            throw std::runtime_error("Unable to save raw data, no project is currently open");

        auto createCodec = [&]() -> std::shared_ptr<BlobCodec> {
            return mv::projects().getCurrentProject()->getCompressionAction().createCodec(nullptr);
		};

        const std::uint64_t maxBlockSizeInBytes = mv::projects().getCurrentProject()->getCompressionAction().getCodecSettingsAction()->getBlockSizeAction().getValue() << 20;

	    QVariantMap rawData;

	    rawData["Size"]     = QVariant::fromValue(numberOfBytes);
	    rawData["Codec"]    = QVariant::fromValue(createCodec()->getName());

	    const auto numberOfBlocks = (numberOfBytes + maxBlockSizeInBytes - 1) / maxBlockSizeInBytes;

        EncodeBlockJobs encodeBlockJobs;

	    encodeBlockJobs.reserve(static_cast<int>(numberOfBlocks));

	    std::uint64_t offset = 0;

	    while (offset < numberOfBytes)
	    {
	        const auto blockSize = std::min(maxBlockSizeInBytes, numberOfBytes - offset);

	        EncodeBlockJob job;

	        job._offset     = offset;
	        job._size       = blockSize;
	        job._rawData    = QByteArray(bytes + offset, static_cast<qsizetype>(blockSize));

	        encodeBlockJobs.push_back(std::move(job));

	        offset += blockSize;
	    }

	    const auto saveDir = QDir::cleanPath(projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Save));

        WorkflowPlan encodeWorkflowPlan("Encode Blocks");
        WorkflowPlan::Jobs encodeJobs;

        std::int32_t encodeBlockJobIndex = 0;

        for (auto& encodeBlockJob : encodeBlockJobs) {
            encodeJobs.emplace_back(QString("Encode Block %1").arg(QString::number(encodeBlockJobIndex)), [&encodeBlockJob, saveDir, createCodec](const WorkflowPlan::Job& job) {
                try {
                    encodeBlockJob._result = encodeBlock(encodeBlockJob, saveDir, createCodec);
                }
                catch (std::exception& e) {
                    Serializable::reportSerializationError("Encoder", "Failed to encode block: " + QString::fromStdString(e.what()));
                }
                catch (...) {
                    Serializable::reportSerializationError("Encoder", "Failed to encode block");
                }
            });

            ++encodeBlockJobIndex;
        }

        if (!encodeJobs.empty()) {
            encodeWorkflowPlan.addParallelStage("Encode Blocks", encodeJobs);
            encodeWorkflowPlan.execute(mv::projects().getWorkflowPlanExecutor());
        }

        QVariantList blocks;

        blocks.reserve(encodeBlockJobs.size());

        for (const auto& encodeBlockJob : encodeBlockJobs) {
            if (!encodeBlockJob._result._error.isEmpty())
                throw std::runtime_error(encodeBlockJob._result._error.toStdString());

            blocks.push_back(encodeBlockJob._result._block);
        }

        rawData["NumberOfBlocks"]   = QVariant::fromValue(numberOfBlocks);
        rawData["BlockSize"]        = QVariant::fromValue(maxBlockSizeInBytes);
        rawData["Blocks"]           = QVariant::fromValue(blocks);

        return rawData;
     }
     catch (const std::exception& e) {
         qWarning() << "Unable to convert raw data to variant map:" << e.what();
     }

     return {};
}

DecodeBlockResult decodeBlockFromFile(const DecodeBlockJob& decodeBlockJob, const std::function<std::shared_ptr<BlobCodec>()>& createCodec)
{
    DecodeBlockResult result;

    result._offset  = decodeBlockJob._offset;
    result._size    = decodeBlockJob._size;

    result._decodedData.resize(static_cast<qsizetype>(result._size));

    auto codec = createCodec();

    if (decodeBlockJob._uri.isEmpty())
        throw std::runtime_error("Block URI is empty");

    const auto decodeResult = codec->decodeFromFileTo(decodeBlockJob._uri, result._decodedData.data(), result._size);

    if (!decodeResult.isSuccess())
        throw std::runtime_error(QString("Failed to decode block from file: %1").arg(decodeBlockJob._uri).toStdString());

    return result;
}

DecodeBlockResult decodeBlockFromFileTo(const DecodeBlockJob& decodeBlockJob, const std::function<std::shared_ptr<BlobCodec>()>& createCodec, char* destination)
{
    DecodeBlockResult result;

    result._offset  = decodeBlockJob._offset;
    result._size    = decodeBlockJob._size;

    //result._decodedData.resize(static_cast<qsizetype>(result._size));

    auto codec = createCodec();

    if (decodeBlockJob._uri.isEmpty())
        throw std::runtime_error("Block URI is empty");

    const auto decodeResult = codec->decodeFromFileTo(decodeBlockJob._uri, destination + result._offset, result._size);

    if (!decodeResult.isSuccess())
        throw std::runtime_error(QString("Failed to decode block from file: %1").arg(decodeBlockJob._uri).toStdString());

    return result;
}

DecodeBlockResult decodeBlockFromBase64To(const DecodeBlockJob& decodeBlockJob, const std::function<std::shared_ptr<BlobCodec>()>& createCodec, char* destination)
{
    DecodeBlockResult result;
    result._offset = decodeBlockJob._offset;
    result._size = decodeBlockJob._size;

    auto codec = createCodec();

    const QByteArray encodedBytes =
        QByteArray::fromBase64(decodeBlockJob._encodedData.toUtf8());

    const auto decodeResult = codec->decodeTo(encodedBytes,destination + result._offset,
            result._size
        );

    if (!decodeResult.isSuccess()) {
        throw std::runtime_error(
            QString("Failed to decode inline block at offset %1")
            .arg(decodeBlockJob._offset)
            .toStdString()
        );
    }

    return result;
}

void populateDataBufferFromVariantMap(const QVariantMap& variantMap, char* bytes)
{
    if (variantMap.isEmpty())
        throw std::runtime_error("Variant map is empty");

    if (!bytes)
        throw std::runtime_error("Destination buffer is null");

    variantMapMustContain(variantMap, "BlockSize");
    variantMapMustContain(variantMap, "Blocks");
    variantMapMustContain(variantMap, "Size");

    const auto blocks       = variantMap.value("Blocks").toList();
    const bool hasCodec     = variantMap.contains("Codec");
    const auto codecName    = hasCodec ? variantMap.value("Codec").toString() : QStringLiteral("none");
    const auto totalSize    = variantMap.value("Size").toULongLong();

    if (hasCodec && !codecRegistry().isRegistered(codecName)) {
        throw std::runtime_error(QStringLiteral("Unable to load raw data, codec %1 is not registered").arg(codecName).toStdString());
    }

    auto createCodec = [hasCodec, codecName]() -> std::shared_ptr<BlobCodec> {
        if (hasCodec)
            return codecRegistry().createCodec(nullptr, codecName);

        return codecRegistry().createCodec(nullptr, BlobCodec::Type::None);
    };

    DecodeBlockJobs decodeBlockJobs;
    decodeBlockJobs.reserve(blocks.size());

    for (const auto& block : blocks) {
        const auto blockMap = block.toMap();

        variantMapMustContain(blockMap, "Offset");
        variantMapMustContain(blockMap, "Size");

        DecodeBlockJob job;

        job._offset         = blockMap.value("Offset").value<quint64>();
        job._size           = blockMap.value("Size").value<quint64>();
        job._compressedSize = blockMap.value("CompressedSize", 0).value<quint64>();
        
        if (blockMap.contains("URI"))
            job._uri = blockMap.value("URI").toString();

        if (blockMap.contains("Data"))
            job._encodedData = blockMap.value("Data").toString();

        decodeBlockJobs.push_back(std::move(job));
    }

    // Overlap check
    std::sort(decodeBlockJobs.begin(), decodeBlockJobs.end(),
        [](const DecodeBlockJob& a, const DecodeBlockJob& b) {
            return a._offset < b._offset;
    });

    quint64 expectedOffset = 0;

    for (int i = 0; i < decodeBlockJobs.size(); ++i) {
        const auto& job = decodeBlockJobs[i];
        const quint64 end = job._offset + job._size;

        if (job._offset < expectedOffset) {
            throw std::runtime_error(QString("Raw-data block overlap detected at offset %1").arg(job._offset).toStdString());
        }

        // Optional: require contiguous packing
        if (job._offset != expectedOffset) {
            throw std::runtime_error(QString("Raw-data block gap detected: expected offset %1, got %2").arg(expectedOffset).arg(job._offset).toStdString());
        }

        expectedOffset = end;
    }

    WorkflowPlan decodeWorkflowPlan("Decode Blocks");

    WorkflowPlan::Jobs decodeJobs;

    std::int32_t decodeBlockJobIndex = 0;

    for (auto& decodeBlockJob : decodeBlockJobs) {
        const double progressWeight = std::max<double>(1.0, static_cast<double>(decodeBlockJob._size));

        decodeJobs.emplace_back(QString("Decode Block %1").arg(QString::number(decodeBlockJobIndex)), [&decodeBlockJob, &bytes, createCodec](const WorkflowPlan::Job& job) {
            try {
                if (decodeBlockJob._uri.isEmpty()) {
                    decodeBlockFromBase64To(decodeBlockJob, createCodec, bytes);
                } else {
                    decodeBlockFromFileTo(decodeBlockJob, createCodec, bytes);
                }
            }
            catch (std::exception& e) {
                Serializable::reportSerializationError("Data hierarchy manager", "Failed to load dataset: " + QString::fromStdString(e.what()));
            }
            catch (...) {
                Serializable::reportSerializationError("Data hierarchy manager", "Failed to load dataset");
            }
        }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread);//s, progressWeight);

        ++decodeBlockJobIndex;
    }

    decodeWorkflowPlan.addParallelStage("Decode blocks", decodeJobs);
    decodeWorkflowPlan.execute(SharedWorkflowPlanExecutor(mv::projects().getWorkflowPlanExecutor()));

    if (auto wokflowExecutionContext = WorkflowExecutionContext::current()) {
        auto state = wokflowExecutionContext->getState();

        if (!state)
            return;

        state->metrics().addInteger("project.data.bytes_loaded", totalSize);
    }
}

void populateDataBufferFromVariantMap(const QVariantMap& variantMap, QByteArray& bytes)
{
    if (variantMap.isEmpty())
        throw std::runtime_error("Variant map is empty");

    variantMapMustContain(variantMap, "Size");

    const auto totalSize = variantMap.value("Size").toULongLong();

    if (totalSize == 0)
        throw std::runtime_error("Decoded buffer size is zero");

    bytes.resize(static_cast<qsizetype>(totalSize));

    populateDataBufferFromVariantMap(variantMap, bytes.data());
}

void variantMapMustContain(const QVariantMap& variantMap, const QString& key)
{
    if (!variantMap.contains(key))
        throw std::runtime_error(QString("%1 not found in map: %2").arg(key, variantMap.keys().join(", ")).toLatin1());
}

QVariant storeQVariant(const QVariant& variant)
{
    
    if (variant.isNull() || !variant.isValid())
        return variant;

    // check how many bytes it takes to store the variant
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream << variant;

    if (byteArray.size() < 10485760)
    {
        return variant;
    }
    // store large variant in one or more binary files similar to raw data
    const quint32 version = 1;
    QVariantMap result = rawDataToVariantMap(byteArray.data(), byteArray.size());
    Q_ASSERT(!result.contains("QVariantOnDiskVersion"));
    result["QVariantOnDiskVersion"] = version; // add an extra item to the result so we can check how the data was stored
    return result;
}

QVariant loadQVariant(const QVariant& variant)
{
    // check if variant contains a non-empty QVariantMap
    QVariantMap variantMap = variant.toMap();
    if (variantMap.empty())
        return variant;

    // check if it was stored on disk, if not just return it
    if (!variantMap.contains("QVariantOnDiskVersion"))
    {
        return variantMap;
    }

    // load the QVariant from disk
    quint32 version = variantMap["QVariantOnDiskVersion"].toUInt();
    Q_ASSERT(version == 1);


    // First check the size we need for a temporary buffer
    variantMapMustContain(variantMap, "Blocks");
    const auto blocks = variantMap["Blocks"].toList();
    uint64_t totalSize = 0;
    for (const auto& block : blocks)
    {
        // Get block variant map
        const auto map = block.toMap();
        variantMapMustContain(map, "Size");
        totalSize += map["Size"].value<uint64_t>();
    }
    // Next create a temporary buffer and load the data
    //std::vector<char> bytes(totalSize);
    //populateDataBufferFromVariantMap(variantMap, bytes.data());

    // Finally convert the data to a QVariant.
    QVariant result;;
    //QByteArray byteArray(bytes.data(), bytes.size());
    //QDataStream stream(&byteArray, QIODevice::ReadOnly);
    //stream >> result;
    return result;
}

QVariantMap storeOnDisk(const QStringList& list)
{
    QByteArray byteArray;
    QDataStream dataStream(&byteArray, QIODevice::WriteOnly);
    dataStream << list;
    QVariantMap variantMap = rawDataToVariantMap((const char*)byteArray.data(), byteArray.size());
    return variantMap;
}

QVariantMap storeOnDisk(const QVector<uint32_t>& vec)
{
    QByteArray byteArray;
    QDataStream dataStream(&byteArray, QIODevice::WriteOnly);
    dataStream << vec;
    QVariantMap variantMap = rawDataToVariantMap((const char*)byteArray.data(), byteArray.size());
    return variantMap;
}

void loadFromDisk(const QVariantMap& variantMap, QStringList& list)
{
    //std::vector<char> bytes(variantMap["Size"].value<uint64_t>());
    //populateDataBufferFromVariantMap(variantMap, (char*)bytes.data());
    //QByteArray byteArray(bytes.data(), bytes.size());
    //QDataStream dataStream(byteArray);
    //dataStream >> list;
}

void loadFromDisk(const QVariantMap& variantMap, QVector<uint32_t>& vec)
{
    //std::vector<char> bytes(variantMap["Size"].value<uint64_t>());
    //populateDataBufferFromVariantMap(variantMap, (char*)bytes.data());
    //QByteArray byteArray(bytes.data(), bytes.size());
    //QDataStream dataStream(byteArray);
    //dataStream >> vec;
}

QVariantMap loadJsonToVariantMap(const QString& filePath)
{
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly)) {
		throw std::runtime_error(("Failed to open file: " + filePath).toStdString());
	}

	const QByteArray data = file.readAll();

	QJsonParseError parseError;
	QJsonDocument   doc = QJsonDocument::fromJson(data, &parseError);

	if (parseError.error != QJsonParseError::NoError) {
		throw std::runtime_error(("JSON parse error: " + parseError.errorString()).toStdString());
	}

	if (!doc.isObject()) {
		throw std::runtime_error("JSON root is not an object");
	}

	return doc.object().toVariantMap();
}

bool isVariantMapRawBlockObject(const QVariantMap& map)
{
	return map.contains("BlockSize") &&
		map.contains("Blocks") &&
		map.contains("Codec") &&
		map.contains("NumberOfBlocks") &&
		map.contains("Size");
}

QVariantMap findRawBlockObject(const QVariant& value)
{
	if (value.canConvert<QVariantMap>()) {
		const QVariantMap map = value.toMap();

		if (isVariantMapRawBlockObject(map))
			return map;

		for (auto it = map.begin(); it != map.end(); ++it) {
			const QVariantMap found = findRawBlockObject(it.value());

			if (!found.isEmpty())
				return found;
		}
	}

	if (value.canConvert<QVariantList>()) {
		const QVariantList list = value.toList();

		for (const QVariant& item : list) {
			const QVariantMap found = findRawBlockObject(item);

			if (!found.isEmpty())
				return found;
		}
	}

	return {};
}

std::uint64_t getRawBlockObjectSize(const QVariantMap& map)
{
	bool       ok   = false;
	const auto size = map.value("Size").toULongLong(&ok);

	return ok ? size : 0;
}

std::uint64_t estimateRawBlockTotalSize(const QVariant& value)
{
	std::uint64_t totalSize = 0;

	if (value.canConvert<QVariantMap>()) {
		const QVariantMap map = value.toMap();

		if (isVariantMapRawBlockObject(map)) {
            totalSize += getRawBlockObjectSize(map);
		}

		for (auto it = map.begin(); it != map.end(); ++it) {
			totalSize += estimateRawBlockTotalSize(it.value());
		}
	}

	if (value.canConvert<QVariantList>()) {
		const QVariantList list = value.toList();

		for (const QVariant& item : list) {
			totalSize += estimateRawBlockTotalSize(item);
		}
	}

	return totalSize;
}

QByteArray serializeVariantMap(const QVariantMap& map)
{
	QByteArray  bytes;
	QDataStream out(&bytes, QIODevice::WriteOnly);

	out.setVersion(QDataStream::Qt_6_8); // match your Qt version if desired
	out << map;

	return bytes;
}

QVariantMap deserializeVariantMap(const QByteArray& bytes)
{
	QVariantMap map;

	QDataStream in(const_cast<QByteArray*>(&bytes), QIODevice::ReadOnly);

	in.setVersion(QDataStream::Qt_6_8);
	in >> map;

	return map;
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

QVariantMap saveOptimizedVariantMap(const QVariantMap& source)
{
	QVariantMap target;
	//target.reserve(source.size());

	for (auto it = source.cbegin(); it != source.cend(); ++it)
		target.insert(it.key(), saveOptimizedVariant(it.value()));

	return target;
}

QVariant saveOptimizedVariant(const QVariant& source)
{
	if (source.metaType().id() == QMetaType::QVariantMap)
		return saveOptimizedVariantMap(source.toMap());

	if (source.metaType().id() == QMetaType::QVariantList)
		return saveOptimizedVariantList(source.toList());

	return source;
}

template<typename T>
QVariant saveTypedVector(const QVariantList& list, const QString& typeName)
{
    QVector<T> values;
    values.reserve(list.size());

    for (const QVariant& item : list)
        values.append(item.value<T>());

    QVariantMap raw;

    raw["__OptimizedVariantList"] = true;
    raw["Type"] = typeName;
    raw["Count"] = static_cast<qulonglong>(values.size());

    raw["Data"] = rawDataToVariantMap(
        reinterpret_cast<const char*>(values.constData()),
        static_cast<std::uint64_t>(values.size() * sizeof(T))
    );

    return raw;
}

QVariant saveBoolVector(const QVariantList& list)
{
    QByteArray bytes;
    bytes.resize(list.size());

    for (qsizetype i = 0; i < list.size(); ++i)
        bytes[i] = list[i].toBool() ? char(1) : char(0);

    QVariantMap raw;

    raw["__OptimizedVariantList"] = true;
    raw["Type"] = "BoolArray";
    raw["Count"] = static_cast<qulonglong>(list.size());

    raw["Data"] = rawDataToVariantMap(
        bytes.constData(),
        static_cast<std::uint64_t>(bytes.size())
    );

    return raw;
}

QVariant saveStringList(const QVariantList& list)
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
    block["__OptimizedVariantList"] = true;
    block["Type"] = "QStringList";
    block["Count"] = static_cast<qulonglong>(strings.size());
    block["Data"] = rawDataToVariantMap(
        bytes.constData(),
        static_cast<std::uint64_t>(bytes.size())
    );

    QDir dir(QDir::cleanPath(projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Save)));

    QStringList entries = dir.entryList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot
    );

    return block;
}

QVariant saveOptimizedVariantList(const QVariantList& list)
{
	if (list.isEmpty())
		return list;

    constexpr qsizetype MinimumOptimizedListSize = 1024;

    if (list.size() < MinimumOptimizedListSize) {
        QVariantList target;
        target.reserve(list.size());

        for (const QVariant& item : list)
            target.append(saveOptimizedVariant(item));

        return target;
    }

	const auto homogeneousType = getVariantListHomogenousType(list);

	if (homogeneousType == QMetaType::UnknownType) {
		QVariantList target;
		target.reserve(list.size());

		for (const QVariant& item : list)
			target.append(saveOptimizedVariant(item));

		return target;
	}

	if (homogeneousType == QMetaType::QVariantMap) {
		QVariantList target;
		target.reserve(list.size());

		for (const QVariant& item : list)
			target.append(saveOptimizedVariantMap(item.toMap()));

		return target;
	}

	if (homogeneousType == QMetaType::QString)
        return saveStringList(list);

	if (homogeneousType == QMetaType::Int)
		return saveTypedVector<int>(list, "Int32Array");

	if (homogeneousType == QMetaType::UInt)
		return saveTypedVector<uint>(list, "UInt32Array");

	if (homogeneousType == QMetaType::LongLong)
		return saveTypedVector<qlonglong>(list, "Int64Array");

	if (homogeneousType == QMetaType::ULongLong)
		return saveTypedVector<qulonglong>(list, "UInt64Array");

	if (homogeneousType == QMetaType::Float)
		return saveTypedVector<float>(list, "Float32Array");

	if (homogeneousType == QMetaType::Double)
		return saveTypedVector<double>(list, "Float64Array");

	if (homogeneousType == QMetaType::Bool)
		return saveBoolVector(list);

	QVariantList target;
	target.reserve(list.size());

	for (const QVariant& item : list)
		target.append(saveOptimizedVariant(item));

	return target;
}

QVariantMap loadOptimizedVariantMap(const QVariantMap& source)
{
	QVariantMap target;

	for (auto it = source.cbegin(); it != source.cend(); ++it)
		target.insert(it.key(), loadOptimizedVariant(it.value()));

	return target;
}

QVariant loadOptimizedVariant(const QVariant& source)
{
	if (source.metaType().id() == QMetaType::QVariantMap) {
		const QVariantMap map = source.toMap();

		if (map.value("__OptimizedVariantList").toBool())
			return loadOptimizedVariantList(map);

		return loadOptimizedVariantMap(map);
	}

	if (source.metaType().id() == QMetaType::QVariantList) {
		const QVariantList list = source.toList();

		QVariantList target;
		target.reserve(list.size());

		for (const QVariant& item : list)
			target.append(loadOptimizedVariant(item));

		return target;
	}

	return source;
}

template<typename T>
QVariantList loadTypedList(const QByteArray& bytes, qsizetype count)
{
    const qsizetype expectedBytes = count * qsizetype(sizeof(T));

    if (bytes.size() != expectedBytes)
        throw std::runtime_error("Invalid optimized QVariantList byte size");

    const auto* src = reinterpret_cast<const T*>(bytes.constData());

    QVariantList list;
    list.reserve(count);

    for (qsizetype i = 0; i < count; ++i)
        list.append(QVariant::fromValue(src[i]));

    return list;
}

QVariantList loadBoolList(const QByteArray& bytes, qsizetype count)
{
    if (bytes.size() != count)
        throw std::runtime_error("Invalid optimized bool QVariantList byte size");

    QVariantList list;
    list.reserve(count);

    for (qsizetype i = 0; i < count; ++i)
        list.append(bytes[i] != char(0));

    return list;
}

QVariant loadStringList(const QVariantMap& block)
{
    if (!block.value("__OptimizedVariantList").toBool())
        return block;

    if (block.value("Type").toString() != "QStringList")
        return block;

    const auto expectedCount =
        static_cast<qsizetype>(block.value("Count").toULongLong());

    const QVariantMap dataMap = block.value("Data").toMap();
    QByteArray bytes;

    populateDataBufferFromVariantMap(dataMap, bytes);

	QDataStream stream(bytes);
    stream.setVersion(QDataStream::Qt_6_8);

    QStringList strings;
    stream >> strings;

    if (stream.status() != QDataStream::Ok)
        throw std::runtime_error("Failed to load optimized QStringList block");

    if (strings.size() != expectedCount)
        throw std::runtime_error("Optimized QStringList count mismatch");

    QVariantList list;
    list.reserve(strings.size());

    for (const QString& string : strings)
        list.append(string);

    return list;
}

QVariant loadOptimizedVariantList(const QVariantMap& map)
{
    const QString type = map.value("Type").toString();

    if (type == "QStringList")
        return loadStringList(map);

    const auto count =
        static_cast<qsizetype>(map.value("Count").toULongLong());

    const QVariantMap dataMap = map.value("Data").toMap();

    QByteArray bytes;
    populateDataBufferFromVariantMap(dataMap, bytes);

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

}
