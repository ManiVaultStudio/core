// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include <tiled/PointDataTiledStorage.h>

#include <QByteArray>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace
{
struct BenchmarkConfiguration
{
    std::uint64_t rows          = 10000;
    std::uint64_t columns       = 128;
    std::size_t rowTileSize     = 256;
    std::size_t columnTileSize  = 8;
    std::size_t cacheCapacity   = 8;
    std::size_t samples         = 100000;
    bool includeEagerReference  = false;
};

struct BenchmarkResult
{
    const char* backend             = nullptr;
    const char* accessPattern       = nullptr;
    std::uint64_t operations        = 0;
    double elapsedMilliseconds      = 0.0;
    double checksum                 = 0.0;
    std::uint64_t cacheHits         = 0;
    std::uint64_t cacheMisses       = 0;
    std::uint64_t evictions         = 0;
};

float valueAt(const std::uint64_t row, const std::uint64_t column, const std::uint64_t columns)
{
    return static_cast<float>((row * columns + column) % 1000003);
}

std::vector<float> makeDenseValues(const BenchmarkConfiguration& configuration)
{
    std::vector<float> values(static_cast<std::size_t>(configuration.rows * configuration.columns));

    for (std::uint64_t row = 0; row < configuration.rows; ++row)
    {
        for (std::uint64_t column = 0; column < configuration.columns; ++column)
            values[static_cast<std::size_t>(row * configuration.columns + column)] = valueAt(row, column, configuration.columns);
    }

    return values;
}

std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> makeGeneratedStorage(const BenchmarkConfiguration& configuration)
{
    return mv::pointdata::tiled::PointDataTiledStorage::fromGenerator(configuration.rows, configuration.columns, sizeof(float), configuration.rowTileSize, configuration.columnTileSize, [columns = configuration.columns](const auto& descriptor) {
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
BenchmarkResult measure(const char* backend, const char* accessPattern, const std::uint64_t operations, Function function)
{
    const auto start = std::chrono::steady_clock::now();
    const auto checksum = function();
    const auto end = std::chrono::steady_clock::now();

    return {
        backend,
        accessPattern,
        operations,
        std::chrono::duration<double, std::milli>(end - start).count(),
        checksum,
        0,
        0,
        0
    };
}

BenchmarkResult benchmarkEagerRandom(const BenchmarkConfiguration& configuration, const std::vector<float>& values, const std::vector<std::pair<std::uint64_t, std::uint64_t>>& coordinates)
{
    return measure("eager_reference", "random", coordinates.size(), [&] {
        double checksum = 0.0;

        for (const auto [row, column] : coordinates)
            checksum += values[static_cast<std::size_t>(row * configuration.columns + column)];

        return checksum;
    });
}

BenchmarkResult benchmarkTiledRandom(const BenchmarkConfiguration& configuration, const std::vector<std::pair<std::uint64_t, std::uint64_t>>& coordinates)
{
    const auto storage = makeGeneratedStorage(configuration);
    storage->setCacheCapacity(configuration.cacheCapacity);
    const auto result = measure("tiled", "random", coordinates.size(), [&] {
        double checksum = 0.0;

        for (const auto [row, column] : coordinates)
            checksum += storage->valueAt(row * configuration.columns + column, 0);

        return checksum;
    });

    return { result.backend, result.accessPattern, result.operations, result.elapsedMilliseconds, result.checksum, storage->cacheHits(), storage->cacheMisses(), storage->cacheEvictions() };
}

BenchmarkResult benchmarkTiledRows(const BenchmarkConfiguration& configuration)
{
    const auto storage = makeGeneratedStorage(configuration);
    storage->setCacheCapacity(configuration.cacheCapacity);
    const auto result = measure("tiled", "row_scan", configuration.rows * configuration.columns, [&] {
        double checksum = 0.0;

        for (std::uint64_t row = 0; row < configuration.rows; ++row)
        {
            for (std::uint64_t column = 0; column < configuration.columns; ++column)
                checksum += storage->valueAt(row * configuration.columns + column, 0);
        }

        return checksum;
    });

    return { result.backend, result.accessPattern, result.operations, result.elapsedMilliseconds, result.checksum, storage->cacheHits(), storage->cacheMisses(), storage->cacheEvictions() };
}

BenchmarkResult benchmarkTiledColumns(const BenchmarkConfiguration& configuration)
{
    const auto storage = makeGeneratedStorage(configuration);
    storage->setCacheCapacity(configuration.cacheCapacity);
    const auto result = measure("tiled", "column_scan", configuration.rows * configuration.columns, [&] {
        double checksum = 0.0;

        for (std::uint64_t column = 0; column < configuration.columns; ++column)
        {
            for (std::uint64_t row = 0; row < configuration.rows; ++row)
                checksum += storage->valueAt(row * configuration.columns + column, 0);
        }

        return checksum;
    });

    return { result.backend, result.accessPattern, result.operations, result.elapsedMilliseconds, result.checksum, storage->cacheHits(), storage->cacheMisses(), storage->cacheEvictions() };
}

void printUsage()
{
    std::cout << "PointDataBenchmark [options]\n"
              << "  --rows N             Number of rows (default: 10000)\n"
              << "  --columns N          Number of columns (default: 128)\n"
              << "  --row-tile-size N    Rows per tile (default: 256)\n"
              << "  --column-tile-size N Columns per tile (default: 8)\n"
              << "  --cache-capacity N   Cached tiles (default: 8)\n"
              << "  --samples N          Random samples (default: 100000)\n"
              << "  --include-eager      Include the legacy eager reference\n"
              << "  --help               Show this help\n";
}

bool parseSize(const std::string& text, std::size_t& value)
{
    try
    {
        value = static_cast<std::size_t>(std::stoull(text));
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool parseArguments(const int argc, char** argv, BenchmarkConfiguration& configuration)
{
    for (int index = 1; index < argc; ++index)
    {
        const std::string argument = argv[index];

        if (argument == "--help")
        {
            printUsage();
            return false;
        }

        if (argument == "--include-eager")
        {
            configuration.includeEagerReference = true;
            continue;
        }

        if (index + 1 >= argc)
            return false;

        std::size_t value = 0;
        if (!parseSize(argv[++index], value))
            return false;

        if (argument == "--rows")
            configuration.rows = value;
        else if (argument == "--columns")
            configuration.columns = value;
        else if (argument == "--row-tile-size")
            configuration.rowTileSize = value;
        else if (argument == "--column-tile-size")
            configuration.columnTileSize = value;
        else if (argument == "--cache-capacity")
            configuration.cacheCapacity = value;
        else if (argument == "--samples")
            configuration.samples = value;
        else
            return false;
    }

    return configuration.rows > 0 && configuration.columns > 0 && configuration.rowTileSize > 0 && configuration.columnTileSize > 0 && configuration.samples > 0;
}

void printResult(const BenchmarkResult& result)
{
    std::cout << result.backend << '\t'
              << result.accessPattern << '\t'
              << result.operations << '\t'
              << std::fixed << std::setprecision(3) << result.elapsedMilliseconds << '\t'
              << std::setprecision(6) << result.checksum << '\t'
              << result.cacheHits << '\t'
              << result.cacheMisses << '\t'
              << result.evictions << '\n';
}
}

int main(const int argc, char** argv)
{
    BenchmarkConfiguration configuration;
    if (!parseArguments(argc, argv, configuration))
        return argc > 1 && std::string(argv[1]) == "--help" ? 0 : 1;

    const auto coordinates = makeRandomCoordinates(configuration);

    std::cout << "backend\taccess_pattern\toperations\tmilliseconds\tchecksum\tcache_hits\tcache_misses\tevictions\n";

    if (configuration.includeEagerReference)
    {
        const auto values = makeDenseValues(configuration);
        printResult(benchmarkEagerRandom(configuration, values, coordinates));
    }

    printResult(benchmarkTiledRandom(configuration, coordinates));
    printResult(benchmarkTiledRows(configuration));
    printResult(benchmarkTiledColumns(configuration));
    return 0;
}
