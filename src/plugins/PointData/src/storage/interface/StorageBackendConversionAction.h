// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "../interface/PointsStorageBackendTypes.h"
#include "PointDataAccess.h"

#include <actions/GroupsAction.h>
#include <actions/MessageCardAction.h>
#include <actions/OptionAction.h>
#include <actions/VerticalGroupAction.h>
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
class POINTDATA_EXPORT StorageBackendConversionAction : public GroupsAction
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

    /** Restores the backend conversion controls from a preset map. */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /** Serializes the backend conversion controls for presets. */
    QVariantMap toVariantMap() const override;

    /** Supplies backend-neutral source access for bounded and full element-type analysis. The callback must remain safe to invoke on a worker thread. */
    void setSourceData(std::uint64_t rows, std::uint64_t columns, std::uint32_t sourceElementTypeIndex, SourceRowReader rowReader);

signals:
    void optionsChanged();

protected:
    /** Adds a control to the expanded settings section. */
    void addAction(WidgetAction* action);

    /** Returns the expanded settings section for backend-specific grouping. */
    VerticalGroupAction& getSettingsGroupAction();

    /** Adds a control to the pre-flight checks section. */
    void addCheckAction(WidgetAction* action);

    /** Adds a control to the advanced options section and shows that section. */
    void addAdvancedAction(WidgetAction* action);

    VerticalGroupAction& getAdvancedOptionsGroupAction();

    void notifyOptionsChanged() { emit optionsChanged(); }
    /** Moves the optional element-type analysis card after backend-specific controls and observations. */
    void moveElementTypeAnalysisActionToEnd();

    /** Returns the pre-flight checks group so derived actions can configure its presentation. */
    VerticalGroupAction& getChecksGroupAction();

private:
    void updatePreflightSummary();
    void updateElementTypeAnalysis();
    void startElementTypeAnalysis(bool fullScan);
    void cancelElementTypeAnalysis();

    VerticalGroupAction             _settingsGroupAction;
    VerticalGroupAction             _advancedOptionsGroupAction;
    VerticalGroupAction             _checksGroupAction;
    OptionAction                    _elementTypeAction;           /**< Optional selector for the converted matrix element type. */
    MessageCardAction               _elementTypeAnalysisAction;   /**< Sample and full-scan findings for the selected element type. */
    bool                            _hasElementTypeAction = false; /**< Whether an element-type selector is available. */
    std::optional<std::uint32_t>    _initialElementTypeIndex;     /**< Element type selected when this action was created. */
    std::uint64_t                   _sourceRows = 0;               /**< Number of source matrix rows available for analysis. */
    std::uint64_t                   _sourceColumns = 0;            /**< Number of source matrix columns available for analysis. */
    std::uint32_t                   _sourceElementTypeIndex = 0;   /**< Element type used by the source matrix. */
    SourceRowReader                 _sourceRowReader;               /**< Lifetime-safe row access used by background analysis. */
    std::shared_ptr<std::atomic_bool> _cancelAnalysis;               /**< Cancellation flag for the active scan, if any. */
    std::uint64_t                   _analysisGeneration = 0;       /**< Identifies the latest requested analysis result. */
};

}
