// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "Dataset.h"
#include "Set.h"

#include "actions/OptionAction.h"

#include "event/EventListener.h"

#include "models/DatasetsListModel.h"
#include "models/DatasetsFilterModel.h"

#include <QAbstractListModel>

namespace mv::gui
{

/**
 * @brief Picks a dataset.
 *
 * DatasetPickerAction presents datasets through an option action and keeps the
 * available choices synchronized with the selected population mode.
 *
 * In automatic population mode, the action follows the global datasets model
 * and applies the configured filter function. In manual population mode, the
 * action uses the dataset list provided through setDatasets().
 *
 * The action tracks the selected dataset by identifier for serialization and
 * emits dataset selection signals when the current dataset changes.
 *
 * currentDatasetChanged() is the complete state-change signal. It emits a
 * valid dataset when a dataset is selected and an invalid dataset when the
 * selection is cleared. datasetPicked() is retained as the selection-specific
 * compatibility signal and is emitted only for valid datasets.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT DatasetPickerAction : public OptionAction
{
    Q_OBJECT

public:

    /**
     * @brief Constructs a dataset picker action.
     * @param parent Parent object.
     * @param title Action title.
     */
    Q_INVOKABLE DatasetPickerAction(QObject* parent, const QString& title);

    /**
     * @brief Returns selectable datasets.
     * @return Datasets currently visible through the picker after filtering.
     */
    [[nodiscard]] mv::Datasets getDatasets() const;

    /**
     * @brief Sets selectable datasets.
     *
     * Assigning datasets switches the action to manual population mode.
     *
     * @param datasets Datasets that can be selected.
     * @param silent Whether to suppress datasetsChanged() while replacing the list.
     */
    void setDatasets(mv::Datasets datasets, bool silent = false);

    /**
     * @brief Sets the dataset filter function.
     * @param filterFunction Function used by the filter model to decide which datasets are selectable.
     */
    void setFilterFunction(const DatasetsFilterModel::FilterFunction& filterFunction);

    /**
     * @brief Returns the current dataset.
     * @return Currently selected dataset, or an invalid dataset when no dataset is selected.
     */
    [[nodiscard]] mv::Dataset<> getCurrentDataset() const;

    /**
     * @brief Returns the current dataset as a typed dataset.
     * @tparam DatasetType Target dataset implementation type.
     * @return Currently selected dataset cast to \p DatasetType.
     */
    template<typename DatasetType>
    [[nodiscard]] mv::Dataset<DatasetType> getCurrentDataset() const
    {
        return mv::Dataset<DatasetType>(DatasetPickerAction::getCurrentDataset());
    }

    /**
     * @brief Sets the current dataset.
     * @param currentDataset Dataset to select, or an invalid dataset to clear the selection.
     */
    void setCurrentDataset(mv::Dataset<mv::DatasetImpl> currentDataset);

    /**
     * @brief Sets the current dataset by identifier.
     * @param datasetId Globally unique identifier of the dataset to select, or an empty string to clear the selection.
     */
    void setCurrentDataset(const QString& datasetId);

    /**
     * @brief Returns the current dataset identifier.
     * @return Globally unique identifier of the selected dataset, or an empty string when no dataset is selected.
     */
    [[nodiscard]] QString getCurrentDatasetId() const;

    /**
     * @brief Returns selectable dataset identifiers.
     * @return Globally unique identifiers of all currently selectable datasets.
     */
    [[nodiscard]] QStringList getCurrentDatasetIds() const;

    /**
     * @brief Returns a dataset by identifier.
     * @param datasetId Globally unique dataset identifier to look up.
     * @return Dataset with the requested identifier, or an invalid dataset when it is not selectable.
     */
    [[nodiscard]] Dataset<DatasetImpl> getDataset(const QString& datasetId) const;

    /**
     * @brief Refreshes the dataset filter.
     *
     * Invalidates the internal filter model so the selectable dataset list is
     * recomputed. This is primarily useful in automatic population mode.
     */
    void invalidateFilter();

public:

    /**
     * @brief Returns the population mode.
     * @return Current population mode.
     */
    [[nodiscard]] AbstractDatasetsModel::PopulationMode getPopulationMode() const;

    /**
     * @brief Sets the population mode.
     * @param populationMode Population mode to use for selectable datasets.
     */
    void setPopulationMode(AbstractDatasetsModel::PopulationMode populationMode);

private:

    /**
     * @brief Applies the current population mode.
     *
     * Reconnects the filter model to either the manual datasets model or the
     * global datasets model.
     */
    void populationModeChanged();

    /**
     * @brief Blocks datasetsChanged().
     */
    void blockDatasetsChangedSignal();

    /**
     * @brief Unblocks datasetsChanged().
     */
    void unblockDatasetsChangedSignal();

    /**
     * @brief Returns whether datasetsChanged() is blocked.
     * @return Whether datasetsChanged() is currently blocked.
     */
    [[nodiscard]] bool isDatasetsChangedSignalBlocked() const;

protected:

    /**
     * @brief Connects this action to a public action.
     * @param publicAction Public action to connect to.
     * @param recursive Whether to also connect descendant child actions.
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * @brief Disconnects this action from its public action.
     * @param recursive Whether to also disconnect descendant child actions.
     */
    void disconnectFromPublicAction(bool recursive) override;

public:

    /**
     * @brief Loads the action from a variant map.
     * @param variantMap Variant map representation of the action.
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * @brief Saves the action to a variant map.
     * @return Variant map representation of the action.
     */
    [[nodiscard]] QVariantMap toVariantMap() const override;

signals:

    /**
     * @brief Signals that the current dataset is about to change.
     *
     * The signal carries the previously selected dataset. The dataset can be
     * invalid when no dataset was selected.
     *
     * @param currentDataset Previously selected dataset, or an invalid dataset when no dataset was selected.
     */
    void datasetAboutToBePicked(mv::Dataset<> currentDataset);

    /**
     * @brief Signals that a valid dataset has been picked.
     *
     * This compatibility signal is emitted only for valid dataset selections.
     * Code that also needs to react to cleared selections should use
     * currentDatasetChanged().
     *
     * @param pickedDataset Newly selected dataset. The dataset is guaranteed to be valid.
     */
    void datasetPicked(mv::Dataset<> pickedDataset);

    /**
     * @brief Signals that the current dataset changed.
     *
     * This is the canonical state-change signal. A valid dataset means a
     * dataset is selected; an invalid dataset means the selection was cleared
     * or could no longer be resolved.
     *
     * @param dataset Newly selected dataset, or an invalid dataset when the selection was cleared.
     */
    void currentDatasetChanged(Dataset<DatasetImpl> dataset);

    /**
     * @brief Signals that selectable datasets changed.
     * @param datasets Current selectable datasets.
     */
    void datasetsChanged(mv::Datasets datasets);

    /**
     * @brief Signals that the population mode changed.
     * @param previousPopulationMode Previous population mode.
     * @param currentPopulationMode Current population mode.
     */
    void populationModeChanged(AbstractDatasetsModel::PopulationMode previousPopulationMode, AbstractDatasetsModel::PopulationMode currentPopulationMode);

private:

    AbstractDatasetsModel::PopulationMode   _populationMode;                /**< Selectable dataset population mode. */
    DatasetsListModel                       _datasetsListModel;             /**< Manual datasets model. */
    DatasetsFilterModel                     _datasetsFilterModel;           /**< Filter model applied to the active datasets model. */
    bool                                    _blockDatasetsChangedSignal;    /**< Whether datasetsChanged() is temporarily blocked. */
    QStringList                             _currentDatasetsIds;            /**< Cached selectable dataset identifiers. */
    QString                                 _currentDatasetId;              /**< Identifier of the currently selected dataset. */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::DatasetPickerAction)

inline const auto datasetPickerActionMetaTypeId = qRegisterMetaType<mv::gui::DatasetPickerAction*>("mv::gui::DatasetPickerAction");
