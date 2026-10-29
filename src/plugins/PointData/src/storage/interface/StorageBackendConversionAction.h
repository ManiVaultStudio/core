// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "../interface/PointsStorageBackendTypes.h"
#include "PointDataAccess.h"

#include <actions/GroupAction.h>
#include <actions/MessageCardAction.h>
#include <actions/OptionAction.h>
#include <PointData/pointdata_export.h>

#include <atomic>
#include <functional>
#include <memory>

namespace mv::gui
{

/**
 * @brief Describes the user controls and options for one backend conversion.
 *
 * A conversion action belongs to the backend that will receive the converted
 * data. This keeps backend-specific settings, validation, and presentation out
 * of the generic conversion dialog and allows future backends to expose their
 * own options without expanding StorageBackendAction for every new format.
 * The action does not perform conversion itself; it only owns the controls
 * from which the conversion options are collected.
 *
 * @author T. Kroes
 */
class POINTDATA_EXPORT StorageBackendConversionAction : public GroupAction
{
    Q_OBJECT

public:
    using SourceRowReader = std::function<PointDataAccessView(std::uint64_t)>;

    /** Constructs backend settings, optionally adding an element-type selector initialized to the current type. */
    StorageBackendConversionAction(QObject* parent, const QString& title, std::optional<std::uint32_t> currentElementTypeIndex = {});

    ~StorageBackendConversionAction() override;

    /** Returns the target element type selected for the conversion, when available. */
    std::optional<std::uint32_t> getTargetElementTypeIndex() const;

    /** Returns the target element size selected for the conversion, when available. */
    std::optional<std::size_t> getTargetElementSize() const;

    /** Returns the backend configured by this action. */
    virtual pointdata::storage::StorageBackendType getTargetBackendType() const = 0;

    /** Returns the current backend-specific conversion options. */
    virtual pointdata::storage::StorageBackendConfigurationOptionsPtr getConversionOptions() const = 0;

    /** Returns whether the current settings pass the action's known preflight checks. */
    virtual bool canApply() const { return true; }

    /** Returns whether the current backend settings differ from their initial values. */
    virtual bool hasChanges() const;

    /** Supplies backend-neutral source access for bounded and full element-type analysis. The callback must remain safe to invoke on a worker thread. */
    void setSourceData(std::uint64_t rows, std::uint64_t columns, std::uint32_t sourceElementTypeIndex, SourceRowReader rowReader);

signals:
    void optionsChanged();

protected:
    void notifyOptionsChanged() { emit optionsChanged(); }

private:
    void updateElementTypeAnalysis();
    void startElementTypeAnalysis(bool fullScan);
    void cancelElementTypeAnalysis();

    OptionAction                    _elementTypeAction;           /**< Optional selector for the converted matrix element type. */
    MessageCardAction               _elementTypeAnalysisAction;   /**< Sample and full-scan findings for the selected element type. */
    bool                            _hasElementTypeAction = false; /**< Whether an element-type selector is available. */
    bool                            _elementTypeAnalysisAdded = false; /**< Whether the analysis card is currently part of the group. */
    std::optional<std::uint32_t>    _initialElementTypeIndex;     /**< Element type selected when this action was created. */
    std::uint64_t                   _sourceRows = 0;               /**< Number of source matrix rows available for analysis. */
    std::uint64_t                   _sourceColumns = 0;            /**< Number of source matrix columns available for analysis. */
    std::uint32_t                   _sourceElementTypeIndex = 0;   /**< Element type used by the source matrix. */
    SourceRowReader                 _sourceRowReader;               /**< Lifetime-safe row access used by background analysis. */
    std::shared_ptr<std::atomic_bool> _cancelAnalysis;               /**< Cancellation flag for the active scan, if any. */
    std::uint64_t                   _analysisGeneration = 0;       /**< Identifies the latest requested analysis result. */
};

}
