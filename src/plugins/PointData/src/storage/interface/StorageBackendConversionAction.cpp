// SPDX-License-Identifier: LGPL-3.0-or-later

#include "StorageBackendConversionAction.h"

#include "../../PointData.h"

#include <Application.h>
#include <Task.h>
#include <workflow/WorkflowExecutionContext.h>
#include <workflow/WorkflowOptions.h>
#include <workflow/WorkflowPlan.h>

#include <QStringList>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace mv::gui
{

namespace
{
    struct ElementTypeAnalysisResult
    {
        std::uint64_t valuesChecked = 0;
        std::uint64_t valuesOutOfRange = 0;
        std::uint64_t valuesLosePrecision = 0;
        bool outOfRangeFailsConversion = false;
        bool complete = false;
        bool cancelled = false;
        QString error;
    };

    template<typename ValueType>
    long double readValue(const char* bytes)
    {
        ValueType value;
        std::memcpy(&value, bytes, sizeof(ValueType));
        if constexpr (std::is_same_v<ValueType, biovault::bfloat16_t>)
            return static_cast<long double>(static_cast<float>(value));
        else
            return static_cast<long double>(value);
    }

    template<typename TargetType>
    void analyzeValue(const long double value, ElementTypeAnalysisResult& result)
    {
        if constexpr (std::is_integral_v<TargetType>)
        {
            if (!std::isfinite(value) || value < static_cast<long double>(std::numeric_limits<TargetType>::lowest()) || value > static_cast<long double>(std::numeric_limits<TargetType>::max()))
                ++result.valuesOutOfRange;
            else if (static_cast<long double>(static_cast<TargetType>(value)) != value)
                ++result.valuesLosePrecision;
        }
        else
        {
            TargetType targetValue;
            if constexpr (std::is_same_v<TargetType, biovault::bfloat16_t>)
                targetValue = biovault::bfloat16_t(static_cast<float>(value));
            else
                targetValue = static_cast<TargetType>(value);

            const auto roundTrip = [&] {
                if constexpr (std::is_same_v<TargetType, biovault::bfloat16_t>)
                    return static_cast<long double>(static_cast<float>(targetValue));
                else
                    return static_cast<long double>(targetValue);
            }();

            if (std::isfinite(value) && !std::isfinite(roundTrip))
                ++result.valuesOutOfRange;
            else if (roundTrip != value && !(std::isnan(roundTrip) && std::isnan(value)))
                ++result.valuesLosePrecision;
        }
    }

    template<typename SourceType, typename TargetType>
    void analyzeRow(const PointDataAccessView& row, const std::vector<std::uint64_t>& columns, ElementTypeAnalysisResult& result)
    {
        for (const auto column : columns)
        {
            analyzeValue<TargetType>(readValue<SourceType>(row.rowData(0) + column * row.elementSizeBytes), result);
            ++result.valuesChecked;
        }
    }

    template<typename SourceType>
    void analyzeRowForTarget(const std::uint32_t targetTypeIndex, const PointDataAccessView& row, const std::vector<std::uint64_t>& columns, ElementTypeAnalysisResult& result)
    {
        switch (targetTypeIndex)
        {
            case 0: return analyzeRow<SourceType, float>(row, columns, result);
            case 1: return analyzeRow<SourceType, biovault::bfloat16_t>(row, columns, result);
            case 2: return analyzeRow<SourceType, std::int32_t>(row, columns, result);
            case 3: return analyzeRow<SourceType, std::uint32_t>(row, columns, result);
            case 4: return analyzeRow<SourceType, std::int16_t>(row, columns, result);
            case 5: return analyzeRow<SourceType, std::uint16_t>(row, columns, result);
            case 6: return analyzeRow<SourceType, std::int8_t>(row, columns, result);
            case 7: return analyzeRow<SourceType, std::uint8_t>(row, columns, result);
            default: throw std::invalid_argument("Point-data analysis uses an unsupported target element type");
        }
    }

    void analyzeRow(const std::uint32_t sourceTypeIndex, const std::uint32_t targetTypeIndex, const PointDataAccessView& row, const std::vector<std::uint64_t>& columns, ElementTypeAnalysisResult& result)
    {
        switch (sourceTypeIndex)
        {
            case 0: return analyzeRowForTarget<float>(targetTypeIndex, row, columns, result);
            case 1: return analyzeRowForTarget<biovault::bfloat16_t>(targetTypeIndex, row, columns, result);
            case 2: return analyzeRowForTarget<std::int32_t>(targetTypeIndex, row, columns, result);
            case 3: return analyzeRowForTarget<std::uint32_t>(targetTypeIndex, row, columns, result);
            case 4: return analyzeRowForTarget<std::int16_t>(targetTypeIndex, row, columns, result);
            case 5: return analyzeRowForTarget<std::uint16_t>(targetTypeIndex, row, columns, result);
            case 6: return analyzeRowForTarget<std::int8_t>(targetTypeIndex, row, columns, result);
            case 7: return analyzeRowForTarget<std::uint8_t>(targetTypeIndex, row, columns, result);
            default: throw std::invalid_argument("Point-data analysis uses an unsupported source element type");
        }
    }

    ElementTypeAnalysisResult analyzeElementType(const std::uint64_t rows, const std::uint64_t columns, const std::uint32_t sourceTypeIndex, const std::uint32_t targetTypeIndex, const bool fullScan, const StorageBackendConversionAction::SourceRowReader& rowReader, const std::shared_ptr<std::atomic_bool>& cancelled, const workflow::SharedWorkflowExecutionContext& context)
    {
        ElementTypeAnalysisResult result;
        result.complete = fullScan;
        result.outOfRangeFailsConversion = targetTypeIndex >= 2;

        try
        {
            if (rows == 0 || columns == 0)
                return result;

            const auto rowSampleCount = fullScan ? rows : std::min<std::uint64_t>(rows, 256);
            const auto columnSampleCount = fullScan ? columns : std::min<std::uint64_t>(columns, std::max<std::uint64_t>(1, 65536 / rowSampleCount));
            std::vector<std::uint64_t> sampledColumns;
            sampledColumns.reserve(static_cast<std::size_t>(columnSampleCount));
            for (std::uint64_t columnIndex = 0; columnIndex < columnSampleCount; ++columnIndex)
                sampledColumns.push_back(fullScan ? columnIndex : static_cast<std::uint64_t>((static_cast<long double>(columnIndex) * columns) / columnSampleCount));

            for (std::uint64_t rowIndex = 0; rowIndex < rowSampleCount; ++rowIndex)
            {
                const auto task = context ? context->getTask() : nullptr;
                if (cancelled->load() || (task && (task->isAboutToBeAborted() || task->isAborting() || task->isAborted())))
                {
                    result.complete = false;
                    result.cancelled = true;
                    return result;
                }

                if (rowIndex % std::max<std::uint64_t>(1, rowSampleCount / 100) == 0)
                    context->setProgress(static_cast<double>(rowIndex) / static_cast<double>(rowSampleCount));

                const auto sourceRowIndex = fullScan ? rowIndex : static_cast<std::uint64_t>((static_cast<long double>(rowIndex) * rows) / rowSampleCount);
                const auto row = rowReader(sourceRowIndex);
                if (!row || row.columns < columns || row.elementTypeIndex != sourceTypeIndex || row.elementSizeBytes != PointData::getElementTypeSize(sourceTypeIndex))
                    throw std::runtime_error("The source row is unavailable or changed during element-type analysis");

                analyzeRow(sourceTypeIndex, targetTypeIndex, row, sampledColumns, result);
            }

            context->setProgress(1.0);
        }
        catch (const std::exception& exception)
        {
            result.error = QString::fromUtf8(exception.what());
            result.complete = false;
        }

        return result;
    }

    QString summarizeAnalysis(const ElementTypeAnalysisResult& result, const bool fullScan)
    {
        auto description = QStringLiteral("Checked %1 value%2.").arg(result.valuesChecked).arg(result.valuesChecked == 1 ? QString() : QStringLiteral("s"));
        if (result.valuesOutOfRange != 0)
            description += result.outOfRangeFailsConversion ? QStringLiteral(" %1 value%2 exceed the target type's range, so conversion will fail.").arg(result.valuesOutOfRange).arg(result.valuesOutOfRange == 1 ? QString() : QStringLiteral("s")) : QStringLiteral(" %1 value%2 exceed the target type's finite range and may become infinity.").arg(result.valuesOutOfRange).arg(result.valuesOutOfRange == 1 ? QString() : QStringLiteral("s"));
        if (result.valuesLosePrecision != 0)
            description += QStringLiteral(" %1 value%2 would be rounded or truncated.").arg(result.valuesLosePrecision).arg(result.valuesLosePrecision == 1 ? QString() : QStringLiteral("s"));
        if (result.valuesOutOfRange == 0 && result.valuesLosePrecision == 0)
            description += fullScan ? QStringLiteral(" No values would change in this conversion.") : QStringLiteral(" No loss was found in this sample; uncommon values may be missed.");
        if (!result.error.isEmpty())
            description += QStringLiteral(" Analysis stopped: %1").arg(result.error);
        else if (!fullScan)
            description += QStringLiteral(" This bounded sample is only an estimate.");
        else if (!result.complete)
            description += QStringLiteral(" The full scan did not complete.");
        return description;
    }
}

StorageBackendConversionAction::StorageBackendConversionAction(QObject* parent, const QString& title, const std::optional<std::uint32_t> currentElementTypeIndex) :
    GroupAction(parent, title),
    _elementTypeAction(this, "Element type"),
    _elementTypeAnalysisAction(this)
{
    setLabelSizingType(LabelSizingType::Auto);

    if (!currentElementTypeIndex)
        return;

    _initialElementTypeIndex = currentElementTypeIndex;

    const auto elementTypeNames = PointData::getElementTypeNames();
    if (*currentElementTypeIndex >= elementTypeNames.size())
        throw std::invalid_argument("Storage conversion uses an unsupported element type");

    QStringList elementTypeOptions;
    for (const auto* elementTypeName : elementTypeNames)
        elementTypeOptions.append(QString::fromLatin1(elementTypeName));

    _hasElementTypeAction = true;
    _elementTypeAction.setOptions(elementTypeOptions);
    _elementTypeAction.setCurrentIndex(static_cast<int>(*currentElementTypeIndex));
    addAction(&_elementTypeAction);

    connect(&_elementTypeAction, &OptionAction::currentIndexChanged, this, [this] {
        notifyOptionsChanged();
        updateElementTypeAnalysis();
    });
}

StorageBackendConversionAction::~StorageBackendConversionAction()
{
    cancelElementTypeAnalysis();
}

void StorageBackendConversionAction::setSourceData(const std::uint64_t rows, const std::uint64_t columns, const std::uint32_t sourceElementTypeIndex, SourceRowReader rowReader)
{
    _sourceRows = rows;
    _sourceColumns = columns;
    _sourceElementTypeIndex = sourceElementTypeIndex;
    _sourceRowReader = std::move(rowReader);
    updateElementTypeAnalysis();
}

void StorageBackendConversionAction::updateElementTypeAnalysis()
{
    cancelElementTypeAnalysis();

    const auto targetElementTypeIndex = getTargetElementTypeIndex();
    if (!_sourceRowReader || !targetElementTypeIndex || *targetElementTypeIndex == _sourceElementTypeIndex)
    {
        if (_elementTypeAnalysisAdded)
        {
            removeAction(&_elementTypeAnalysisAction);
            _elementTypeAnalysisAdded = false;
        }
        return;
    }

    if (!_elementTypeAnalysisAdded)
    {
        addAction(&_elementTypeAnalysisAction);
        _elementTypeAnalysisAdded = true;
    }

    _elementTypeAnalysisAction.setMessage(util::SeverityLevel::Info, "Element type", "Checking a bounded sample across the matrix. This may miss uncommon values.");
    startElementTypeAnalysis(false);
}

void StorageBackendConversionAction::startElementTypeAnalysis(const bool fullScan)
{
    if (!_sourceRowReader)
        return;

    cancelElementTypeAnalysis();
    const auto generation = ++_analysisGeneration;
    const auto cancelled = std::make_shared<std::atomic_bool>(false);
    _cancelAnalysis = cancelled;
    const auto rows = _sourceRows;
    const auto columns = _sourceColumns;
    const auto sourceTypeIndex = _sourceElementTypeIndex;
    const auto targetTypeIndex = getTargetElementTypeIndex().value_or(sourceTypeIndex);
    const auto rowReader = _sourceRowReader;
    const auto scanResult = std::make_shared<ElementTypeAnalysisResult>();
    _elementTypeAnalysisAction.setMessage(util::SeverityLevel::Info, "Element type", fullScan ? "Scanning every matrix value…" : "Checking a bounded sample across the matrix…", {{ "Cancel scan", [this] {
        cancelElementTypeAnalysis();
        _elementTypeAnalysisAction.setMessage(util::SeverityLevel::Info, "Element type", "Analysis cancelled.");
    } }});

    auto workflow = std::make_unique<workflow::WorkflowPlan>(fullScan ? "Analyze target element type" : "Sample target element type");
    workflow->addSequentialStage(fullScan ? "Scan matrix values" : "Sample matrix values", [scanResult, rows, columns, sourceTypeIndex, targetTypeIndex, fullScan, rowReader, cancelled](const workflow::WorkflowPlan::Job&, const workflow::SharedWorkflowExecutionContext& context) {
        *scanResult = analyzeElementType(rows, columns, sourceTypeIndex, targetTypeIndex, fullScan, rowReader, cancelled, context);
    }, workflow::WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, 1.0);

    workflow::WorkflowOptions options;
    options.cancellation.enabled = true;
    options.reporting.progress = true;
    options.reporting.finishedNotification = false;

    auto future = Application::getWorkflowPlanExecutor().execute(std::move(workflow), nullptr, options);
    future.onFinished(this, [this, scanResult, generation, fullScan](const workflow::SharedWorkflowResult& workflowResult) {
        if (generation != _analysisGeneration)
            return;

        _cancelAnalysis.reset();
        if (!workflowResult || workflowResult->getStatus() == workflow::WorkflowResultBase::Status::Canceled || scanResult->cancelled)
        {
            _elementTypeAnalysisAction.setMessage(util::SeverityLevel::Info, "Element type", "Analysis cancelled.");
            return;
        }

        if (workflowResult->getStatus() != workflow::WorkflowResultBase::Status::Success && scanResult->error.isEmpty())
            scanResult->error = "The analysis workflow failed.";

        const auto hasIssues = scanResult->valuesOutOfRange != 0 || scanResult->valuesLosePrecision != 0 || !scanResult->error.isEmpty() || workflowResult->getStatus() != workflow::WorkflowResultBase::Status::Success;
        const auto severity = hasIssues ? util::SeverityLevel::Warning : util::SeverityLevel::Info;
        const auto category = fullScan ? "Full scan" : "Sample";
        auto buttons = MessageCardAction::ButtonActions {};
        if (!fullScan && scanResult->error.isEmpty())
            buttons.append({ "Full scan", [this] { startElementTypeAnalysis(true); } });
        else if (fullScan && scanResult->complete && scanResult->error.isEmpty())
            buttons.append({ "Scan again", [this] { startElementTypeAnalysis(true); } });
        _elementTypeAnalysisAction.setMessage(severity, category, summarizeAnalysis(*scanResult, fullScan), buttons);
    });
}

void StorageBackendConversionAction::cancelElementTypeAnalysis()
{
    if (_cancelAnalysis)
        _cancelAnalysis->store(true);
    _cancelAnalysis.reset();
    ++_analysisGeneration;
}

std::optional<std::uint32_t> StorageBackendConversionAction::getTargetElementTypeIndex() const
{
    if (!_hasElementTypeAction)
        return {};

    const auto index = _elementTypeAction.getCurrentIndex();
    return index >= 0 ? std::optional<std::uint32_t>(static_cast<std::uint32_t>(index)) : std::nullopt;
}

std::optional<std::size_t> StorageBackendConversionAction::getTargetElementSize() const
{
    const auto elementTypeIndex = getTargetElementTypeIndex();
    return elementTypeIndex ? std::optional<std::size_t>(PointData::getElementTypeSize(*elementTypeIndex)) : std::nullopt;
}

bool StorageBackendConversionAction::hasChanges() const
{
    return getTargetElementTypeIndex() != _initialElementTypeIndex;
}

}
