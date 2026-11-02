// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "../PointData.h"

#include <CoreInterface.h>
#include <DeveloperActionRegistry.h>
#include <Application.h>
#include <actions/OptionsAction.h>
#include <util/StyledIcon.h>
#include <workflow/WorkflowOptions.h>
#include <workflow/WorkflowPlan.h>

#include <QMessageBox>
#include <QColor>
#include <QDialog>
#include <QFileDialog>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollBar>
#include <QItemSelectionModel>
#include <QSaveFile>
#include <QTableWidget>
#include <QToolButton>
#include <QVBoxLayout>

#include <cstdint>
#include <array>
#include <cmath>
#include <cstring>
#include <exception>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <optional>
#include <QStringList>
#include <type_traits>
#include <utility>
#include <vector>

namespace mv::pointdata::developer
{
namespace
{
struct ConversionEvidence
{
    QString sourceType;
    QString targetType;
    std::uint64_t columns = 0;
    QStringList sourceValues;
    QStringList convertedValues;
    bool conversionSucceeded = false;
};

struct TestResult
{
    QString name;
    QString backend;
    QString details;
    bool passed = false;
    std::shared_ptr<ConversionEvidence> conversionEvidence;
};

using TestResults = std::vector<TestResult>;

constexpr std::uint64_t conversionTestRows    = 9;
constexpr std::uint64_t conversionTestColumns = 17;

QString formatNumericValue(const long double value)
{
    if (std::isnan(value))
        return QStringLiteral("NaN");
    if (std::isinf(value))
        return std::signbit(value) ? QStringLiteral("-Infinity") : QStringLiteral("Infinity");
    return QString::number(static_cast<double>(value), 'g', std::numeric_limits<double>::max_digits10);
}

QString runDeterministicRowFetchTest(const bool thorough, const Points::StorageBackendType backend)
{
    const std::uint64_t rows    = thorough ? 10001 : 1025;
    constexpr std::uint64_t columns = 33;
    constexpr std::size_t rowTileSize = 256;
    constexpr std::size_t columnTileSize = 8;
    std::vector<float> source(rows * columns);

    for (std::uint64_t row = 0; row < rows; ++row)
    {
        for (std::uint64_t column = 0; column < columns; ++column)
            source[row * columns + column] = row == 0 || column == 0 || row == 6 || column == 4 ? 0.0f : static_cast<float>(static_cast<std::int32_t>(row * 17 + column * 5) - 50) / 4.0f;
    }

    const auto backendName = pointdata::storage::getStorageBackendTypeName(backend);
    auto dataset = mv::data().createDataset<Points>("Points", QString("Point-data %1 row access test").arg(backendName));
    dataset->setData(source, columns);

    try
    {
        if (backend == Points::StorageBackendType::Tiled)
        {
            pointdata::storage::StorageBackendConfiguration configuration;
            configuration.rowTileSize = rowTileSize;
            configuration.columnTileSize = columnTileSize;
            dataset->convertToStorageBackend(backend, configuration);
        }
        else if (backend == Points::StorageBackendType::Sparse)
            dataset->convertToStorageBackend(backend, std::make_shared<const pointdata::storage::SparseStorageBackendConfiguration>());

        for (std::uint64_t row = 0; row < rows; ++row)
        {
            const auto view = dataset->getRow(row);
            if (!view || view.rows != 1 || view.columns != columns || view.elementSizeBytes != sizeof(float))
                throw std::runtime_error(QString("Unexpected row view at row %1").arg(row).toStdString());

            const auto* actual = static_cast<const float*>(view.data);
            for (std::uint64_t column = 0; column < columns; ++column)
            {
                if (actual[column] != source[row * columns + column])
                    throw std::runtime_error(QString("Value mismatch at row %1, column %2").arg(row).arg(column).toStdString());
            }

        }

        mv::data().removeDataset(dataset);
        return QObject::tr("%1: passed (%2 rows checked)").arg(backendName).arg(rows);
    }
    catch (...)
    {
        mv::data().removeDataset(dataset);
        throw;
    }
}

template <typename T>
std::vector<T> makeConversionValues()
{
    constexpr std::array<long double, 20> probes{ -40000.75L, -32768.0L, -257.0L, -128.0L, -2.75L, -1.5L, -0.5L, 0.0L, 0.5L, 1.5L, 2.75L, 127.0L, 128.0L, 255.0L, 256.0L, 32767.0L, 32768.0L, 65535.0L, 65536.0L, 4294967296.0L };
    std::vector<T> values;

    for (const auto probe : probes)
    {
        if constexpr (std::is_integral_v<T>)
        {
            if (probe < static_cast<long double>(std::numeric_limits<T>::lowest()) || probe > static_cast<long double>(std::numeric_limits<T>::max()))
                continue;
        }

        const auto value = static_cast<T>(probe);
        if (std::find(values.cbegin(), values.cend(), value) == values.cend())
            values.push_back(value);
    }

    if constexpr (std::is_integral_v<T>)
    {
        for (const auto boundary : { std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max() })
        {
            if (std::find(values.cbegin(), values.cend(), boundary) == values.cend())
                values.push_back(boundary);
        }
    }

    if constexpr (std::is_floating_point_v<T>)
    {
        values.push_back(std::numeric_limits<T>::infinity());
        values.push_back(std::numeric_limits<T>::quiet_NaN());
    }

    return values;
}

bool expectedConversion(long double source, std::uint32_t targetType, long double& expected);

template <typename T>
void setConversionData(Points& points, std::vector<T> values, const std::optional<std::uint32_t> targetType = {})
{
    if (targetType)
    {
        std::vector<T> inRangeValues;
        for (const auto value : values)
        {
            long double expected = 0;
            const auto sourceValue = [&] {
                if constexpr (std::is_same_v<T, biovault::bfloat16_t>)
                    return static_cast<long double>(static_cast<float>(value));
                else
                    return static_cast<long double>(value);
            }();
            if (expectedConversion(sourceValue, *targetType, expected))
                inRangeValues.push_back(value);
        }
        values = std::move(inRangeValues);
    }

    if (values.empty())
        throw std::runtime_error("No source values fit the requested target element type");

    const auto probeCount = values.size();
    const auto elementCount = conversionTestRows * conversionTestColumns;
    values.reserve(elementCount);
    for (std::size_t index = 0; values.size() < elementCount; ++index)
        values.push_back(values[index % probeCount]);

    points.setData(values, conversionTestColumns);
}

void setConversionSource(Points& points, const std::uint32_t typeIndex, const std::optional<std::uint32_t> targetType = {})
{
    switch (typeIndex)
    {
        case 0: setConversionData(points, makeConversionValues<float>(), targetType); return;
        case 1:
        {
            const auto floats = makeConversionValues<float>();
            std::vector<biovault::bfloat16_t> values;
            values.reserve(floats.size());
            for (const auto value : floats)
                values.emplace_back(value);
            setConversionData(points, std::move(values), targetType);
            return;
        }
        case 2: setConversionData(points, makeConversionValues<std::int32_t>(), targetType); return;
        case 3: setConversionData(points, makeConversionValues<std::uint32_t>(), targetType); return;
        case 4: setConversionData(points, makeConversionValues<std::int16_t>(), targetType); return;
        case 5: setConversionData(points, makeConversionValues<std::uint16_t>(), targetType); return;
        case 6: setConversionData(points, makeConversionValues<std::int8_t>(), targetType); return;
        case 7: setConversionData(points, makeConversionValues<std::uint8_t>(), targetType); return;
        default: throw std::invalid_argument("Unsupported conversion source type");
    }
}

long double readNumericValue(const PointDataAccessView& view, const std::uint64_t column)
{
    const auto* bytes = static_cast<const char*>(view.data) + column * view.elementSizeBytes;
    switch (view.elementTypeIndex)
    {
        case 0: { float value; std::memcpy(&value, bytes, sizeof(value)); return value; }
        case 1: { biovault::bfloat16_t value; std::memcpy(&value, bytes, sizeof(value)); return static_cast<float>(value); }
        case 2: { std::int32_t value; std::memcpy(&value, bytes, sizeof(value)); return value; }
        case 3: { std::uint32_t value; std::memcpy(&value, bytes, sizeof(value)); return value; }
        case 4: { std::int16_t value; std::memcpy(&value, bytes, sizeof(value)); return value; }
        case 5: { std::uint16_t value; std::memcpy(&value, bytes, sizeof(value)); return value; }
        case 6: { std::int8_t value; std::memcpy(&value, bytes, sizeof(value)); return value; }
        case 7: { std::uint8_t value; std::memcpy(&value, bytes, sizeof(value)); return value; }
        default: throw std::invalid_argument("Unsupported conversion value type");
    }
}

bool expectedConversion(const long double source, const std::uint32_t targetType, long double& expected)
{
    switch (targetType)
    {
        case 0: expected = static_cast<float>(source); return true;
        case 1: expected = static_cast<float>(biovault::bfloat16_t(static_cast<float>(source))); return true;
        case 2: if (!std::isfinite(source) || source < std::numeric_limits<std::int32_t>::lowest() || source > std::numeric_limits<std::int32_t>::max()) return false; expected = static_cast<std::int32_t>(source); return true;
        case 3: if (!std::isfinite(source) || source < 0 || source > std::numeric_limits<std::uint32_t>::max()) return false; expected = static_cast<std::uint32_t>(source); return true;
        case 4: if (!std::isfinite(source) || source < std::numeric_limits<std::int16_t>::lowest() || source > std::numeric_limits<std::int16_t>::max()) return false; expected = static_cast<std::int16_t>(source); return true;
        case 5: if (!std::isfinite(source) || source < 0 || source > std::numeric_limits<std::uint16_t>::max()) return false; expected = static_cast<std::uint16_t>(source); return true;
        case 6: if (!std::isfinite(source) || source < std::numeric_limits<std::int8_t>::lowest() || source > std::numeric_limits<std::int8_t>::max()) return false; expected = static_cast<std::int8_t>(source); return true;
        case 7: if (!std::isfinite(source) || source < 0 || source > std::numeric_limits<std::uint8_t>::max()) return false; expected = static_cast<std::uint8_t>(source); return true;
        default: throw std::invalid_argument("Unsupported conversion target type");
    }
}

void convertToBackend(Points& points, const Points::StorageBackendType backend, const std::uint32_t targetType)
{
    auto setTargetType = [targetType](auto& options) {
        options.targetElementTypeIndex = targetType;
    };

    if (backend == Points::StorageBackendType::Tiled)
    {
        pointdata::storage::StorageBackendConfiguration configuration;
        configuration.rowTileSize = 4;
        configuration.columnTileSize = 8;
        setTargetType(configuration);
        points.convertToStorageBackend(backend, configuration);
    }
    else if (backend == Points::StorageBackendType::Sparse)
    {
        auto configuration = std::make_shared<pointdata::storage::SparseStorageBackendConfiguration>();
        setTargetType(*configuration);
        points.convertToStorageBackend(backend, configuration);
    }
    else
    {
        auto configuration = std::make_shared<pointdata::storage::StorageBackendConfiguration>();
        setTargetType(*configuration);
        points.convertToStorageBackend(backend, configuration);
    }
}

void runElementTypeConversion(const Points::StorageBackendType sourceBackend, const Points::StorageBackendType targetBackend, const std::uint32_t sourceType, const std::uint32_t targetType, const bool inRangeOnly, const std::shared_ptr<ConversionEvidence>& evidence)
{
    const auto sourceBackendName = pointdata::storage::getStorageBackendTypeName(sourceBackend);
    const auto targetBackendName = pointdata::storage::getStorageBackendTypeName(targetBackend);

    evidence->sourceType = Points::elementTypeName(static_cast<Points::ElementTypeSpecifier>(sourceType));
    evidence->targetType = Points::elementTypeName(static_cast<Points::ElementTypeSpecifier>(targetType));
    evidence->columns = conversionTestColumns;
    auto dataset = mv::data().createDataset<Points>("Points", QString("Point-data conversion %1 %2 to %3 %4").arg(sourceBackendName, evidence->sourceType, targetBackendName, evidence->targetType));
    try
    {
        setConversionSource(*dataset, sourceType, inRangeOnly ? std::optional<std::uint32_t>(targetType) : std::nullopt);
        convertToBackend(*dataset, sourceBackend, sourceType);

        std::vector<long double> sourceValues;
        for (std::uint64_t row = 0; row < dataset->getNumPoints(); ++row)
        {
            const auto view = dataset->getRow(row);
            for (std::uint64_t column = 0; column < view.columns; ++column)
                sourceValues.push_back(readNumericValue(view, column));
        }
        for (const auto value : sourceValues)
            evidence->sourceValues << formatNumericValue(value);

        bool shouldFail = false;
        for (const auto value : sourceValues)
        {
            long double expected = 0;
            if (!expectedConversion(value, targetType, expected))
                shouldFail = true;
        }

        bool conversionFailed = false;
        try
        {
            convertToBackend(*dataset, targetBackend, targetType);
        }
        catch (const std::range_error&)
        {
            conversionFailed = true;
        }

        evidence->conversionSucceeded = !conversionFailed;
        for (std::uint64_t row = 0; row < dataset->getNumPoints(); ++row)
        {
            const auto view = dataset->getRow(row);
            for (std::uint64_t column = 0; column < view.columns; ++column)
                evidence->convertedValues << formatNumericValue(readNumericValue(view, column));
        }

        if (conversionFailed != shouldFail)
            throw std::runtime_error(QString("Unexpected conversion outcome for %1 %2 to %3 %4").arg(sourceBackendName, evidence->sourceType, targetBackendName, evidence->targetType).toStdString());

        if (conversionFailed)
        {
            if (static_cast<std::uint32_t>(Points::elementTypeSpecifier(dataset->getElementTypeName())) != sourceType || dataset->getStorageBackendType() != sourceBackend)
                throw std::runtime_error("Failed conversion changed the source representation");
            std::size_t valueIndex = 0;
            for (std::uint64_t row = 0; row < dataset->getNumPoints(); ++row)
            {
                const auto view = dataset->getRow(row);
                for (std::uint64_t column = 0; column < view.columns; ++column, ++valueIndex)
                {
                    const auto actual = readNumericValue(view, column);
                    if (!(actual == sourceValues[valueIndex] || (std::isnan(actual) && std::isnan(sourceValues[valueIndex]))))
                        throw std::runtime_error("Failed conversion changed source values");
                }
            }
        }
        else
        {
            if (static_cast<std::uint32_t>(Points::elementTypeSpecifier(dataset->getElementTypeName())) != targetType || dataset->getStorageBackendType() != targetBackend)
                throw std::runtime_error("Successful conversion produced the wrong representation");
            std::size_t valueIndex = 0;
            for (std::uint64_t row = 0; row < dataset->getNumPoints(); ++row)
            {
                const auto view = dataset->getRow(row);
                for (std::uint64_t column = 0; column < view.columns; ++column, ++valueIndex)
                {
                    long double expected = 0;
                    expectedConversion(sourceValues[valueIndex], targetType, expected);
                    const auto actual = readNumericValue(view, column);
                    if (!(actual == expected || (std::isnan(actual) && std::isnan(expected))))
                        throw std::runtime_error("Converted value differs from the expected cast result");
                }
            }
        }

        mv::data().removeDataset(dataset);
    }
    catch (...)
    {
        mv::data().removeDataset(dataset);
        throw;
    }
}

void showConversionEvidence(QWidget* owner, const TestResult& result)
{
    if (!result.conversionEvidence)
        return;

    const auto& evidence = *result.conversionEvidence;
    QDialog dialog(owner);
    dialog.setWindowTitle(QObject::tr("Conversion data: %1 | %2 to %3").arg(result.backend, evidence.sourceType, evidence.targetType));
    dialog.setWindowIcon(mv::util::StyledIcon("right-left"));
    dialog.setMinimumSize(900, 520);
    dialog.resize(1100, 620);
    auto* layout = new QVBoxLayout(&dialog);
    const auto statusText = evidence.conversionSucceeded
        ? (result.passed ? QObject::tr("Conversion completed and the stored target values passed validation. Changed values are highlighted.") : QObject::tr("Conversion completed, but validation failed. Compare the source and stored target values."))
        : (result.passed ? QObject::tr("The out-of-range conversion was correctly rejected. The test passed because the source values were retained.") : QObject::tr("Conversion was rejected unexpectedly. The target view shows the values retained after rejection."));
    auto* status = new QLabel(statusText, &dialog);
    auto* tablesLayout = new QHBoxLayout;
    auto* sourceLayout = new QVBoxLayout;
    auto* targetLayout = new QVBoxLayout;
    auto* sourceLabel = new QLabel(QObject::tr("Source (%1)").arg(evidence.sourceType), &dialog);
    auto* targetLabel = new QLabel(QObject::tr("Converted (%1)").arg(evidence.conversionSucceeded ? evidence.targetType : evidence.sourceType), &dialog);
    const auto rowCount = static_cast<int>((evidence.sourceValues.size() + evidence.columns - 1) / evidence.columns);
    auto* sourceTable = new QTableWidget(rowCount, static_cast<int>(evidence.columns + 1), &dialog);
    auto* targetTable = new QTableWidget(rowCount, static_cast<int>(evidence.columns + 1), &dialog);
    for (auto* table : { sourceTable, targetTable })
    {
        QStringList headers { QObject::tr("Row") };
        for (std::uint64_t column = 0; column < evidence.columns; ++column)
            headers << QObject::tr("D%1").arg(static_cast<qulonglong>(column));
        table->setHorizontalHeaderLabels(headers);
        table->verticalHeader()->hide();
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectItems);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        table->horizontalHeader()->setDefaultSectionSize(96);
        for (std::uint64_t column = 1; column <= evidence.columns; ++column)
            table->setColumnWidth(static_cast<int>(column), 96);
    }
    for (int row = 0; row < rowCount; ++row)
    {
        auto* sourceIndex = new QTableWidgetItem(QString::number(static_cast<qulonglong>(row)));
        auto* targetIndex = new QTableWidgetItem(QString::number(static_cast<qulonglong>(row)));
        for (auto* item : { sourceIndex, targetIndex })
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        sourceTable->setItem(static_cast<int>(row), 0, sourceIndex);
        targetTable->setItem(static_cast<int>(row), 0, targetIndex);
        for (std::uint64_t column = 0; column < evidence.columns; ++column)
        {
            const auto valueIndex = static_cast<qsizetype>(row * evidence.columns + column);
            auto* sourceValue = new QTableWidgetItem(evidence.sourceValues.value(valueIndex));
            auto* targetValue = new QTableWidgetItem(evidence.convertedValues.value(valueIndex));
            sourceValue->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            targetValue->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            if (sourceValue->text() != targetValue->text())
            {
                const QColor background(164, 92, 12);
                const QColor foreground(Qt::white);
                sourceValue->setBackground(background);
                sourceValue->setForeground(foreground);
                targetValue->setBackground(background);
                targetValue->setForeground(foreground);
                sourceValue->setToolTip(QObject::tr("This value differs after conversion."));
                targetValue->setToolTip(QObject::tr("This value differs from the source."));
            }
            sourceTable->setItem(row, static_cast<int>(column + 1), sourceValue);
            targetTable->setItem(row, static_cast<int>(column + 1), targetValue);
        }
    }
    QObject::connect(sourceTable->verticalScrollBar(), &QScrollBar::valueChanged, targetTable->verticalScrollBar(), &QScrollBar::setValue);
    QObject::connect(targetTable->verticalScrollBar(), &QScrollBar::valueChanged, sourceTable->verticalScrollBar(), &QScrollBar::setValue);
    QObject::connect(sourceTable->horizontalScrollBar(), &QScrollBar::valueChanged, targetTable->horizontalScrollBar(), &QScrollBar::setValue);
    QObject::connect(targetTable->horizontalScrollBar(), &QScrollBar::valueChanged, sourceTable->horizontalScrollBar(), &QScrollBar::setValue);
    bool synchronizingSelection = false;
    QObject::connect(sourceTable->selectionModel(), &QItemSelectionModel::currentChanged, targetTable, [targetTable, &synchronizingSelection](const QModelIndex& current, const QModelIndex&) {
        if (synchronizingSelection)
            return;

        synchronizingSelection = true;
        const auto targetIndex = current.isValid() ? targetTable->model()->index(current.row(), current.column()) : QModelIndex();
        targetTable->selectionModel()->setCurrentIndex(targetIndex, QItemSelectionModel::ClearAndSelect);
        synchronizingSelection = false;
    });
    QObject::connect(targetTable->selectionModel(), &QItemSelectionModel::currentChanged, sourceTable, [sourceTable, &synchronizingSelection](const QModelIndex& current, const QModelIndex&) {
        if (synchronizingSelection)
            return;

        synchronizingSelection = true;
        const auto sourceIndex = current.isValid() ? sourceTable->model()->index(current.row(), current.column()) : QModelIndex();
        sourceTable->selectionModel()->setCurrentIndex(sourceIndex, QItemSelectionModel::ClearAndSelect);
        synchronizingSelection = false;
    });
    sourceLayout->addWidget(sourceLabel);
    sourceLayout->addWidget(sourceTable);
    targetLayout->addWidget(targetLabel);
    targetLayout->addWidget(targetTable);
    tablesLayout->addLayout(sourceLayout);
    tablesLayout->addLayout(targetLayout);
    auto* closeButton = new QPushButton(QObject::tr("Close"), &dialog);
    QObject::connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(status);
    layout->addLayout(tablesLayout);
    layout->addWidget(closeButton, 0, Qt::AlignRight);
    dialog.exec();
}

QJsonArray makeJsonMatrix(const QStringList& values, const std::uint64_t columns)
{
    QJsonArray matrix;
    if (columns == 0)
        return matrix;

    for (qsizetype firstValue = 0; firstValue < values.size(); firstValue += static_cast<qsizetype>(columns))
    {
        QJsonArray row;
        const auto lastValue = std::min<qsizetype>(values.size(), firstValue + static_cast<qsizetype>(columns));
        for (auto valueIndex = firstValue; valueIndex < lastValue; ++valueIndex)
        {
            const auto value = values.at(valueIndex);
            bool ok = false;
            const auto number = value.toDouble(&ok);
            row.append(ok && std::isfinite(number) ? QJsonValue(number) : QJsonValue(value));
        }
        matrix.append(row);
    }
    return matrix;
}

QJsonObject makeJsonMatrixObject(const QString& elementType, const QStringList& values, const std::uint64_t columns)
{
    QJsonObject matrix;
    matrix.insert(QStringLiteral("elementType"), elementType);
    matrix.insert(QStringLiteral("rows"), columns ? static_cast<qint64>((values.size() + columns - 1) / columns) : 0);
    matrix.insert(QStringLiteral("columns"), static_cast<qint64>(columns));
    matrix.insert(QStringLiteral("values"), makeJsonMatrix(values, columns));
    return matrix;
}

void exportTestResults(QWidget* owner, const bool thorough, const TestResults& results)
{
    const auto path = QFileDialog::getSaveFileName(owner, QObject::tr("Export point-data test report"), QString(), QObject::tr("JSON files (*.json)"));
    if (path.isEmpty())
        return;

    QJsonArray resultArray;
    for (const auto& result : results)
    {
        QJsonObject resultObject;
        resultObject.insert(QStringLiteral("test"), result.name);
        resultObject.insert(QStringLiteral("backend"), result.backend);
        resultObject.insert(QStringLiteral("passed"), result.passed);
        resultObject.insert(QStringLiteral("details"), result.details);
        if (result.conversionEvidence)
        {
            const auto& evidence = *result.conversionEvidence;
            resultObject.insert(QStringLiteral("conversionSucceeded"), evidence.conversionSucceeded);
            resultObject.insert(QStringLiteral("source"), makeJsonMatrixObject(evidence.sourceType, evidence.sourceValues, evidence.columns));
            resultObject.insert(QStringLiteral("afterConversionAttempt"), makeJsonMatrixObject(evidence.conversionSucceeded ? evidence.targetType : evidence.sourceType, evidence.convertedValues, evidence.columns));
        }
        resultArray.append(resultObject);
    }

    const auto failures = std::count_if(results.cbegin(), results.cend(), [](const TestResult& result) { return !result.passed; });
    QJsonObject root;
    root.insert(QStringLiteral("runType"), thorough ? QStringLiteral("thorough") : QStringLiteral("quick"));
    root.insert(QStringLiteral("passed"), static_cast<qint64>(results.size() - failures));
    root.insert(QStringLiteral("failed"), static_cast<qint64>(failures));
    root.insert(QStringLiteral("nonFiniteValues"), QStringLiteral("NaN and infinities are represented as strings because JSON has no non-finite number values."));
    root.insert(QStringLiteral("results"), resultArray);

    QSaveFile output(path);
    const auto json = QJsonDocument(root).toJson(QJsonDocument::Indented);
    if (!output.open(QIODevice::WriteOnly) || output.write(json) != json.size() || !output.commit())
    {
        QMessageBox::warning(owner, QObject::tr("Export failed"), QObject::tr("Could not write the JSON file: %1").arg(output.errorString()));
        return;
    }
    QMessageBox::information(owner, QObject::tr("Export complete"), QObject::tr("Test report exported to %1").arg(path));
}

void showReport(QWidget* owner, const bool thorough, const TestResults& results)
{
    const auto failures = std::count_if(results.cbegin(), results.cend(), [](const TestResult& result) { return !result.passed; });
    const auto passed = results.size() - failures;
    QDialog report(owner);
    report.setWindowTitle(QObject::tr("Point-data test report"));
    report.setWindowIcon(mv::util::StyledIcon("clipboard-check"));
    report.setMinimumWidth(680);

    auto* layout = new QVBoxLayout(&report);
    auto* summary = new QLabel(QObject::tr("%1 run: %2 passed, %3 failed").arg(thorough ? QObject::tr("Thorough") : QObject::tr("Quick")).arg(passed).arg(failures), &report);
    auto* toggleButton = new QToolButton(&report);
    toggleButton->setText(QObject::tr("Show test results"));
    toggleButton->setCheckable(true);
    toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toggleButton->setArrowType(Qt::RightArrow);
    mv::gui::OptionsAction resultFilterAction(&report, QObject::tr("Filter results"), { QObject::tr("Passed"), QObject::tr("Failed") }, { QObject::tr("Passed"), QObject::tr("Failed") });
    resultFilterAction.setDefaultWidgetFlags(mv::gui::OptionsAction::Tags);
    auto* table = new QTableWidget(static_cast<int>(results.size()), 5, &report);
    table->setHorizontalHeaderLabels({ QObject::tr("Test"), QObject::tr("Backend"), QObject::tr("Result"), QObject::tr("Details"), QObject::tr("Data") });
    table->verticalHeader()->hide();
    table->setAlternatingRowColors(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);

    for (std::size_t row = 0; row < results.size(); ++row)
    {
        const auto& result = results.at(row);
        auto* testItem = new QTableWidgetItem(result.name);
        testItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        testItem->setIcon(mv::util::StyledIcon(result.passed ? "square-check" : "square-xmark"));
        table->setItem(static_cast<int>(row), 0, testItem);
        auto* backendItem = new QTableWidgetItem(result.backend);
        backendItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        table->setItem(static_cast<int>(row), 1, backendItem);
        auto* resultItem = new QTableWidgetItem(result.passed ? QObject::tr("Passed") : QObject::tr("Failed"));
        resultItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        table->setItem(static_cast<int>(row), 2, resultItem);
        auto* detailsItem = new QTableWidgetItem(result.details);
        detailsItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        table->setItem(static_cast<int>(row), 3, detailsItem);
        if (result.conversionEvidence)
        {
            auto* inspectItem = new QTableWidgetItem(mv::util::StyledIcon("eye"), QString());
            inspectItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            inspectItem->setToolTip(QObject::tr("View conversion data"));
            table->setItem(static_cast<int>(row), 4, inspectItem);
        }
    }
    QObject::connect(table, &QTableWidget::cellClicked, &report, [table, owner, &results](const int row, const int column) {
        if (column == 4 && row >= 0 && static_cast<std::size_t>(row) < results.size() && results.at(static_cast<std::size_t>(row)).conversionEvidence)
            showConversionEvidence(owner, results.at(static_cast<std::size_t>(row)));
    });
    const auto updateResultFilters = [table, &results](const QStringList& selectedOptions) {
        const auto showPassed = selectedOptions.contains(QObject::tr("Passed"));
        const auto showFailed = selectedOptions.contains(QObject::tr("Failed"));
        for (std::size_t row = 0; row < results.size(); ++row)
            table->setRowHidden(static_cast<int>(row), results.at(row).passed ? !showPassed : !showFailed);
    };
    QObject::connect(&resultFilterAction, &mv::gui::OptionsAction::selectedOptionsChanged, &report, updateResultFilters);

    auto* exportButton = new QPushButton(mv::util::StyledIcon("file-export"), QObject::tr("Export JSON"), &report);
    auto* closeButton = new QPushButton(QObject::tr("Close"), &report);
    QObject::connect(exportButton, &QPushButton::clicked, &report, [owner, thorough, &results] { exportTestResults(owner, thorough, results); });
    auto* resultsContainer = new QWidget(&report);
    auto* resultsLayout = new QVBoxLayout(resultsContainer);
    resultsLayout->setContentsMargins(0, 0, 0, 0);
    resultsLayout->addWidget(resultFilterAction.createWidget(resultsContainer));
    resultsLayout->addWidget(table);
    resultsContainer->setMaximumHeight(0);
    resultsContainer->hide();
    QObject::connect(closeButton, &QPushButton::clicked, &report, &QDialog::accept);
    layout->addWidget(summary);
    layout->addWidget(toggleButton);
    layout->addWidget(resultsContainer);
    auto* reportButtonsLayout = new QHBoxLayout;
    reportButtonsLayout->addWidget(exportButton);
    reportButtonsLayout->addStretch();
    reportButtonsLayout->addWidget(closeButton);
    layout->addLayout(reportButtonsLayout);
    QObject::connect(toggleButton, &QToolButton::toggled, &report, [toggleButton, resultsContainer, layout, &report](const bool expanded) {
        toggleButton->setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);
        toggleButton->setText(expanded ? QObject::tr("Hide test results") : QObject::tr("Show test results"));
        if (expanded)
        {
            resultsContainer->setMaximumHeight(QWIDGETSIZE_MAX);
            resultsContainer->show();
        }
        else
        {
            resultsContainer->setMaximumHeight(0);
            resultsContainer->hide();
            report.setMinimumWidth(680);
        }
        layout->invalidate();
        layout->activate();
        report.adjustSize();
    });
    report.exec();
}

void startWorkflow(QWidget* owner, const bool thorough, const std::vector<Points::StorageBackendType>& backends, const bool includeRows = true, const bool includeConversions = true, const std::optional<Points::StorageBackendType> onlyTargetBackend = {})
{
    auto results = std::make_shared<TestResults>();
    auto plan = std::make_unique<workflow::WorkflowPlan>(QObject::tr("Point-data %1 tests").arg(thorough ? QObject::tr("thorough") : QObject::tr("quick")));
    constexpr Points::StorageBackendType allBackends[] = { Points::StorageBackendType::Eager, Points::StorageBackendType::Tiled, Points::StorageBackendType::Sparse };

    for (const auto backend : backends)
    {
        const auto backendName = pointdata::storage::getStorageBackendTypeName(backend);
        if (includeRows)
        {
            plan->addSequentialStage(QObject::tr("Check %1 row access").arg(backendName), [results, backend, thorough, backendName](const workflow::WorkflowPlan::Job&, const workflow::SharedWorkflowExecutionContext&) {
                try
                {
                    results->push_back({ QObject::tr("Fetch deterministic rows"), backendName, runDeterministicRowFetchTest(thorough, backend), true, {} });
                }
                catch (const std::exception& exception)
                {
                    results->push_back({ QObject::tr("Fetch deterministic rows"), backendName, QString::fromUtf8(exception.what()), false, {} });
                }
            }, workflow::WorkflowPlan::JobThreadAffinity::GuiThread);
        }
        if (!includeConversions)
            continue;

        for (const auto targetBackend : allBackends)
        {
            if (onlyTargetBackend && *onlyTargetBackend != targetBackend)
                continue;

            const auto targetBackendName = pointdata::storage::getStorageBackendTypeName(targetBackend);
            workflow::WorkflowPlan::Jobs conversionJobs;
            for (std::uint32_t sourceType = 0; sourceType < 8; ++sourceType)
            {
                for (std::uint32_t targetType = 0; targetType < 8; ++targetType)
                {
                    const auto sourceTypeName = Points::elementTypeName(static_cast<Points::ElementTypeSpecifier>(sourceType));
                    const auto targetTypeName = Points::elementTypeName(static_cast<Points::ElementTypeSpecifier>(targetType));
                    for (const bool inRangeOnly : { true, false })
                    {
                        const auto caseName = inRangeOnly ? QObject::tr("target-safe values") : QObject::tr("range-boundary values");
                        conversionJobs.emplace_back(QObject::tr("%1 to %2 · %3").arg(sourceTypeName, targetTypeName, caseName), [results, backend, targetBackend, backendName, targetBackendName, sourceType, targetType, inRangeOnly](const workflow::WorkflowPlan::Job&, const workflow::SharedWorkflowExecutionContext&) {
                            auto evidence = std::make_shared<ConversionEvidence>();
                            const auto testName = QObject::tr("%1 to %2 · %3")
                                .arg(Points::elementTypeName(static_cast<Points::ElementTypeSpecifier>(sourceType)), Points::elementTypeName(static_cast<Points::ElementTypeSpecifier>(targetType)), inRangeOnly ? QObject::tr("target-safe") : QObject::tr("range-boundary"));
                            try
                            {
                                runElementTypeConversion(backend, targetBackend, sourceType, targetType, inRangeOnly, evidence);
                                const auto details = evidence->conversionSucceeded
                                    ? QObject::tr("All stored values match the expected conversion")
                                    : QObject::tr("Out-of-range conversion correctly rejected; source values retained");
                                results->push_back({ testName, QStringLiteral("%1 → %2").arg(backendName, targetBackendName), details, true, evidence });
                            }
                            catch (const std::exception& exception)
                            {
                                results->push_back({ testName, QStringLiteral("%1 → %2").arg(backendName, targetBackendName), QString::fromUtf8(exception.what()), false, evidence });
                            }
                        }, workflow::WorkflowPlan::JobThreadAffinity::GuiThread);
                    }
                }
            }
            plan->addSequentialStage(QObject::tr("Convert %1 to %2").arg(backendName, targetBackendName), std::move(conversionJobs));
        }
    }

    workflow::WorkflowOptions options;
    options.cancellation.enabled = true;
    options.reporting.progress = true;
    options.reporting.finishedNotification = true;
    auto future = Application::getWorkflowPlanExecutor().execute(std::move(plan), nullptr, options);
    future.onFinished(owner, [owner, thorough, results](const workflow::SharedWorkflowResult&) {
        showReport(owner, thorough, *results);
    });
}

const bool registered = [] {
    mv::developer::registerMenuPopulator(QStringLiteral("PointData.Testing.RowFetch"), [](QMenu& menu, QWidget* owner) {
        auto populateMode = [owner](QMenu& modeMenu, const bool thorough) {
            constexpr Points::StorageBackendType backends[] = { Points::StorageBackendType::Eager, Points::StorageBackendType::Tiled, Points::StorageBackendType::Sparse };
            auto runAll = [owner, thorough, backends] {
                startWorkflow(owner, thorough, { backends[0], backends[1], backends[2] });
            };

            auto* runAllAction = modeMenu.addAction(mv::util::StyledIcon("forward-fast"), QObject::tr("Run all scenarios"));
            QObject::connect(runAllAction, &QAction::triggered, owner, runAll);
            modeMenu.addSeparator();
            auto* fetchRowsMenu = modeMenu.addMenu(mv::util::StyledIcon("list-check"), QObject::tr("Fetch rows"));
            auto* conversionsMenu = modeMenu.addMenu(mv::util::StyledIcon("right-left"), QObject::tr("Conversions"));
            for (const auto backend : backends)
            {
                const auto backendName = pointdata::storage::getStorageBackendTypeName(backend);
                auto* action = fetchRowsMenu->addAction(mv::util::StyledIcon("database"), backendName);
                QObject::connect(action, &QAction::triggered, owner, [owner, thorough, backend] {
                    startWorkflow(owner, thorough, { backend }, true, false);
                });
                auto* sourceBackendMenu = conversionsMenu->addMenu(mv::util::StyledIcon("database"), backendName);
                for (const auto targetBackend : backends)
                {
                    auto* conversionAction = sourceBackendMenu->addAction(mv::util::StyledIcon("right-left"), QObject::tr("To %1 · all element types").arg(pointdata::storage::getStorageBackendTypeName(targetBackend)));
                    QObject::connect(conversionAction, &QAction::triggered, owner, [owner, thorough, backend, targetBackend] {
                        startWorkflow(owner, thorough, { backend }, false, true, targetBackend);
                    });
                }
            }
        };

        auto* quickMenu = menu.addMenu(mv::util::StyledIcon("bolt"), QObject::tr("Quick"));
        populateMode(*quickMenu, false);

        auto* thoroughMenu = menu.addMenu(mv::util::StyledIcon("list-check"), QObject::tr("Thorough"));
        populateMode(*thoroughMenu, true);
    });
    return true;
}();
}
}
