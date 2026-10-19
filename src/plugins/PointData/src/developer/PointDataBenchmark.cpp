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
#include <actions/OptionAction.h>
#include <actions/PresetsAction.h>
#include <actions/StringAction.h>
#include <actions/VerticalGroupAction.h>
#include <util/StyledIcon.h>

#include <workflow/WorkflowOptions.h>
#include <workflow/WorkflowExecutionContext.h>
#include <workflow/WorkflowPlan.h>

#include "../tiled/PackedTileFile.h"
#include "../tiled/PointDataTiledStorage.h"

#include <QCoreApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QEventLoop>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTemporaryDir>
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

namespace
{
struct BenchmarkConfiguration
{
    std::uint64_t rows      = 10000;
    std::uint64_t columns   = 128;
    std::size_t samples     = 10000;
    std::size_t randomRowSamples = 100;
    QString elementType     = QStringLiteral("float32");
    QString rowTileSizes    = QStringLiteral("auto");
    QString columnTileSizes = QStringLiteral("auto");
    QString cacheCapacities = QStringLiteral("auto");
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
    mv::pointdata::tiled::PackedTileFileWriter writer(path, codec);
    std::vector<mv::pointdata::tiled::PackedTileBlock> blocks;
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
FixtureResult measurePoints(const mv::Dataset<Points>& points, const BenchmarkConfiguration& configuration, const std::vector<std::pair<std::uint64_t, std::uint64_t>>& coordinates, const std::vector<std::uint64_t>& randomRows, const mv::workflow::SharedWorkflowExecutionContext& executionContext, const std::size_t phaseIndex, const std::size_t phaseCount, const QString& accessPattern, Function function)
{
    const auto rows       = points->getNumPoints();
    const auto columns    = points->getNumDimensions();
    const auto operations = accessPattern == QStringLiteral("random") ? static_cast<std::uint64_t>(coordinates.size()) : accessPattern == QStringLiteral("random_row") ? static_cast<std::uint64_t>(randomRows.size()) * columns : rows * columns;
    const auto start      = std::chrono::steady_clock::now();
    const auto checksum   = function(operations);
    const auto end        = std::chrono::steady_clock::now();

    if (executionContext)
        executionContext->setProgress(static_cast<double>(phaseIndex + 1) / static_cast<double>(phaseCount));

    const auto elapsedMilliseconds = std::chrono::duration<double, std::milli>(end - start).count();
    const auto meanMillisecondsPerRow = accessPattern == QStringLiteral("random_row") && !randomRows.empty() ? elapsedMilliseconds / static_cast<double>(randomRows.size()) : 0.0;

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
        points->getTiledCacheEvictions()
    };
}

std::vector<mv::Dataset<Points>> getPointDatasets()
{
    std::vector<mv::Dataset<Points>> pointDatasets;

    for (const auto& dataset : mv::data().getDatasetsListModel().getDatasets())
    {
        const auto* points = dynamic_cast<const Points*>(dataset.getDataset());

        if (points != nullptr && !points->isProxy() && points->getNumPoints() > 0 && points->getNumDimensions() > 0)
            pointDatasets.emplace_back(const_cast<Points*>(points));
    }

    return pointDatasets;
}

void writeFixtureResult(std::ofstream& output, const FixtureResult& result)
{
    output << csvValue(result.datasetName).toStdString() << ',' << result.backend.toStdString() << ',' << result.elementType.toStdString() << ',' << result.rows << ',' << result.columns << ',' << result.rowTileSize << ',' << result.columnTileSize << ',' << result.cacheCapacity << ',' << result.accessPattern.toStdString() << ',' << result.operations << ',' << result.elapsedMilliseconds << ',' << result.meanMillisecondsPerRow << ',' << result.checksum << ',' << result.cacheHits << ',' << result.cacheMisses << ',' << result.cacheEvictions << '\n';
}

class BenchmarkDialog final : public QDialog
{
public:
    explicit BenchmarkDialog(QWidget* parent) :
        QDialog(parent),
        _settingsGroupAction(this, "Point-data benchmark sweep", true),
        _descriptionAction(&_settingsGroupAction, "Description", "The benchmark uses the currently loaded concrete point datasets in the data hierarchy as its fixture. It does not create or reload data. Each dataset is measured for random-point, full row, full column, and random-row access."),
        _samplesAction(&_settingsGroupAction, "Random samples", 1, std::numeric_limits<std::int32_t>::max(), 10000),
        _randomRowSamplesAction(&_settingsGroupAction, "Random row samples", 1, std::numeric_limits<std::int32_t>::max(), 100),
        _outputPathAction(&_settingsGroupAction, "CSV output", QDir::homePath() + QStringLiteral("/point-data-benchmark.csv")),
        _previewAction(&_settingsGroupAction, "Preview"),
        _presetsAction(&_settingsGroupAction, &_settingsGroupAction, "Developer/PointDataBenchmark/Presets", "Benchmark sweep", mv::util::StyledIcon("prescription"))
    {
        setWindowIcon(mv::util::StyledIcon("gauge-high"));
        setWindowTitle(tr("Point-data benchmark sweep"));
        resize(560, 640);

        _settingsGroupAction.setShowLabels(true);
        _descriptionAction.setDefaultWidgetFlags(mv::gui::StringAction::TextEdit);
        _descriptionAction.setEnabled(false);
        _samplesAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
        _randomRowSamplesAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
        _outputPathAction.setNameFilters({ "CSV files (*.csv)" });
        _outputPathAction.setDefaultSuffix("csv");
        _previewAction.setDefaultWidgetFlags(mv::gui::StringAction::TextEdit);
        _previewAction.setEnabled(false);

        _settingsGroupAction.addAction(&_descriptionAction, -1, [](mv::gui::WidgetAction*, QWidget* widget) {
            if (auto* textEdit = widget->findChild<QTextEdit*>(QStringLiteral("TextEdit")))
                textEdit->setFixedHeight(72);
        });
        _settingsGroupAction.addAction(&_samplesAction);
        _settingsGroupAction.addAction(&_randomRowSamplesAction);
        _settingsGroupAction.addAction(&_outputPathAction);
        _settingsGroupAction.addAction(&_previewAction);

        connect(&_samplesAction, &mv::gui::IntegralAction::valueChanged, this, [this] { updatePreview(); });
        connect(&_randomRowSamplesAction, &mv::gui::IntegralAction::valueChanged, this, [this] { updatePreview(); });

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
        updatePreview();
    }

    BenchmarkConfiguration configuration() const
    {
        return { 0, 0, static_cast<std::size_t>(_samplesAction.getValue()), static_cast<std::size_t>(_randomRowSamplesAction.getValue()), {}, {}, {}, {} };
    }

    QString outputPath() const { return _outputPathAction.getFilePath(); }

    void saveSettings() const
    {
        auto* application = mv::Application::current();

        application->setSetting("Developer/PointDataBenchmark/Samples", _samplesAction.getValue());
        application->setSetting("Developer/PointDataBenchmark/RandomRowSamples", _randomRowSamplesAction.getValue());
        application->setSetting("Developer/PointDataBenchmark/OutputPath", _outputPathAction.getFilePath());
    }

private:
    void loadSettings()
    {
        auto* application = mv::Application::current();

        _samplesAction.setValue(application->getSetting("Developer/PointDataBenchmark/Samples", _samplesAction.getValue()).toInt());
        _randomRowSamplesAction.setValue(application->getSetting("Developer/PointDataBenchmark/RandomRowSamples", _randomRowSamplesAction.getValue()).toInt());
        _outputPathAction.setFilePath(application->getSetting("Developer/PointDataBenchmark/OutputPath", _outputPathAction.getFilePath()).toString());
    }

    void updatePreview()
    {
        try
        {
            const auto datasets = getPointDatasets();
            QStringList datasetSummary;

            for (const auto& dataset : datasets)
                datasetSummary.push_back(tr("%1 (%2 × %3, %4, %5)").arg(dataset.getDataset()->getGuiName()).arg(dataset->getNumPoints()).arg(dataset->getNumDimensions()).arg(dataset->getElementTypeName()).arg(backendName(dataset->getStorageBackendType())));

            _previewAction.setString(tr("Fixture datasets: %1\n%2\nRandom point samples per dataset: %3\nRandom row samples per dataset: %4\nMeasurements: %5 per dataset (random point, row scan, column scan, random-row mean)\nTile sizes and cache capacities are read from each loaded dataset.")
                .arg(datasets.size())
                .arg(datasetSummary.isEmpty() ? tr("No point datasets are currently loaded") : datasetSummary.join('\n'))
                .arg(_samplesAction.getValue())
                .arg(_randomRowSamplesAction.getValue())
                .arg(datasets.size() * 4));
        }
        catch (const std::exception& exception)
        {
            _previewAction.setString(tr("Invalid settings: %1").arg(QString::fromUtf8(exception.what())));
        }
    }

    BenchmarkSettingsAction     _settingsGroupAction;
    mv::gui::StringAction        _descriptionAction;
    mv::gui::IntegralAction      _samplesAction;
    mv::gui::IntegralAction      _randomRowSamplesAction;
    mv::gui::FilePickerAction    _outputPathAction;
    mv::gui::StringAction        _previewAction;
    mv::gui::PresetsAction       _presetsAction;
};

void runBenchmark(QWidget* owner, const BenchmarkConfiguration& configuration, const QString& outputPath)
{
    const auto datasets = getPointDatasets();

    if (datasets.empty())
        throw std::runtime_error("At least one non-empty point dataset must be loaded in the data hierarchy before running the benchmark");

    const auto output = std::make_shared<std::ofstream>(outputPath.toStdString(), std::ios::trunc);

    if (!*output)
        throw std::runtime_error("Unable to open the benchmark output file");

    *output << "dataset,backend,element_type,rows,columns,row_tile_size,column_tile_size,cache_capacity,access_pattern,operations,milliseconds,mean_milliseconds_per_row,checksum,cache_hits,cache_misses,evictions\n";

    auto plan = std::make_unique<mv::workflow::WorkflowPlan>(QObject::tr("Point-data benchmark"));
    mv::workflow::WorkflowPlan::Jobs benchmarkJobs;
    benchmarkJobs.reserve(datasets.size());

    for (const auto& points : datasets)
    {
        benchmarkJobs.emplace_back(QStringLiteral("%1: %2, %3 × %4, %5").arg(points.getDataset()->getGuiName()).arg(points->getElementTypeName()).arg(points->getNumPoints()).arg(points->getNumDimensions()).arg(backendName(points->getStorageBackendType())), [configuration, points, output](const mv::workflow::WorkflowPlan::Job&, const mv::workflow::SharedWorkflowExecutionContext& executionContext) {
            BenchmarkConfiguration datasetConfiguration = configuration;
            datasetConfiguration.rows = points->getNumPoints();
            datasetConfiguration.columns = points->getNumDimensions();
            const auto coordinates = makeRandomCoordinates(datasetConfiguration);
            const auto randomRows = makeRandomRows(datasetConfiguration);
            const auto phaseCount = std::size_t { 4 };

            const auto measureAccess = [&](const QString& accessPattern, const std::size_t phaseIndex) {
                return measurePoints(points, datasetConfiguration, coordinates, randomRows, executionContext, phaseIndex, phaseCount, accessPattern, [&](const std::uint64_t operations) {
                    double checksum = 0.0;
                    std::uint64_t completedOperations = 0;
                    const auto progressInterval = std::max<std::uint64_t>(1, operations / 100);
                    const auto reportProgress = [&](const std::uint64_t count) {
                        completedOperations += count;

                        if (executionContext && (completedOperations >= progressInterval || completedOperations == operations))
                            executionContext->setProgress((static_cast<double>(phaseIndex) + std::min(1.0, static_cast<double>(completedOperations) / static_cast<double>(std::max<std::uint64_t>(1, operations)))) / static_cast<double>(phaseCount));
                    };

                    if (accessPattern == QStringLiteral("random"))
                    {
                        for (const auto [row, column] : coordinates)
                        {
                            checksum += points->getValueAt(row * datasetConfiguration.columns + column);
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
                            for (std::uint64_t firstRow = 0; firstRow < datasetConfiguration.rows; firstRow += 1024)
                                points->visitRows(firstRow, std::min<std::uint64_t>(1024, datasetConfiguration.rows - firstRow), visitor);
                        }
                        else if (accessPattern == QStringLiteral("column_scan"))
                        {
                            for (std::uint64_t firstColumn = 0; firstColumn < datasetConfiguration.columns; firstColumn += 64)
                                points->visitColumns(firstColumn, std::min<std::uint64_t>(64, datasetConfiguration.columns - firstColumn), visitor);
                        }
                        else
                        {
                            for (std::size_t firstRow = 0; firstRow < randomRows.size(); firstRow += 128)
                            {
                                const auto lastRow = std::min<std::size_t>(randomRows.size(), firstRow + 128);
                                points->visitRows(std::vector<std::uint64_t>(randomRows.begin() + firstRow, randomRows.begin() + lastRow), visitor);
                            }
                        }
                    }

                    return checksum;
                });
            };

            writeFixtureResult(*output, measureAccess(QStringLiteral("random"), 0));
            writeFixtureResult(*output, measureAccess(QStringLiteral("row_scan"), 1));
            writeFixtureResult(*output, measureAccess(QStringLiteral("column_scan"), 2));
            writeFixtureResult(*output, measureAccess(QStringLiteral("random_row"), 3));
        });
    }

    plan->addSequentialStage(QObject::tr("Benchmark datasets"), std::move(benchmarkJobs));
    plan->addFinalizationStage(QObject::tr("Close benchmark output"), [output] {
        output->flush();
    });

    mv::workflow::WorkflowOptions options;
    options.reporting.progress = true;
    options.reporting.finishedNotification = false;

    auto future = mv::Application::getWorkflowPlanExecutor().execute(std::move(plan), nullptr, options);
    future.onFinished(owner, [output, outputPath](const mv::workflow::SharedWorkflowResult&) {
        output->flush();
        mv::help().addNotification(QObject::tr("Point-data benchmark completed"), QObject::tr("Benchmark results were saved to %1.").arg(outputPath), mv::util::StyledIcon("check"));
    });
}
}

void PointDataBenchmark::populateMenu(QMenu& menu, QWidget* owner)
{
    auto* action = menu.addAction(mv::util::StyledIcon("gauge-high"), QObject::tr("Point-data benchmark sweep…"));
    action->setToolTip(QObject::tr("Benchmark the loaded concrete point datasets using random-point, row, column, and random-row access"));

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
