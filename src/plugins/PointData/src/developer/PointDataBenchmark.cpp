// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "PointDataBenchmark.h"

#include <CoreInterface.h>
#include <DeveloperActionRegistry.h>
#include <Application.h>
#include <Project.h>
#include <actions/FilePickerAction.h>
#include <actions/IntegralAction.h>
#include <actions/StringAction.h>
#include <actions/VerticalGroupAction.h>
#include <util/StyledIcon.h>

#include <workflow/WorkflowOptions.h>
#include <workflow/WorkflowPlan.h>

#include "../tiled/PackedTileFile.h"
#include "../tiled/PointDataTiledStorage.h"

#include <QCoreApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QEventLoop>
#include <QMessageBox>
#include <QTemporaryDir>
#include <QVBoxLayout>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
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
    QString rowTileSizes    = QStringLiteral("auto");
    QString columnTileSizes = QStringLiteral("auto");
    QString cacheCapacities = QStringLiteral("auto");
};

struct BenchmarkResult
{
    QString backend;
    QString accessPattern;
    std::uint64_t operations     = 0;
    double elapsedMilliseconds   = 0.0;
    double checksum              = 0.0;
    std::uint64_t cacheHits      = 0;
    std::uint64_t cacheMisses    = 0;
    std::uint64_t cacheEvictions = 0;
};

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

std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> makeGeneratedStorage(const BenchmarkConfiguration& configuration, const std::size_t rowTileSize, const std::size_t columnTileSize)
{
    return mv::pointdata::tiled::PointDataTiledStorage::fromGenerator(configuration.rows, configuration.columns, sizeof(float), rowTileSize, columnTileSize, [columns = configuration.columns](const auto& descriptor) {
        QByteArray bytes(static_cast<qsizetype>(descriptor.rows * descriptor.columns * sizeof(float)), '\0');

        for (std::uint64_t row = 0; row < descriptor.rows; ++row)
        {
            for (std::uint64_t column = 0; column < descriptor.columns; ++column)
            {
                const auto value = valueAt(descriptor.rowOffset + row, descriptor.columnOffset + column, columns);
                std::memcpy(bytes.data() + (row * descriptor.columns + column) * sizeof(float), &value, sizeof(value));
            }
        }

        return bytes;
    });
}

std::unique_ptr<ShardedStorage> makeShardedStorage(const BenchmarkConfiguration& configuration, const std::size_t rowTileSize, const std::size_t columnTileSize, const mv::util::SharedCodec& codec)
{
    const auto source = makeGeneratedStorage(configuration, rowTileSize, columnTileSize);
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
    }

    writer.close();
    shardedStorage->storage = mv::pointdata::tiled::PointDataTiledStorage::fromPackedFile(configuration.rows, configuration.columns, sizeof(float), rowTileSize, columnTileSize, path, codec, blocks);
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

template <typename Function>
BenchmarkResult measure(const QString& backend, const QString& accessPattern, const std::uint64_t operations, Function function)
{
    const auto start = std::chrono::steady_clock::now();
    const auto checksum = function();
    const auto end = std::chrono::steady_clock::now();

    return { backend, accessPattern, operations, std::chrono::duration<double, std::milli>(end - start).count(), checksum };
}

BenchmarkResult benchmarkStorage(const QString& backend, const std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage>& storage, const BenchmarkConfiguration& configuration, const std::vector<std::pair<std::uint64_t, std::uint64_t>>& coordinates, const QString& accessPattern)
{
    const auto result = measure(backend, accessPattern, accessPattern == QStringLiteral("random") ? coordinates.size() : configuration.rows * configuration.columns, [&] {
        double checksum = 0.0;

        if (accessPattern == QStringLiteral("random"))
        {
            for (const auto [row, column] : coordinates)
                checksum += storage->valueAt(row * configuration.columns + column, 0);
        }
        else if (accessPattern == QStringLiteral("row_scan"))
        {
            for (std::uint64_t row = 0; row < configuration.rows; ++row)
            {
                for (std::uint64_t column = 0; column < configuration.columns; ++column)
                    checksum += storage->valueAt(row * configuration.columns + column, 0);
            }
        }
        else
        {
            for (std::uint64_t column = 0; column < configuration.columns; ++column)
            {
                for (std::uint64_t row = 0; row < configuration.rows; ++row)
                    checksum += storage->valueAt(row * configuration.columns + column, 0);
            }
        }

        return checksum;
    });

    return { result.backend, result.accessPattern, result.operations, result.elapsedMilliseconds, result.checksum, storage->cacheHits(), storage->cacheMisses(), storage->cacheEvictions() };
}

void writeResult(std::ofstream& output, const BenchmarkResult& result, const std::size_t rowTileSize, const std::size_t columnTileSize, const std::size_t cacheCapacity)
{
    output << result.backend.toStdString() << ',' << result.accessPattern.toStdString() << ',' << rowTileSize << ',' << columnTileSize << ',' << cacheCapacity << ',' << result.operations << ',' << result.elapsedMilliseconds << ',' << result.checksum << ',' << result.cacheHits << ',' << result.cacheMisses << ',' << result.cacheEvictions << '\n';
}

class BenchmarkDialog final : public QDialog
{
public:
    explicit BenchmarkDialog(QWidget* parent) :
        QDialog(parent),
        _settingsGroupAction(this, "Point-data benchmark sweep", true),
        _descriptionAction(&_settingsGroupAction, "Description", "Each combination is measured for random, row-major, and column-major access. Use 'auto' for dataset-scaled tile sizes. Results use the active project compression codec."),
        _rowsAction(&_settingsGroupAction, "Rows", 1, std::numeric_limits<std::int32_t>::max(), 10000),
        _columnsAction(&_settingsGroupAction, "Columns", 1, std::numeric_limits<std::int32_t>::max(), 128),
        _samplesAction(&_settingsGroupAction, "Random samples", 1, std::numeric_limits<std::int32_t>::max(), 10000),
        _rowTileSizesAction(&_settingsGroupAction, "Row tile sizes", "auto"),
        _columnTileSizesAction(&_settingsGroupAction, "Column tile sizes", "auto"),
        _cacheCapacitiesAction(&_settingsGroupAction, "Cache capacities", "auto"),
        _outputPathAction(&_settingsGroupAction, "CSV output", QDir::homePath() + QStringLiteral("/point-data-benchmark.csv")),
        _previewAction(&_settingsGroupAction, "Preview")
    {
        setWindowIcon(mv::util::StyledIcon("gauge-high"));
        setWindowTitle(tr("Point-data benchmark sweep"));
        resize(560, 420);

        _settingsGroupAction.setShowLabels(true);
        _descriptionAction.setDefaultWidgetFlags(mv::gui::StringAction::TextEdit);
        _descriptionAction.setEnabled(false);
        _rowsAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
        _columnsAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
        _samplesAction.setDefaultWidgetFlags(mv::gui::IntegralAction::SpinBox);
        _rowTileSizesAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
        _columnTileSizesAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
        _cacheCapacitiesAction.setDefaultWidgetFlags(mv::gui::StringAction::LineEdit);
        _outputPathAction.setNameFilters({ "CSV files (*.csv)" });
        _outputPathAction.setDefaultSuffix("csv");
        _previewAction.setDefaultWidgetFlags(mv::gui::StringAction::TextEdit);
        _previewAction.setEnabled(false);

        _settingsGroupAction.addAction(&_descriptionAction);
        _settingsGroupAction.addAction(&_rowsAction);
        _settingsGroupAction.addAction(&_columnsAction);
        _settingsGroupAction.addAction(&_samplesAction);
        _settingsGroupAction.addAction(&_rowTileSizesAction);
        _settingsGroupAction.addAction(&_columnTileSizesAction);
        _settingsGroupAction.addAction(&_cacheCapacitiesAction);
        _settingsGroupAction.addAction(&_outputPathAction);
        _settingsGroupAction.addAction(&_previewAction);

        connect(&_rowsAction, &mv::gui::IntegralAction::valueChanged, this, [this] { updatePreview(); });
        connect(&_columnsAction, &mv::gui::IntegralAction::valueChanged, this, [this] { updatePreview(); });
        connect(&_samplesAction, &mv::gui::IntegralAction::valueChanged, this, [this] { updatePreview(); });
        connect(&_rowTileSizesAction, &mv::gui::StringAction::stringChanged, this, [this] { updatePreview(); });
        connect(&_columnTileSizesAction, &mv::gui::StringAction::stringChanged, this, [this] { updatePreview(); });
        connect(&_cacheCapacitiesAction, &mv::gui::StringAction::stringChanged, this, [this] { updatePreview(); });

        auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

        auto* layout = new QVBoxLayout(this);
        layout->addWidget(_settingsGroupAction.createWidget(this));
        layout->addStretch(1);
        layout->addWidget(buttons);

        loadSettings();
        updatePreview();
    }

    BenchmarkConfiguration configuration() const
    {
        return { static_cast<std::uint64_t>(_rowsAction.getValue()), static_cast<std::uint64_t>(_columnsAction.getValue()), static_cast<std::size_t>(_samplesAction.getValue()), _rowTileSizesAction.getString(), _columnTileSizesAction.getString(), _cacheCapacitiesAction.getString() };
    }

    QString outputPath() const { return _outputPathAction.getFilePath(); }

    void saveSettings() const
    {
        auto* application = mv::Application::current();

        application->setSetting("Developer/PointDataBenchmark/Rows", _rowsAction.getValue());
        application->setSetting("Developer/PointDataBenchmark/Columns", _columnsAction.getValue());
        application->setSetting("Developer/PointDataBenchmark/Samples", _samplesAction.getValue());
        application->setSetting("Developer/PointDataBenchmark/RowTileSizes", _rowTileSizesAction.getString());
        application->setSetting("Developer/PointDataBenchmark/ColumnTileSizes", _columnTileSizesAction.getString());
        application->setSetting("Developer/PointDataBenchmark/CacheCapacities", _cacheCapacitiesAction.getString());
        application->setSetting("Developer/PointDataBenchmark/OutputPath", _outputPathAction.getFilePath());
    }

private:
    void loadSettings()
    {
        auto* application = mv::Application::current();

        _rowsAction.setValue(application->getSetting("Developer/PointDataBenchmark/Rows", _rowsAction.getValue()).toInt());
        _columnsAction.setValue(application->getSetting("Developer/PointDataBenchmark/Columns", _columnsAction.getValue()).toInt());
        _samplesAction.setValue(application->getSetting("Developer/PointDataBenchmark/Samples", _samplesAction.getValue()).toInt());
        _rowTileSizesAction.setString(application->getSetting("Developer/PointDataBenchmark/RowTileSizes", _rowTileSizesAction.getString()).toString());
        _columnTileSizesAction.setString(application->getSetting("Developer/PointDataBenchmark/ColumnTileSizes", _columnTileSizesAction.getString()).toString());
        _cacheCapacitiesAction.setString(application->getSetting("Developer/PointDataBenchmark/CacheCapacities", _cacheCapacitiesAction.getString()).toString());
        _outputPathAction.setFilePath(application->getSetting("Developer/PointDataBenchmark/OutputPath", _outputPathAction.getFilePath()).toString());
    }

    void updatePreview()
    {
        try
        {
            const auto rows            = static_cast<std::uint64_t>(_rowsAction.getValue());
            const auto columns         = static_cast<std::uint64_t>(_columnsAction.getValue());
            const auto rowTileSizes    = parseValues(_rowTileSizesAction.getString(), rows, 128, 4096, { 64, 128, 256 });
            const auto columnTileSizes = parseValues(_columnTileSizesAction.getString(), columns, 16, 512, { 8, 16, 32 });
            const auto cacheCapacities = resolveCacheCapacities(_cacheCapacitiesAction.getString(), columns, columnTileSizes);
            const auto configurations  = rowTileSizes.size() * columnTileSizes.size() * cacheCapacities.size();
            const auto measurements    = configurations * 2 * 3;

            _previewAction.setString(tr("Dataset: %1 × %2\nRandom samples: %3\nRow tiles: %4 → %5\nColumn tiles: %6 → %7\nCache capacities: %8 → %9\nConfigurations: %10\nMeasurements: %11 (2 backends × 3 access patterns)")
                .arg(rows)
                .arg(columns)
                .arg(_samplesAction.getValue())
                .arg(_rowTileSizesAction.getString())
                .arg(formatValues(rowTileSizes))
                .arg(_columnTileSizesAction.getString())
                .arg(formatValues(columnTileSizes))
                .arg(_cacheCapacitiesAction.getString())
                .arg(formatValues(cacheCapacities))
                .arg(configurations)
                .arg(measurements));
        }
        catch (const std::exception& exception)
        {
            _previewAction.setString(tr("Invalid settings: %1").arg(QString::fromUtf8(exception.what())));
        }
    }

    mv::gui::VerticalGroupAction _settingsGroupAction;
    mv::gui::StringAction        _descriptionAction;
    mv::gui::IntegralAction      _rowsAction;
    mv::gui::IntegralAction      _columnsAction;
    mv::gui::IntegralAction      _samplesAction;
    mv::gui::StringAction        _rowTileSizesAction;
    mv::gui::StringAction        _columnTileSizesAction;
    mv::gui::StringAction        _cacheCapacitiesAction;
    mv::gui::FilePickerAction    _outputPathAction;
    mv::gui::StringAction        _previewAction;
};

void runBenchmark(QWidget* owner, const BenchmarkConfiguration& configuration, const QString& outputPath)
{
    const auto rowTileSizes = parseValues(configuration.rowTileSizes, configuration.rows, 128, 4096, { 64, 128, 256 });
    const auto columnTileSizes = parseValues(configuration.columnTileSizes, configuration.columns, 16, 512, { 8, 16, 32 });
    const auto cacheCapacities = resolveCacheCapacities(configuration.cacheCapacities, configuration.columns, columnTileSizes);

    if (!mv::projects().getCurrentProject())
        throw std::runtime_error("A project must be open before running the point-data benchmark");

    const auto codec = mv::projects().getCurrentProject()->getCompressionAction().createCodec(nullptr);
    const auto coordinates = makeRandomCoordinates(configuration);
    const auto output = std::make_shared<std::ofstream>(outputPath.toStdString(), std::ios::trunc);

    if (!*output)
        throw std::runtime_error("Unable to open the benchmark output file");

    *output << "backend,access_pattern,row_tile_size,column_tile_size,cache_capacity,operations,milliseconds,checksum,cache_hits,cache_misses,evictions\n";

    auto plan = std::make_unique<mv::workflow::WorkflowPlan>(QObject::tr("Point-data benchmark sweep"));
    mv::workflow::WorkflowPlan::Jobs benchmarkJobs;
    benchmarkJobs.reserve(rowTileSizes.size() * columnTileSizes.size() * cacheCapacities.size());

    for (const auto rowTileSize : rowTileSizes)
    {
        for (const auto columnTileSize : columnTileSizes)
        {
            for (const auto cacheCapacity : cacheCapacities)
            {
                benchmarkJobs.emplace_back(QStringLiteral("Tiles %1 x %2, cache %3").arg(rowTileSize).arg(columnTileSize).arg(cacheCapacity), [configuration, coordinates, codec, output, rowTileSize, columnTileSize, cacheCapacity](const mv::workflow::WorkflowPlan::Job&, const mv::workflow::SharedWorkflowExecutionContext&) {
                    const auto tiledStorage = makeGeneratedStorage(configuration, rowTileSize, columnTileSize);
                    tiledStorage->setCacheCapacity(cacheCapacity);
                    writeResult(*output, benchmarkStorage(QStringLiteral("tiled"), tiledStorage, configuration, coordinates, QStringLiteral("random")), rowTileSize, columnTileSize, cacheCapacity);
                    writeResult(*output, benchmarkStorage(QStringLiteral("tiled"), tiledStorage, configuration, coordinates, QStringLiteral("row_scan")), rowTileSize, columnTileSize, cacheCapacity);
                    writeResult(*output, benchmarkStorage(QStringLiteral("tiled"), tiledStorage, configuration, coordinates, QStringLiteral("column_scan")), rowTileSize, columnTileSize, cacheCapacity);

                    const auto shardedStorage = makeShardedStorage(configuration, rowTileSize, columnTileSize, codec);
                    shardedStorage->storage->setCacheCapacity(cacheCapacity);
                    writeResult(*output, benchmarkStorage(QStringLiteral("sharded_tiled"), shardedStorage->storage, configuration, coordinates, QStringLiteral("random")), rowTileSize, columnTileSize, cacheCapacity);
                    writeResult(*output, benchmarkStorage(QStringLiteral("sharded_tiled"), shardedStorage->storage, configuration, coordinates, QStringLiteral("row_scan")), rowTileSize, columnTileSize, cacheCapacity);
                    writeResult(*output, benchmarkStorage(QStringLiteral("sharded_tiled"), shardedStorage->storage, configuration, coordinates, QStringLiteral("column_scan")), rowTileSize, columnTileSize, cacheCapacity);
                });
            }
        }
    }

    plan->addSequentialStage(QObject::tr("Benchmark configurations"), std::move(benchmarkJobs));
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
    action->setToolTip(QObject::tr("Compare tiled and packed/sharded tiled point-data access for a range of tile and cache configurations"));

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
