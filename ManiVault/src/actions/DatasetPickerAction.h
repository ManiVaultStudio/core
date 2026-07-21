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

namespace mv::gui {

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
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT DatasetPickerAction : public OptionAction
{
    Q_OBJECT

public:

    /**
     * @brief Construct a dataset picker action.
     * @param parent Parent object.
     * @param title Action title.
     */
    Q_INVOKABLE DatasetPickerAction(QObject* parent, const QString& title);

    /**
     * @brief Get selectable datasets.
     * @return Datasets currently visible through the picker after filtering.
     */
    mv::Datasets getDatasets() const;

    /**
     * @brief Set selectable datasets.
     *
     * Assigning datasets switches the action to manual population mode.
     *
     * @param datasets Datasets that can be selected.
     * @param silent Whether to suppress datasetsChanged() while replacing the list.
     */
    void setDatasets(mv::Datasets datasets, bool silent = false);

    /**
     * @brief Set the dataset filter function.
     * @param filterFunction Function used by the filter model to decide which datasets are selectable.
     */
    void setFilterFunction(const DatasetsFilterModel::FilterFunction& filterFunction);

    /**
     * @brief Get the current dataset.
     * @return Currently selected dataset, or an invalid dataset when no dataset is selected.
     */
    mv::Dataset<> getCurrentDataset() const;

    /**
     * @brief Get the current dataset as a typed dataset.
     * @return Currently selected dataset cast to \p DatasetType.
     */
    template<typename DatasetType>
    mv::Dataset<DatasetType> getCurrentDataset() const
    {
        return mv::Dataset<DatasetType>(DatasetPickerAction::getCurrentDataset());
    }

    /**
     * @brief Set the current dataset.
     * @param currentDataset Dataset to select, or an invalid dataset to clear the selection.
     */
    void setCurrentDataset(mv::Dataset<mv::DatasetImpl> currentDataset);

    /**
     * @brief Set the current dataset by identifier.
     * @param datasetId Globally unique identifier of the dataset to select, or an empty string to clear the selection.
     */
    void setCurrentDataset(const QString& datasetId);

    /**
     * @brief Get the current dataset identifier.
     * @return Globally unique identifier of the selected dataset, or an empty string when no dataset is selected.
     */
    QString getCurrentDatasetId() const;

    /**
     * @brief Get selectable dataset identifiers.
     * @return Globally unique identifiers of all currently selectable datasets.
     */
    QStringList getCurrentDatasetIds() const;

    /**
     * @brief Get a dataset by identifier.
     * @param datasetId Globally unique dataset identifier to look up.
     * @return Dataset with the requested identifier, or an invalid dataset when it is not selectable.
     */
    Dataset<DatasetImpl> getDataset(const QString& datasetId) const;

    /**
     * @brief Refresh the dataset filter.
     *
     * Invalidates the internal filter model so the selectable dataset list is
     * recomputed. This is primarily useful in automatic population mode.
     */
    void invalidateFilter();

public: // Population

    /**
     * @brief Get the population mode.
     * @return Current population mode.
     */
    AbstractDatasetsModel::PopulationMode getPopulationMode() const;

    /**
     * @brief Set the population mode.
     * @param populationMode Population mode to use for selectable datasets.
     */
    void setPopulationMode(AbstractDatasetsModel::PopulationMode populationMode);

private:

    /**
     * @brief Apply the current population mode.
     *
     * Reconnects the filter model to either the manual datasets model or the
     * global datasets model.
     */
    void populationModeChanged();

    /**
     * @brief Block datasetsChanged().
     */
    void blockDatasetsChangedSignal();

    /**
     * @brief Unblock datasetsChanged().
     */
    void unblockDatasetsChangedSignal();

    /**
     * @brief Get whether datasetsChanged() is blocked.
     * @return Whether datasetsChanged() is currently blocked.
     */
    bool isDatasetsChangedSignalBlocked() const;

protected: // Linking

    /**
     * @brief Connect this action to a public action.
     * @param publicAction Public action to connect to.
     * @param recursive Whether to also connect descendant child actions.
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * @brief Disconnect this action from its public action.
     * @param recursive Whether to also disconnect descendant child actions.
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
     * @brief Load the action from a variant map.
     * @param variantMap Variant map representation of the action.
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * @brief Save the action to a variant map.
     * @return Variant map representation of the action.
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * @brief Signals that the current dataset is about to change.
     * @param currentDataset Previously selected dataset, or an invalid dataset when no dataset was selected.
     */
    void datasetAboutToBePicked(mv::Dataset<> currentDataset);

    /**
     * @brief Signals that a dataset has been picked.
     * @param pickedDataset Newly selected dataset, or an invalid dataset when the selection was cleared.
     */
    void datasetPicked(mv::Dataset<> pickedDataset);

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
    AbstractDatasetsModel::PopulationMode   _populationMode;                /**< Selectable dataset population mode */
    DatasetsListModel                       _datasetsListModel;             /**< Manual datasets model */
    DatasetsFilterModel                     _datasetsFilterModel;           /**< Filter model applied to the active datasets model */
    bool                                    _blockDatasetsChangedSignal;    /**< Whether datasetsChanged() is temporarily blocked */
    QStringList                             _currentDatasetsIds;            /**< Cached selectable dataset identifiers */
    QString                                 _currentDatasetId;              /**< Identifier of the currently selected dataset */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::DatasetPickerAction)

inline const auto datasetPickerActionMetaTypeId = qRegisterMetaType<mv::gui::DatasetPickerAction*>("mv::gui::DatasetPickerAction");
