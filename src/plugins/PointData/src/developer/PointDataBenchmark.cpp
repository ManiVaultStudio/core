// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "PointDataBenchmark.h"

#include "../PointData.h"

#include <CoreInterface.h>
#include <DeveloperActionRegistry.h>
#include <Application.h>
#include <Project.h>
#include <models/DatasetsListModel.h>
#include <actions/FilePickerAction.h>
#include <actions/IntegralAction.h>
#include <actions/OptionsAction.h>
#include <actions/OptionAction.h>
#include <actions/PresetsAction.h>
#include <actions/StringAction.h>
#include <actions/VerticalGroupAction.h>
#include <util/StyledIcon.h>

#include <workflow/WorkflowOptions.h>
#include <workflow/WorkflowExecutionContext.h>
#include <workflow/WorkflowPlan.h>

#include "../tiled/PointDataTiledStorage.h"

#include <QCoreApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDateTime>
#include <QDir>
#include <QEventLoop>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QStorageInfo>
#include <QSysInfo>
#include <QTextEdit>
#include <QThread>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <QVariantMap>
#include <QVBoxLayout>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <vector>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#elif defined(Q_OS_UNIX)
#include <unistd.h>
#endif

namespace
{
struct BenchmarkConfiguration
{
    std::uint64_t rows          = 10000;
    std::uint64_t columns       = 128;
    std::size_t samples         = 10000;
    std::size_t randomRowSamples = 100;
    QString scanMode            = QStringLiteral("Quick");
    QString elementType         = QStringLiteral("float32");
    QString rowTileSizes        = QStringLiteral("auto");
    QString columnTileSizes     = QStringLiteral("auto");
    QString cacheCapacities     = QStringLiteral("auto");
    QStringList datasetLabels;
};

struct BenchmarkContext
{
    QString benchmarkTimestamp;
    QString cpuModel;
    QString cpuArchitecture;
    QString operatingSystem;
    std::int32_t logicalCores            = 0;
    std::uint64_t physicalMemoryBytes    = 0;
    QString storageVolume;
    QString storageDevice;
    QString storageFileSystem;
    std::uint64_t storageTotalBytes      = 0;
    std::uint64_t storageAvailableBytes = 0;
    QString projectCodec;
    bool projectCompressionEnabled      = false;
};

struct ScanLimits
{
    std::uint64_t rows;
    std::uint64_t columns;
};

struct ScanDefaults
{
    std::int32_t randomSamples;
    std::int32_t randomRowSamples;
};

struct BenchmarkDataset
{
    mv::Dataset<Points> points;
    QString label;
};

struct BenchmarkResult
{
    QString backend;
    QString elementType;
    QString accessPattern;
    std::uint64_t operations     = 0;
    double elapsedMilliseconds   = 0.0;
    double meanMillisecondsPerRow = 0.0;
    double checksum              = 0.0;
    std::uint64_t cacheHits      = 0;
    std::uint64_t cacheMisses    = 0;
    std::uint64_t cacheEvictions = 0;
};

QString getCpuModel()
{
#ifdef Q_OS_WIN
    QSettings settings(QStringLiteral("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), QSettings::NativeFormat);
    const auto model = settings.value(QStringLiteral("ProcessorNameString")).toString().simplified();

    if (!model.isEmpty())
        return model;
#elif defined(Q_OS_LINUX)
    QFile cpuInfo(QStringLiteral("/proc/cpuinfo"));

    if (cpuInfo.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&cpuInfo);

        while (!stream.atEnd())
        {
            const auto line = stream.readLine();

            if (line.startsWith(QStringLiteral("model name")))
                return line.section(':', 1).trimmed();
        }
    }
#endif

    return QSysInfo::currentCpuArchitecture();
}

std::uint64_t getPhysicalMemoryBytes()
{
#ifdef Q_OS_WIN
    MEMORYSTATUSEX memoryStatus { };
    memoryStatus.dwLength = sizeof(memoryStatus);

    if (GlobalMemoryStatusEx(&memoryStatus))
        return static_cast<std::uint64_t>(memoryStatus.ullTotalPhys);
#elif defined(Q_OS_UNIX)
    const auto pageCount = sysconf(_SC_PHYS_PAGES);
    const auto pageSize  = sysconf(_SC_PAGESIZE);

    if (pageCount > 0 && pageSize > 0)
        return static_cast<std::uint64_t>(pageCount) * static_cast<std::uint64_t>(pageSize);
#endif

    return 0;
}

BenchmarkContext collectBenchmarkContext()
{
    BenchmarkContext context;
    context.benchmarkTimestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
    context.cpuModel           = getCpuModel();
    context.cpuArchitecture    = QSysInfo::currentCpuArchitecture();
    context.operatingSystem    = QStringLiteral("%1 %2").arg(QSysInfo::prettyProductName(), QSysInfo::kernelVersion()).simplified();
    context.logicalCores       = QThread::idealThreadCount();
    context.physicalMemoryBytes = getPhysicalMemoryBytes();

    auto* project = mv::projects().getCurrentProject();
    const auto projectPath = project && !project->getFilePath().isEmpty() ? project->getFilePath() : QDir::currentPath();
    const QStorageInfo storage(QFileInfo(projectPath).absolutePath());

    if (storage.isValid())
    {
        context.storageVolume         = storage.rootPath();
        context.storageDevice         = QString::fromUtf8(storage.device());
        context.storageFileSystem     = QString::fromUtf8(storage.fileSystemType());
        context.storageTotalBytes     = storage.bytesTotal();
        context.storageAvailableBytes = storage.bytesAvailable();
    }

    if (project)
    {
        context.projectCompressionEnabled = project->getCompressionAction().getEnabledAction().isChecked();
        context.projectCodec              = project->getCompressionAction().getCodecTypeAction().getCurrentText();
    }

    return context;
}

QString formatBytes(long double bytes)
{
    static constexpr const char* suffixes[] = { "B", "KiB", "MiB", "GiB", "TiB" };
    std::size_t suffix = 0;

    while (bytes >= 1024.0L && suffix < 4)
    {
        bytes /= 1024.0L;
        ++suffix;
    }

    return QString::number(static_cast<double>(bytes), 'f', suffix == 0 ? 0 : 2) + " " + suffixes[suffix];
}

std::size_t elementSizeForType(const Points::ElementTypeSpecifier type)
{
    switch (type)
    {
        case Points::ElementTypeSpecifier::float32:
        case Points::ElementTypeSpecifier::int32:
        case Points::ElementTypeSpecifier::uint32:
            return sizeof(std::int32_t);

        case Points::ElementTypeSpecifier::bfloat16:
        case Points::ElementTypeSpecifier::int16:
        case Points::ElementTypeSpecifier::uint16:
            return sizeof(std::int16_t);

        case Points::ElementTypeSpecifier::int8:
        case Points::ElementTypeSpecifier::uint8:
            return sizeof(std::int8_t);

        default:
            return sizeof(float);
    }
}

void writeBenchmarkValue(char* destination, const std::size_t offset, const Points::ElementTypeSpecifier type, const float value)
{
    switch (type)
    {
        case Points::ElementTypeSpecifier::float32:
            std::memcpy(destination + offset, &value, sizeof(value));
            break;

        case Points::ElementTypeSpecifier::bfloat16:
        {
            std::uint32_t bits = 0;
            std::memcpy(&bits, &value, sizeof(bits));
            const auto truncated = static_cast<std::uint16_t>(bits >> 16);
            std::memcpy(destination + offset, &truncated, sizeof(truncated));
            break;
        }

        case Points::ElementTypeSpecifier::int32:
        {
            const auto converted = static_cast<std::int32_t>(value);
            std::memcpy(destination + offset, &converted, sizeof(converted));
            break;
        }

        case Points::ElementTypeSpecifier::uint32:
        {
            const auto converted = static_cast<std::uint32_t>(value);
            std::memcpy(destination + offset, &converted, sizeof(converted));
            break;
        }

        case Points::ElementTypeSpecifier::int16:
        {
            const auto converted = static_cast<std::int16_t>(value);
            std::memcpy(destination + offset, &converted, sizeof(converted));
            break;
        }

        case Points::ElementTypeSpecifier::uint16:
        {
            const auto converted = static_cast<std::uint16_t>(value);
            std::memcpy(destination + offset, &converted, sizeof(converted));
            break;
        }

        case Points::ElementTypeSpecifier::int8:
        {
            const auto converted = static_cast<std::int8_t>(value);
            std::memcpy(destination + offset, &converted, sizeof(converted));
            break;
        }

        case Points::ElementTypeSpecifier::uint8:
        {
            const auto converted = static_cast<std::uint8_t>(value);
            std::memcpy(destination + offset, &converted, sizeof(converted));
            break;
        }
    }
}

struct ShardedStorage
{
    std::unique_ptr<QTemporaryDir> directory;
    std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> storage;
};

float valueAt(const std::uint64_t row, const std::uint64_t column, const std::uint64_t columns)
{
    return static_cast<float>((row * columns + column) % 1000003);
}

std::size_t nextPowerOfTwo(const std::uint64_t value)
{
    std::size_t result = 1;

    while (result < value && result <= std::numeric_limits<std::size_t>::max() / 2)
        result *= 2;

    return result;
}

std::vector<std::size_t> autoTileSizes(const std::uint64_t dimension, const std::size_t minimum, const std::size_t maximum, const std::initializer_list<std::size_t> targetTileCounts)
{
    std::vector<std::size_t> values;

    for (const auto targetTileCount : targetTileCounts)
    {
        const auto requestedSize = (dimension + targetTileCount - 1) / targetTileCount;
        const auto tileSize       = std::clamp(nextPowerOfTwo(requestedSize), minimum, maximum);

        if (tileSize <= dimension && std::find(values.begin(), values.end(), tileSize) == values.end())
            values.push_back(tileSize);
    }

    if (values.empty())
        values.push_back(static_cast<std::size_t>(std::min<std::uint64_t>(dimension, maximum)));

    return values;
}

std::vector<std::size_t> parseValues(const QString& text, const std::uint64_t dimension, const std::size_t minimum, const std::size_t maximum, const std::initializer_list<std::size_t> targetTileCounts)
{
    if (text.trimmed().compare(QStringLiteral("auto"), Qt::CaseInsensitive) == 0)
        return autoTileSizes(dimension, minimum, maximum, targetTileCounts);

    std::vector<std::size_t> values;

    for (const auto& item : text.split(',', Qt::SkipEmptyParts))
    {
        bool ok = false;
        const auto value = item.trimmed().toULongLong(&ok);

        if (!ok || value == 0)
            throw std::invalid_argument(QStringLiteral("Invalid sweep value: %1").arg(item).toStdString());

        const auto tileSize = static_cast<std::size_t>(value);

        if (tileSize > dimension)
            throw std::invalid_argument(QStringLiteral("Tile size %1 exceeds dataset dimension %2").arg(tileSize).arg(dimension).toStdString());

        values.push_back(tileSize);
    }

    if (values.empty())
        throw std::invalid_argument("A benchmark sweep must contain at least one value");

    return values;
}

std::vector<std::size_t> parseValues(const QString& text)
{
    std::vector<std::size_t> values;

    for (const auto& item : text.split(',', Qt::SkipEmptyParts))
    {
        bool ok = false;
        const auto value = item.trimmed().toULongLong(&ok);

        if (!ok || value == 0)
            throw std::invalid_argument(QStringLiteral("Invalid sweep value: %1").arg(item).toStdString());

        values.push_back(static_cast<std::size_t>(value));
    }

    if (values.empty())
        throw std::invalid_argument("A benchmark sweep must contain at least one value");

    return values;
}

std::vector<std::size_t> resolveCacheCapacities(const QString& text, const std::uint64_t columns, const std::vector<std::size_t>& columnTileSizes)
{
    if (text.trimmed().compare(QStringLiteral("auto"), Qt::CaseInsensitive) != 0)
        return parseValues(text);

    const auto maximumColumnTileCount = *std::max_element(columnTileSizes.begin(), columnTileSizes.end(), [columns](const auto left, const auto right) {
        return (columns + left - 1) / left < (columns + right - 1) / right;
    });
    const auto capacities = std::vector<std::size_t> {
        maximumColumnTileCount,
        std::max<std::size_t>(16, maximumColumnTileCount / 2),
        std::max<std::size_t>(4, maximumColumnTileCount / 4)
    };
    std::vector<std::size_t> uniqueCapacities;

    for (const auto capacity : capacities)
    {
        if (std::find(uniqueCapacities.begin(), uniqueCapacities.end(), capacity) == uniqueCapacities.end())
            uniqueCapacities.push_back(capacity);
    }

    return uniqueCapacities;
}

QString formatValues(const std::vector<std::size_t>& values)
{
    QStringList formattedValues;

    for (const auto value : values)
        formattedValues.push_back(QString::number(value));

    return formattedValues.join(QStringLiteral(", "));
}

ScanLimits scanLimits(const QString& scanMode)
{
    if (scanMode == QStringLiteral("Standard"))
        return { 10000, 1000 };

    if (scanMode == QStringLiteral("Full"))
        return { std::numeric_limits<std::uint64_t>::max(), std::numeric_limits<std::uint64_t>::max() };

    return { 1, 1 };
}

ScanDefaults scanDefaults(const QString& scanMode)
{
    if (scanMode == QStringLiteral("Standard"))
        return { 10000, 1000 };

    if (scanMode == QStringLiteral("Full"))
        return { 100000, 10000 };

    return { 100, 1 };
}

std::vector<std::uint64_t> makeScanIndices(const std::uint64_t dimension, const std::uint64_t maximumCount)
{
    const auto count = std::min(dimension, maximumCount);
    std::vector<std::uint64_t> indices;
    indices.reserve(static_cast<std::size_t>(count));

    if (count == 0)
        return indices;

    if (count == dimension)
    {
        for (std::uint64_t index = 0; index < dimension; ++index)
            indices.push_back(index);

        return indices;
    }

    for (std::uint64_t index = 0; index < count; ++index)
        indices.push_back(count == 1 ? 0 : index * (dimension - 1) / (count - 1));

    return indices;
}

class BenchmarkSettingsAction final : public mv::gui::VerticalGroupAction
{
public:
    BenchmarkSettingsAction(QObject* parent, const QString& title, const bool expanded = false) :
        mv::gui::VerticalGroupAction(parent, title, expanded)
    {
    }

    void fromVariantMap(const QVariantMap& variantMap) override
    {
        mv::gui::VerticalGroupAction::fromVariantMap(variantMap);

        for (auto* action : getActions())
            action->fromParentVariantMap(variantMap, true);
    }

    QVariantMap toVariantMap() const override
    {
        auto variantMap = mv::gui::VerticalGroupAction::toVariantMap();

        for (const auto* action : const_cast<BenchmarkSettingsAction*>(this)->getConstActions())
            action->insertIntoVariantMap(variantMap);

        return variantMap;
    }
};

std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> makeGeneratedStorage(const BenchmarkConfiguration& configuration, const Points::ElementTypeSpecifier type, const std::size_t rowTileSize, const std::size_t columnTileSize)
{
    const auto elementSize = elementSizeForType(type);

    return mv::pointdata::tiled::PointDataTiledStorage::fromGenerator(configuration.rows, configuration.columns, elementSize, rowTileSize, columnTileSize, [columns = configuration.columns, elementSize, type](const auto& descriptor) {
        QByteArray bytes(static_cast<qsizetype>(descriptor.rows * descriptor.columns * elementSize), '\0');

        for (std::uint64_t row = 0; row < descriptor.rows; ++row)
        {
            for (std::uint64_t column = 0; column < descriptor.columns; ++column)
            {
                const auto value = valueAt(descriptor.rowOffset + row, descriptor.columnOffset + column, columns);
                writeBenchmarkValue(bytes.data(), static_cast<std::size_t>((row * descriptor.columns + column) * elementSize), type, value);
            }
        }

        return bytes;
    });
}

std::unique_ptr<ShardedStorage> makeShardedStorage(const BenchmarkConfiguration& configuration, const Points::ElementTypeSpecifier type, const std::size_t rowTileSize, const std::size_t columnTileSize, const mv::util::SharedCodec& codec, const std::function<void(double)>& progress)
{
    const auto source = makeGeneratedStorage(configuration, type, rowTileSize, columnTileSize);
    auto shardedStorage = std::make_unique<ShardedStorage>();
    shardedStorage->directory = std::make_unique<QTemporaryDir>(QDir::tempPath() + QStringLiteral("/pointdata-benchmark-XXXXXX"));

    if (!shardedStorage->directory->isValid())
        throw std::runtime_error("Unable to create a temporary point-data benchmark directory");

    const auto path = shardedStorage->directory->filePath(QStringLiteral("point-data.shard"));
    mv::util::PackedBlobFileWriter writer(path, codec);
    std::vector<mv::util::PackedBlobBlock> blocks;
    blocks.reserve(source->descriptors().size());

    for (std::size_t tileIndex = 0; tileIndex < source->descriptors().size(); ++tileIndex)
    {
        const auto tile = source->acquire(tileIndex / source->tileColumnCount(), tileIndex % source->tileColumnCount());
        blocks.push_back(writer.append(static_cast<const char*>(tile.data()), tile.sizeInBytes()));

        if (progress)
            progress(static_cast<double>(tileIndex + 1) / static_cast<double>(source->descriptors().size()));
    }

    writer.close();
    shardedStorage->storage = mv::pointdata::tiled::PointDataTiledStorage::fromPackedFile(configuration.rows, configuration.columns, elementSizeForType(type), rowTileSize, columnTileSize, path, codec, blocks);
    return shardedStorage;
}

std::vector<std::pair<std::uint64_t, std::uint64_t>> makeRandomCoordinates(const BenchmarkConfiguration& configuration)
{
    std::mt19937_64 random(0x4D616E695661756C);
    std::uniform_int_distribution<std::uint64_t> rowDistribution(0, configuration.rows - 1);
    std::uniform_int_distribution<std::uint64_t> columnDistribution(0, configuration.columns - 1);
    std::vector<std::pair<std::uint64_t, std::uint64_t>> coordinates;
    coordinates.reserve(configuration.samples);

    for (std::size_t sample = 0; sample < configuration.samples; ++sample)
        coordinates.emplace_back(rowDistribution(random), columnDistribution(random));

    return coordinates;
}

std::vector<std::uint64_t> makeRandomRows(const BenchmarkConfiguration& configuration)
{
    std::mt19937_64 random(0x52616E646F6D526F);
    std::uniform_int_distribution<std::uint64_t> rowDistribution(0, configuration.rows - 1);
    std::vector<std::uint64_t> rows;
    rows.reserve(configuration.randomRowSamples);

    for (std::size_t sample = 0; sample < configuration.randomRowSamples; ++sample)
        rows.push_back(rowDistribution(random));

    return rows;
}

template <typename Function>
BenchmarkResult measure(const QString& backend, const QString& elementType, const QString& accessPattern, const std::uint64_t operations, Function function)
{
    const auto start = std::chrono::steady_clock::now();
    const auto checksum = function();
    const auto end = std::chrono::steady_clock::now();

    return { backend, elementType, accessPattern, operations, std::chrono::duration<double, std::milli>(end - start).count(), 0.0, checksum };
}

BenchmarkResult benchmarkStorage(const QString& backend, const QString& elementType, const Points::ElementTypeSpecifier type, const std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage>& storage, const BenchmarkConfiguration& configuration, const std::vector<std::pair<std::uint64_t, std::uint64_t>>& coordinates, const std::vector<std::uint64_t>& randomRows, const mv::workflow::SharedWorkflowExecutionContext& executionContext, const std::size_t phaseIndex, const std::size_t phaseCount, const QString& accessPattern)
{
    const auto operations = accessPattern == QStringLiteral("random") ? coordinates.size() : accessPattern == QStringLiteral("random_row") ? randomRows.size() * configuration.columns : configuration.rows * configuration.columns;
    const auto typeIndex = static_cast<std::uint32_t>(type);
    const auto progressInterval = std::max<std::uint64_t>(1, operations / 100);
    std::uint64_t completedOperations = 0;
    std::uint64_t nextProgressUpdate = progressInterval;
    const auto reportProgress = [&] {
        ++completedOperations;

        if (executionContext && (completedOperations >= nextProgressUpdate || completedOperations == operations))
        {
            const auto phaseProgress = operations == 0 ? 1.0 : static_cast<double>(completedOperations) / static_cast<double>(operations);
            executionContext->setProgress((static_cast<double>(phaseIndex) + phaseProgress) / static_cast<double>(phaseCount));
            nextProgressUpdate = completedOperations + progressInterval;
        }
    };
    const auto result = measure(backend, elementType, accessPattern, operations, [&] {
        double checksum = 0.0;

        if (accessPattern == QStringLiteral("random"))
        {
            for (const auto [row, column] : coordinates)
            {
                checksum += storage->valueAt(row * configuration.columns + column, typeIndex);
                reportProgress();
            }
        }
        else if (accessPattern == QStringLiteral("row_scan"))
        {
            for (std::uint64_t row = 0; row < configuration.rows; ++row)
            {
                for (std::uint64_t column = 0; column < configuration.columns; ++column)
                {
                    checksum += storage->valueAt(row * configuration.columns + column, typeIndex);
                    reportProgress();
                }
            }
        }
        else if (accessPattern == QStringLiteral("random_row"))
        {
            for (const auto row : randomRows)
            {
                for (std::uint64_t column = 0; column < configuration.columns; ++column)
                {
                    checksum += storage->valueAt(row * configuration.columns + column, typeIndex);
                    reportProgress();
                }
            }
        }
        else
        {
            for (std::uint64_t column = 0; column < configuration.columns; ++column)
            {
                for (std::uint64_t row = 0; row < configuration.rows; ++row)
                {
                    checksum += storage->valueAt(row * configuration.columns + column, typeIndex);
                    reportProgress();
                }
            }
        }

        return checksum;
    });

    const auto meanMillisecondsPerRow = accessPattern == QStringLiteral("random_row") && !randomRows.empty() ? result.elapsedMilliseconds / static_cast<double>(randomRows.size()) : 0.0;
    return { result.backend, result.elementType, result.accessPattern, result.operations, result.elapsedMilliseconds, meanMillisecondsPerRow, result.checksum, storage->cacheHits(), storage->cacheMisses(), storage->cacheEvictions() };
}

void writeResult(std::ofstream& output, const BenchmarkResult& result, const std::size_t rowTileSize, const std::size_t columnTileSize, const std::size_t cacheCapacity)
{
    output << result.backend.toStdString() << ',' << result.elementType.toStdString() << ',' << result.accessPattern.toStdString() << ',' << rowTileSize << ',' << columnTileSize << ',' << cacheCapacity << ',' << result.operations << ',' << result.elapsedMilliseconds << ',' << result.meanMillisecondsPerRow << ',' << result.checksum << ',' << result.cacheHits << ',' << result.cacheMisses << ',' << result.cacheEvictions << '\n';
}

struct FixtureResult
{
    QString datasetName;
    QString backend;
    QString elementType;
    QString accessPattern;
    std::uint64_t rows                  = 0;
    std::uint64_t columns               = 0;
    std::size_t rowTileSize             = 0;
    std::size_t columnTileSize          = 0;
    std::size_t cacheCapacity           = 0;
    std::uint64_t operations            = 0;
    double elapsedMilliseconds          = 0.0;
    double meanMillisecondsPerRow       = 0.0;
    double checksum                     = 0.0;
    std::uint64_t cacheHits             = 0;
    std::uint64_t cacheMisses           = 0;
    std::uint64_t cacheEvictions        = 0;
    QString rowAccess;
    QString columnAccess;
    QString rowAccessFrequency;
    QString columnAccessFrequency;
    QString pointAccessFrequency;
    QString memoryPreference;
    std::size_t recommendedRowTileSize       = 0;
    std::size_t recommendedColumnTileSize    = 0;
    std::size_t recommendedCacheCapacity     = 0;
    bool recommendedPackedTiles              = false;
    std::size_t recommendedSuperTileRows     = 0;
    std::size_t recommendedSuperTileColumns = 0;
};

QString backendName(const Points::StorageBackendType backend)
{
    switch (backend)
    {
        case Points::StorageBackendType::Eager:
            return QStringLiteral("eager");

        case Points::StorageBackendType::Tiled:
            return QStringLiteral("tiled");

        case Points::StorageBackendType::Mixed:
            return QStringLiteral("mixed");

        case Points::StorageBackendType::Unknown:
            return QStringLiteral("unknown");
    }

    return QStringLiteral("unknown");
}

QString csvValue(const QString& value)
{
    auto escaped = value;
    escaped.replace('"', QStringLiteral("\"\""));
    return QStringLiteral("\"%1\"").arg(escaped);
}

bool isCancellationRequested(const mv::workflow::SharedWorkflowExecutionContext& executionContext)
{
    const auto task = executionContext ? executionContext->getTask() : nullptr;

    return task && (task->isAboutToBeAborted() || task->isAborting() || task->isAborted());
}

void throwIfCancellationRequested(const mv::workflow::SharedWorkflowExecutionContext& executionContext)
{
    if (isCancellationRequested(executionContext))
        throw std::runtime_error("Point-data benchmark canceled");
}

double checksumView(const PointDataAccessView& view)
{
    double checksum = 0.0;

    for (std::uint64_t row = 0; row < view.rows; ++row)
    {
        const auto* bytes = reinterpret_cast<const unsigned char*>(view.rowData(row));
        const auto byteCount = view.columns * view.elementSizeBytes;

        for (std::uint64_t byte = 0; byte < byteCount; ++byte)
            checksum += bytes[byte];
    }

    return checksum;
}

template <typename Function>
FixtureResult measurePoints(const mv::Dataset<Points>& points, const BenchmarkConfiguration& configuration, const std::vector<std::pair<std::uint64_t, std::uint64_t>>& coordinates, const std::vector<std::uint64_t>& randomRows, const std::vector<std::uint64_t>& scanRows, const std::vector<std::uint64_t>& scanColumns, const mv::workflow::SharedWorkflowExecutionContext& executionContext, const std::size_t phaseIndex, const std::size_t phaseCount, const QString& accessPattern, Function function)
{
    const auto rows       = points->getNumPoints();
    const auto columns    = points->getNumDimensions();
    const auto operations = accessPattern == QStringLiteral("random") ? static_cast<std::uint64_t>(coordinates.size()) : accessPattern == QStringLiteral("row_scan") ? static_cast<std::uint64_t>(scanRows.size()) * columns : accessPattern == QStringLiteral("column_scan") ? rows * static_cast<std::uint64_t>(scanColumns.size()) : static_cast<std::uint64_t>(randomRows.size()) * columns;
    const auto start      = std::chrono::steady_clock::now();
    throwIfCancellationRequested(executionContext);
    const auto checksum   = function(operations);
    const auto end        = std::chrono::steady_clock::now();

    if (executionContext)
        executionContext->setProgress(static_cast<double>(phaseIndex + 1) / static_cast<double>(phaseCount));

    const auto elapsedMilliseconds = std::chrono::duration<double, std::milli>(end - start).count();
    const auto meanMillisecondsPerRow = accessPattern == QStringLiteral("random_row") && !randomRows.empty() ? elapsedMilliseconds / static_cast<double>(randomRows.size()) : 0.0;
    const auto recommendation = points.getDataset()->getProperty(QStringLiteral("PointDataStorageRecommendation")).toMap();

    return {
        points.getDataset()->getGuiName(),
        backendName(points->getStorageBackendType()),
        points->getElementTypeName(),
        accessPattern,
        rows,
        columns,
        points->getTiledRowTileSize(),
        points->getTiledColumnTileSize(),
        points->getTiledCacheCapacity(),
        operations,
        elapsedMilliseconds,
        meanMillisecondsPerRow,
        checksum,
        points->getTiledCacheHits(),
        points->getTiledCacheMisses(),
        points->getTiledCacheEvictions(),
        recommendation.value(QStringLiteral("row_access")).toString(),
        recommendation.value(QStringLiteral("column_access")).toString(),
        recommendation.value(QStringLiteral("row_access_frequency")).toString(),
        recommendation.value(QStringLiteral("column_access_frequency")).toString(),
        recommendation.value(QStringLiteral("point_access_frequency")).toString(),
        recommendation.value(QStringLiteral("memory_preference")).toString(),
        static_cast<std::size_t>(recommendation.value(QStringLiteral("recommended_tile_rows")).toULongLong()),
        static_cast<std::size_t>(recommendation.value(QStringLiteral("recommended_tile_columns")).toULongLong()),
        static_cast<std::size_t>(recommendation.value(QStringLiteral("recommended_cache_capacity")).toULongLong()),
        recommendation.value(QStringLiteral("recommended_packed_tiles")).toBool(),
        static_cast<std::size_t>(recommendation.value(QStringLiteral("recommended_super_tile_rows")).toULongLong()),
        static_cast<std::size_t>(recommendation.value(QStringLiteral("recommended_super_tile_columns")).toULongLong())
    };
}

QString datasetLabel(const mv::Dataset<Points>& dataset)
{
    auto label = QObject::tr("%1 (%2 × %3, %4, %5)").arg(dataset.getDataset()->getGuiName()).arg(dataset->getNumPoints()).arg(dataset->getNumDimensions()).arg(dataset->getElementTypeName()).arg(backendName(dataset->getStorageBackendType()));

    return label;
}

std::vector<BenchmarkDataset> getPointDatasets()
{
    std::vector<BenchmarkDataset> pointDatasets;

    for (const auto& dataset : mv::data().getDatasetsListModel().getDatasets())
    {
        const auto* points = dynamic_cast<const Points*>(dataset.getDataset());

        if (points != nullptr && !points->isProxy() && points->getNumPoints() > 0 && points->getNumDimensions() > 0)
        {
            const mv::Dataset<Points> pointDataset(const_cast<Points*>(points));
            pointDatasets.push_back({ pointDataset, datasetLabel(pointDataset) });
        }
    }

    return pointDatasets;
}

void writeFixtureResult(std::ofstream& output, const FixtureResult& result, const BenchmarkContext& context)
{
    output << csvValue(result.datasetName).toStdString() << ',' << result.backend.toStdString() << ',' << result.elementType.toStdString() << ',' << result.rows << ',' << result.columns << ',' << result.rowTileSize << ',' << result.columnTileSize << ',' << result.cacheCapacity << ',' << result.accessPattern.toStdString() << ',' << result.operations << ',' << result.elapsedMilliseconds << ',' << result.meanMillisecondsPerRow << ',' << result.checksum << ',' << result.cacheHits << ',' << result.cacheMisses << ',' << result.cacheEvictions << ',' << csvValue(result.rowAccess).toStdString() << ',' << csvValue(result.columnAccess).toStdString() << ',' << csvValue(result.rowAccessFrequency).toStdString() << ',' << csvValue(result.columnAccessFrequency).toStdString() << ',' << csvValue(result.pointAccessFrequency).toStdString() << ',' << csvValue(result.memoryPreference).toStdString() << ',' << result.recommendedRowTileSize << ',' << result.recommendedColumnTileSize << ',' << result.recommendedCacheCapacity << ',' << (result.recommendedPackedTiles ? 1 : 0) << ',' << result.recommendedSuperTileRows << ',' << result.recommendedSuperTileColumns << ',' << csvValue(context.benchmarkTimestamp).toStdString() << ',' << csvValue(context.cpuModel).toStdString() << ',' << csvValue(context.cpuArchitecture).toStdString() << ',' << csvValue(context.operatingSystem).toStdString() << ',' << context.logicalCores << ',' << context.physicalMemoryBytes << ',' << csvValue(context.storageVolume).toStdString() << ',' << csvValue(context.storageDevice).toStdString() << ',' << csvValue(context.storageFileSystem).toStdString() << ',' << context.storageTotalBytes << ',' << context.storageAvailableBytes << ',' << (context.projectCompressionEnabled ? 1 : 0) << ',' << csvValue(context.projectCodec).toStdString() << '\n';
}

QString makeTimestampedBenchmarkOutputPath(const QString& outputPath)
{
    const QFileInfo fileInfo(outputPath);
    const auto directory = fileInfo.absolutePath();
    const auto baseName  = fileInfo.completeBaseName().isEmpty() ? QStringLiteral("point-data-benchmark") : fileInfo.completeBaseName();
    const auto suffix    = fileInfo.suffix().isEmpty() ? QStringLiteral("csv") : fileInfo.suffix();
    const auto timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-HHmmss-zzz"));

    return QDir(directory).filePath(QStringLiteral("%1-%2.%3").arg(baseName, timestamp, suffix));
}

class BenchmarkDialog final : public QDialog
{
public:
    explicit BenchmarkDialog(QWidget* parent) :
        QDialog(parent),
        _settingsGroupAction(this, "Point Data Discovery and Benchmark", true),
        _descriptionAction(&_settingsGroupAction, "Description", "The benchmark uses the currently loaded concrete point datasets in the data hierarchy as its fixture. It does not create or reload data. Each access pattern starts with an empty application-level tiled cache; the operating-system file cache is not cleared. Each dataset is measured for random-point, sampled or full row-major, sampled or full column-major, and random-row access. The CSV also records CPU, memory, operating-system, storage-volume, and project-codec context."),
        _datasetsAction(&_settingsGroupAction, "Point datasets"),
        _scanModeAction(&_settingsGroupAction, "Scan mode", { "Quick", "Standard", "Full" }, "Quick"),
        _samplesAction(&_settingsGroupAction, "Random samples", 1, std::numeric_limits<std::int32_t>::max(), 10000),
        _randomRowSamplesAction(&_settingsGroupAction, "Random row samples", 1, std::numeric_limits<std::int32_t>::max(), 100),
        _outputPathAction(&_settingsGroupAction, "CSV output", QDir::homePath() + QStringLiteral("/point-data-benchmark.csv")),
        _previewAction(&_settingsGroupAction, "Preview"),
        _presetsAction(&_settingsGroupAction, &_settingsGroupAction, "Developer/PointDataBenchmark/Presets", "Point Data Benchmark", mv::util::StyledIcon("prescription"))
    {
        setWindowIcon(mv::util::StyledIcon("gauge-high"));
        setWindowTitle(tr("Point Data Discovery and Benchmark"));
        resize(560, 640);

        _settingsGroupAction.setShowLabels(true);
        _settingsGroupAction.setDefaultWidgetFlag(mv::gui::GroupAction::NoMargins);
        _descriptionAction.setDefaultWidgetFlags(mv::gui::StringAction::TextEdit);
        _descriptionAction.setEnabled(false);
        _datasetsAction.setDefaultWidgetFlags(mv::gui::OptionsAction::ListView | mv::gui::OptionsAction::Selection);
        _datasetsAction.setSerializeAllOptions(false);
        _scanModeAction.setDefaultWidgetFlags(mv::gui::OptionAction::ComboBox);
        _samplesAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
        _randomRowSamplesAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
        _outputPathAction.setNameFilters({ "CSV files (*.csv)" });
        _outputPathAction.setDefaultSuffix("csv");
        _previewAction.setDefaultWidgetFlags(mv::gui::StringAction::TextEdit);
        _previewAction.setEnabled(false);
        _presetsAction.setSaveDefaultPresetActionVisible(false);

        const auto datasets = getPointDatasets();
        QStringList datasetLabels;

        for (const auto& dataset : datasets)
            datasetLabels.push_back(dataset.label);

        _datasetsAction.initialize(datasetLabels, datasetLabels);

        _settingsGroupAction.addAction(&_descriptionAction, -1, [](mv::gui::WidgetAction*, QWidget* widget) {
            if (auto* textEdit = widget->findChild<QTextEdit*>(QStringLiteral("TextEdit")))
                textEdit->setFixedHeight(72);
        });
        _settingsGroupAction.addAction(&_datasetsAction);
        _settingsGroupAction.addAction(&_scanModeAction);
        _settingsGroupAction.addAction(&_samplesAction);
        _settingsGroupAction.addAction(&_randomRowSamplesAction);
        _settingsGroupAction.addAction(&_outputPathAction);
        _settingsGroupAction.addAction(&_previewAction);

        connect(&_samplesAction, &mv::gui::IntegralAction::valueChanged, this, [this] { updatePreview(); });
        connect(&_randomRowSamplesAction, &mv::gui::IntegralAction::valueChanged, this, [this] { updatePreview(); });
        connect(&_datasetsAction, &mv::gui::OptionsAction::selectedOptionsChanged, this, [this] { updatePreview(); });

        auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

        auto* presetsButton = new QPushButton(tr("Presets"), this);
        presetsButton->setMenu(_presetsAction.getMenu(presetsButton));

        auto* layout = new QVBoxLayout(this);
        layout->addWidget(_settingsGroupAction.createWidget(this));
        layout->addStretch(1);

        auto* buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(presetsButton);
        buttonLayout->addStretch(1);
        buttonLayout->addWidget(buttons);
        layout->addLayout(buttonLayout);

        loadSettings();
        connect(&_scanModeAction, &mv::gui::OptionAction::currentIndexChanged, this, [this] {
            const auto defaults = scanDefaults(_scanModeAction.getCurrentText());
            _samplesAction.setValue(defaults.randomSamples);
            _randomRowSamplesAction.setValue(defaults.randomRowSamples);
            updatePreview();
        });
        updatePreview();
    }

    BenchmarkConfiguration configuration() const
    {
        return { 0, 0, static_cast<std::size_t>(_samplesAction.getValue()), static_cast<std::size_t>(_randomRowSamplesAction.getValue()), _scanModeAction.getCurrentText(), {}, {}, {}, {}, _datasetsAction.getSelectedOptions() };
    }

    QString outputPath() const { return _outputPathAction.getFilePath(); }

    void saveSettings() const
    {
        auto* application = mv::Application::current();

        application->setSetting("Developer/PointDataBenchmark/Samples", _samplesAction.getValue());
        application->setSetting("Developer/PointDataBenchmark/RandomRowSamples", _randomRowSamplesAction.getValue());
        application->setSetting("Developer/PointDataBenchmark/ScanMode", _scanModeAction.getCurrentText());
        application->setSetting("Developer/PointDataBenchmark/Datasets", _datasetsAction.getSelectedOptions());
        application->setSetting("Developer/PointDataBenchmark/OutputPath", _outputPathAction.getFilePath());
    }

private:
    void loadSettings()
    {
        auto* application = mv::Application::current();

        _samplesAction.setValue(application->getSetting("Developer/PointDataBenchmark/Samples", _samplesAction.getValue()).toInt());
        _randomRowSamplesAction.setValue(application->getSetting("Developer/PointDataBenchmark/RandomRowSamples", _randomRowSamplesAction.getValue()).toInt());
        _scanModeAction.setCurrentText(application->getSetting("Developer/PointDataBenchmark/ScanMode", _scanModeAction.getCurrentText()).toString());
        _datasetsAction.setSelectedOptions(application->getSetting("Developer/PointDataBenchmark/Datasets", _datasetsAction.getSelectedOptions()).toStringList());
        _outputPathAction.setFilePath(application->getSetting("Developer/PointDataBenchmark/OutputPath", _outputPathAction.getFilePath()).toString());
    }

    void updatePreview()
    {
        try
        {
            const auto datasets = getPointDatasets();
            QStringList datasetSummary;
            const auto limits = scanLimits(_scanModeAction.getCurrentText());

            for (const auto& dataset : datasets)
                datasetSummary.push_back(tr("%1%2").arg(_datasetsAction.isOptionSelected(dataset.label) ? QStringLiteral("[enabled] ") : QStringLiteral("[disabled] ")).arg(dataset.label));

            _previewAction.setString(tr("Available point datasets: %1\n%2\nEnabled datasets: %3\nScan mode: %4 (up to %5 rows and %6 columns per scan)\nRandom point samples per dataset: %7\nRandom row samples per dataset: %8\nMeasurements: %9 per enabled dataset (random point, row-major scan, column-major scan, random-row mean)\nTile sizes and cache capacities are read from each loaded dataset.")
                .arg(datasets.size())
                .arg(datasetSummary.isEmpty() ? tr("No point datasets are currently loaded") : datasetSummary.join('\n'))
                .arg(_datasetsAction.getSelectedOptions().size())
                .arg(_scanModeAction.getCurrentText())
                .arg(limits.rows == std::numeric_limits<std::uint64_t>::max() ? tr("all") : QString::number(limits.rows))
                .arg(limits.columns == std::numeric_limits<std::uint64_t>::max() ? tr("all") : QString::number(limits.columns))
                .arg(_samplesAction.getValue())
                .arg(_randomRowSamplesAction.getValue())
                .arg(_datasetsAction.getSelectedOptions().size() * 4));
        }
        catch (const std::exception& exception)
        {
            _previewAction.setString(tr("Invalid settings: %1").arg(QString::fromUtf8(exception.what())));
        }
    }

    BenchmarkSettingsAction     _settingsGroupAction;
    mv::gui::StringAction        _descriptionAction;
    mv::gui::OptionsAction       _datasetsAction;
    mv::gui::OptionAction        _scanModeAction;
    mv::gui::IntegralAction      _samplesAction;
    mv::gui::IntegralAction      _randomRowSamplesAction;
    mv::gui::FilePickerAction    _outputPathAction;
    mv::gui::StringAction        _previewAction;
    mv::gui::PresetsAction       _presetsAction;
};

void runBenchmark(QWidget* owner, const BenchmarkConfiguration& configuration, const QString& outputPath)
{
    const auto availableDatasets = getPointDatasets();
    std::vector<BenchmarkDataset> datasets;

    for (const auto& dataset : availableDatasets)
    {
        if (configuration.datasetLabels.contains(dataset.label))
            datasets.push_back(dataset);
    }

    if (datasets.empty())
        throw std::runtime_error("Enable at least one non-empty point dataset before running the benchmark");

    const auto benchmarkOutputPath = makeTimestampedBenchmarkOutputPath(outputPath);
    const auto output              = std::make_shared<std::ofstream>(benchmarkOutputPath.toStdString(), std::ios::trunc);

    if (!*output)
        throw std::runtime_error("Unable to open the benchmark output file");

    *output << "dataset,backend,element_type,rows,columns,row_tile_size,column_tile_size,cache_capacity,access_pattern,operations,milliseconds,mean_milliseconds_per_row,checksum,cache_hits,cache_misses,evictions,row_access,column_access,row_access_frequency,column_access_frequency,point_access_frequency,memory_preference,recommended_tile_rows,recommended_tile_columns,recommended_cache_capacity,recommended_packed_tiles,recommended_super_tile_rows,recommended_super_tile_columns,benchmark_timestamp,cpu_model,cpu_architecture,operating_system,logical_cores,physical_memory_bytes,storage_volume,storage_device,storage_filesystem,storage_total_bytes,storage_available_bytes,project_compression_enabled,project_codec\n";

    const auto benchmarkContext = collectBenchmarkContext();
    auto plan = std::make_unique<mv::workflow::WorkflowPlan>(QObject::tr("Point-data benchmark"));
    mv::workflow::WorkflowPlan::Jobs benchmarkJobs;
    benchmarkJobs.reserve(datasets.size());

    for (const auto& points : datasets)
    {
        benchmarkJobs.emplace_back(points.label, [configuration, points, output, benchmarkContext](const mv::workflow::WorkflowPlan::Job&, const mv::workflow::SharedWorkflowExecutionContext& executionContext) {
            BenchmarkConfiguration datasetConfiguration = configuration;
            const auto& pointDataset = points.points;
            datasetConfiguration.rows = pointDataset->getNumPoints();
            datasetConfiguration.columns = pointDataset->getNumDimensions();
            const auto coordinates = makeRandomCoordinates(datasetConfiguration);
            const auto randomRows = makeRandomRows(datasetConfiguration);
            const auto limits = scanLimits(datasetConfiguration.scanMode);
            const auto scanRows = makeScanIndices(datasetConfiguration.rows, limits.rows);
            const auto scanColumns = makeScanIndices(datasetConfiguration.columns, limits.columns);
            const auto phaseCount = std::size_t { 4 };

            const auto measureAccess = [&](const QString& accessPattern, const std::size_t phaseIndex) {
                pointDataset->resetTiledCache();

                return measurePoints(pointDataset, datasetConfiguration, coordinates, randomRows, scanRows, scanColumns, executionContext, phaseIndex, phaseCount, accessPattern, [&](const std::uint64_t operations) {
                    double checksum = 0.0;
                    std::uint64_t completedOperations = 0;
                    const auto progressInterval = std::max<std::uint64_t>(1, operations / 100);
                    const auto reportProgress = [&](const std::uint64_t count) {
                        throwIfCancellationRequested(executionContext);
                        completedOperations += count;

                        if (executionContext && (completedOperations >= progressInterval || completedOperations == operations))
                            executionContext->setProgress((static_cast<double>(phaseIndex) + std::min(1.0, static_cast<double>(completedOperations) / static_cast<double>(std::max<std::uint64_t>(1, operations)))) / static_cast<double>(phaseCount));
                    };

                    if (accessPattern == QStringLiteral("random"))
                    {
                        for (const auto [row, column] : coordinates)
                        {
                            checksum += pointDataset->getValueAt(row * datasetConfiguration.columns + column);
                            reportProgress(1);
                        }
                    }
                    else
                    {
                        const auto visitor = [&](const PointDataAccessView& view) {
                            checksum += checksumView(view);
                            reportProgress(view.rows * view.columns);
                        };

                        if (accessPattern == QStringLiteral("row_scan"))
                        {
                            if (datasetConfiguration.scanMode == QStringLiteral("Full"))
                            {
                                for (std::uint64_t firstRow = 0; firstRow < datasetConfiguration.rows; firstRow += 1024)
                                    pointDataset->visitRows(firstRow, std::min<std::uint64_t>(1024, datasetConfiguration.rows - firstRow), visitor);
                            }
                            else
                            {
                                for (std::size_t firstRow = 0; firstRow < scanRows.size(); firstRow += 128)
                                {
                                    const auto lastRow = std::min<std::size_t>(scanRows.size(), firstRow + 128);
                                    pointDataset->visitRows(std::vector<std::uint64_t>(scanRows.begin() + firstRow, scanRows.begin() + lastRow), visitor);
                                }
                            }
                        }
                        else if (accessPattern == QStringLiteral("column_scan"))
                        {
                            if (datasetConfiguration.scanMode == QStringLiteral("Full"))
                            {
                                for (std::uint64_t firstColumn = 0; firstColumn < datasetConfiguration.columns; firstColumn += 64)
                                    pointDataset->visitColumns(firstColumn, std::min<std::uint64_t>(64, datasetConfiguration.columns - firstColumn), visitor);
                            }
                            else
                            {
                                for (std::size_t firstColumn = 0; firstColumn < scanColumns.size(); firstColumn += 64)
                                {
                                    const auto lastColumn = std::min<std::size_t>(scanColumns.size(), firstColumn + 64);
                                    pointDataset->visitColumns(std::vector<std::uint64_t>(scanColumns.begin() + firstColumn, scanColumns.begin() + lastColumn), visitor);
                                }
                            }
                        }
                        else
                        {
                            for (std::size_t firstRow = 0; firstRow < randomRows.size(); firstRow += 128)
                            {
                                const auto lastRow = std::min<std::size_t>(randomRows.size(), firstRow + 128);
                                pointDataset->visitRows(std::vector<std::uint64_t>(randomRows.begin() + firstRow, randomRows.begin() + lastRow), visitor);
                            }
                        }
                    }

                    return checksum;
                });
            };

            writeFixtureResult(*output, measureAccess(QStringLiteral("random"), 0), benchmarkContext);
            writeFixtureResult(*output, measureAccess(QStringLiteral("row_scan"), 1), benchmarkContext);
            writeFixtureResult(*output, measureAccess(QStringLiteral("column_scan"), 2), benchmarkContext);
            writeFixtureResult(*output, measureAccess(QStringLiteral("random_row"), 3), benchmarkContext);
        });
    }

    plan->addSequentialStage(QObject::tr("Benchmark datasets"), std::move(benchmarkJobs));
    plan->addFinalizationStage(QObject::tr("Close benchmark output"), [output] {
        output->flush();
    });

    mv::workflow::WorkflowOptions options;
    options.cancellation.enabled = true;
    options.reporting.progress = true;
    options.reporting.finishedNotification = true;

    const auto benchmarkStart = std::chrono::steady_clock::now();
    auto future = mv::Application::getWorkflowPlanExecutor().execute(std::move(plan), nullptr, options);
    future.onFinished(owner, [output, benchmarkOutputPath, benchmarkStart](const mv::workflow::SharedWorkflowResult& result) {
        output->flush();

        const auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - benchmarkStart).count();
        const auto elapsedTime = QObject::tr("%1 s").arg(static_cast<double>(elapsedMilliseconds) / 1000.0, 0, 'f', 2);

        if (!result || result->getStatus() == mv::workflow::WorkflowResultBase::Status::Canceled)
        {
            mv::help().addNotification(QObject::tr("Point-data benchmark canceled"), QObject::tr("The benchmark was canceled after %1. Partial results were saved to %2.").arg(elapsedTime, benchmarkOutputPath), mv::util::StyledIcon("stop"));
            return;
        }

        if (result->getStatus() != mv::workflow::WorkflowResultBase::Status::Success)
        {
            mv::help().addNotification(QObject::tr("Point-data benchmark failed"), QObject::tr("The benchmark failed after %1. Partial results were saved to %2.").arg(elapsedTime, benchmarkOutputPath), mv::util::StyledIcon("times"));
            return;
        }

        mv::help().addNotification(QObject::tr("Point-data benchmark completed"), QObject::tr("The benchmark completed in %1. Results were saved to %2.").arg(elapsedTime, benchmarkOutputPath), mv::util::StyledIcon("check"));
    });
}
}

void PointDataBenchmark::populateMenu(QMenu& menu, QWidget* owner)
{
    auto* action = menu.addAction(mv::util::StyledIcon("gauge-high"), QObject::tr("Point Data Discovery and Benchmark…"));
    action->setToolTip(QObject::tr("Discover and benchmark selected concrete point datasets using random-point, row, column, and random-row access"));

    QObject::connect(action, &QAction::triggered, owner, [owner] {
        BenchmarkDialog dialog(owner);
        if (dialog.exec() != QDialog::Accepted)
            return;

         dialog.saveSettings();

        try
        {
            const auto configuration = dialog.configuration();
            runBenchmark(owner, configuration, dialog.outputPath());
        }
        catch (const std::exception& exception)
        {
            QMessageBox::critical(owner, QObject::tr("Point-data benchmark failed"), QString::fromUtf8(exception.what()));
        }
    });
}

namespace
{
const bool registered = [] {
    mv::developer::registerMenuPopulator(QStringLiteral("PointData.PerformanceTesting.Benchmark"), [](QMenu& menu, QWidget* owner) {
        PointDataBenchmark::populateMenu(menu, owner);
    });
    return true;
}();
}
