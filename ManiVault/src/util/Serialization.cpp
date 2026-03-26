// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Serialization.h"
#include "CoreInterface.h"
#include "Application.h"
#include "CodecRegistry.h"
#include "DecodeRequestState.h"

#include <QUuid>
#include <QtConcurrent>

#include <exception>

#include <math.h>

namespace mv::util {

void saveRawDataToBinaryFile(const char* bytes, const std::uint64_t& numberOfBytes, const QString& filePath)
{
    // Exit prematurely if the serialization process was aborted
    if (Application::isSerializationAborted())
        return;

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
    // Exit prematurely if the serialization process was aborted
    if (Application::isSerializationAborted())
        return;

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

static EncodeBlockResult encodeBlock(const EncodeBlockJob& job, bool saveToDisk, const QString& saveDir, const std::function<std::shared_ptr<BlobCodec>()>& createCodec)
{
    EncodeBlockResult result;

    try {
        QVariantMap blockVariantMap;

        blockVariantMap["Offset"] = QVariant::fromValue(job._offset);
        blockVariantMap["Size"]   = QVariant::fromValue(job._size);

        if (saveToDisk) {
            auto codec = createCodec();

            const auto fileName         = QUuid::createUuid().toString(QUuid::WithoutBraces) + codec->getFileExtension();
            const auto filePath         = QDir::cleanPath(saveDir + QDir::separator() + fileName);
            const auto encodeResult     = codec->encodeToFile(job._rawData, filePath);

            if (!encodeResult.isSuccess()) {
                throw std::runtime_error(QString("Failed to encode block to file: %1").arg(filePath).toStdString());
            }

            blockVariantMap["CompressedSize"]   = QVariant::fromValue<std::uint64_t>(static_cast<std::uint64_t>(encodeResult._data.size()));
            blockVariantMap["URI"]              = fileName;
        }
        else {
            blockVariantMap["Data"] = QString::fromUtf8(job._rawData.toBase64());
        }

        result._block = std::move(blockVariantMap);
    }
    catch (const std::exception& e) {
        result._error = QString::fromUtf8(e.what());
    }

    return result;
}

QVariantMap rawDataToVariantMap(const char* bytes, const std::uint64_t& numberOfBytes, bool saveToDisk /*= false*/, const BlobCodec* blobCodecOverride /*= nullptr*/, ConcurrencyMode concurrencyMode /*= ConcurrencyMode::Sequential*/)
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

	    QVector<EncodeBlockJob> encodeBlockJobs;

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

        QVector<EncodeBlockResult> encodeBlockResults;

        encodeBlockResults.resize(encodeBlockJobs.size());

        if (concurrencyMode == ConcurrencyMode::Parallel) {
            encodeBlockResults = QtConcurrent::blockingMapped<QVector<EncodeBlockResult>>(encodeBlockJobs, [saveToDisk, saveDir, createCodec](const EncodeBlockJob& job) {
            	return encodeBlock(job, saveToDisk, saveDir, createCodec);
            });
        }
        else {
            for (int i = 0; i < encodeBlockJobs.size(); ++i) {
                encodeBlockResults[i] = encodeBlock(encodeBlockJobs[i], saveToDisk, saveDir, createCodec);
            }
        }

        QVariantList blocks;

        blocks.reserve(encodeBlockResults.size());

        for (const auto& result : encodeBlockResults) {
            if (!result._error.isEmpty())
                throw std::runtime_error(result._error.toStdString());

            blocks.push_back(result._block);
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

DecodeBlockResult decodeBlock(const DecodeBlockJob& job, char* bytes, const std::function<std::shared_ptr<BlobCodec>()>& createCodec)
{
    DecodeBlockResult result;

    try {
        auto codec = createCodec();

        if (!job._uri.isEmpty()) {
            const auto decodeResult = codec->decodeFromFileTo(
                job._uri,
                bytes + job._offset,
                static_cast<std::uint64_t>(job._size)
            );

            if (!decodeResult.isSuccess()) {
                throw std::runtime_error(
                    QString("Failed to decode block from file: %1").arg(job._uri).toStdString()
                );
            }

            return result;
        }

        if (!job._encodedData.isEmpty()) {
            const QByteArray encodedBytes = QByteArray::fromBase64(job._encodedData.toUtf8());

            const auto decodeResult = codec->decodeTo(
                encodedBytes,
                bytes + job._offset,
                static_cast<std::uint64_t>(job._size)
            );

            if (!decodeResult.isSuccess()) {
                throw std::runtime_error(
                    QString("Failed to decode inline block at offset %1").arg(job._offset).toStdString()
                );
            }

            return result;
        }

        throw std::runtime_error("Block contains neither URI nor Data");
    }
    catch (const std::exception& e) {
        result._error = QString::fromUtf8(e.what());
    }

    return result;
}

bool populateDataBufferFromVariantMap(const QVariantMap& variantMap, char* bytes, ConcurrencyMode concurrencyMode /*= ConcurrencyMode::Parallel*/)
{
    auto handle = SerializationScheduler::instance().submitDecode(variantMap, bytes, concurrencyMode);

    QEventLoop loop;

    QObject::connect(handle.data(), &DecodeRequestState::finished,
        &loop, &QEventLoop::quit);

    loop.exec();

    if (handle->hasError()) {
        qWarning() << "Unable to populate data from buffer:" << handle->errorString();
        return false;
    }

    return true;
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
    QVariantMap result = rawDataToVariantMap(byteArray.data(), byteArray.size(), true);
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
    std::vector<char> bytes(totalSize);
    populateDataBufferFromVariantMap(variantMap, bytes.data());

    // Finally convert the data to a QVariant.
    QVariant result;;
    QByteArray byteArray(bytes.data(), bytes.size());
    QDataStream stream(&byteArray, QIODevice::ReadOnly);
    stream >> result;
    return result;
}

QVariantMap storeOnDisk(const QStringList& list)
{
    QByteArray byteArray;
    QDataStream dataStream(&byteArray, QIODevice::WriteOnly);
    dataStream << list;
    QVariantMap variantMap = rawDataToVariantMap((const char*)byteArray.data(), byteArray.size(), true);
    return variantMap;
}

QVariantMap storeOnDisk(const QVector<uint32_t>& vec)
{
    QByteArray byteArray;
    QDataStream dataStream(&byteArray, QIODevice::WriteOnly);
    dataStream << vec;
    QVariantMap variantMap = rawDataToVariantMap((const char*)byteArray.data(), byteArray.size(), true);
    return variantMap;
}

void loadFromDisk(const QVariantMap& variantMap, QStringList& list)
{
    std::vector<char> bytes(variantMap["Size"].value<uint64_t>());
    populateDataBufferFromVariantMap(variantMap, (char*)bytes.data());
    QByteArray byteArray(bytes.data(), bytes.size());
    QDataStream dataStream(byteArray);
    dataStream >> list;
}

void loadFromDisk(const QVariantMap& variantMap, QVector<uint32_t>& vec)
{
    std::vector<char> bytes(variantMap["Size"].value<uint64_t>());
    populateDataBufferFromVariantMap(variantMap, (char*)bytes.data());
    QByteArray byteArray(bytes.data(), bytes.size());
    QDataStream dataStream(byteArray);
    dataStream >> vec;
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

}
