// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Dataset.h"
#include "Set.h"

#include "actions/WidgetAction.h"
#include "actions/OptionAction.h"

#include "event/EventListener.h"

#include "models/DatasetsListModel.h"
#include "models/DatasetsFilterModel.h"

#include <QAbstractListModel>

namespace mv::gui {

/**
 * Dataset picker action class
 *
 * Action class for picking a dataset from a list
 * Automatically removes items when datasets are removed and renamed
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT DatasetPickerAction : public OptionAction
{
Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param mode Picker mode
     */
    Q_INVOKABLE DatasetPickerAction(QObject* parent, const QString& title);

    /**
     * Get datasets
     * @return Datasets
     */
    mv::Datasets getDatasets() const;

    /**
     * Set the datasets from which can be picked (mode is set to StorageMode::Manual)
     * @param datasets Datasets from which can be picked
     * @param silent Whether the signal datasetsChanged is emitted
     */
    void setDatasets(mv::Datasets datasets, bool silent = false);

    /**
     * Set datasets filter function
     * @param datasetsFilterFunction Filter lambda (triggered when datasets are added and/or removed from the global datasets model)
     */
    void setFilterFunction(const DatasetsFilterModel::FilterFunction& filterFunction);

    /** Get the current dataset */
    mv::Dataset<mv::DatasetImpl> getCurrentDataset() const;

    /** Get the current dataset */
    template<typename DatasetType>
    inline mv::Dataset<DatasetType> getCurrentDataset() const
    {
        return getCurrentDataset();
    }

    /**
     * Set current dataset to \p currentDataset
     * @param currentDataset Smart pointer to current dataset
     */
    void setCurrentDataset(mv::Dataset<mv::DatasetImpl> currentDataset);

    /**
     * Set current dataset by \p datasetId
     * @param datasetId Current dataset globally unique identifier
     */
    void setCurrentDataset(const QString& datasetId);

    /**
     * Get current dataset globally unique identifier
     * @return The globally unique identifier of the currently selected dataset (if any)
     */
    QString getCurrentDatasetId() const;

public: // Population

    /**
     * Get current population mode
     * @return Population mode
     */
    AbstractDatasetsModel::PopulationMode getPopulationMode() const;

    /**
     * Set population mode to \p populationMode
     * @param populationMode Population mode
     */
    void setPopulationMode(AbstractDatasetsModel::PopulationMode populationMode);

private:

    /** Handle changes to the population mode */
    void populationModeChanged();

    /** Blocks the DatasetPickerAction::datasetsChanged() signal from being emitted */
    void blockDatasetsChangedSignal();

    /** Allows the DatasetPickerAction::datasetsChanged() signal to be emitted */
    void unblockDatasetsChangedSignal();

    /**
     * Get whether the DatasetPickerAction::datasetsChanged() may be emitted
     * @return Boolean determining whether the DatasetPickerAction::datasetsChanged() may be emitted
     */
    bool isDatasetsChangedSignalBlocked() const;

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that a dataset has been picked
     * @param Smart pointer to picked dataset
     */
    void datasetPicked(mv::Dataset<mv::DatasetImpl> pickedDataset);

    /**
     * Signals that selectable datasets changed
     * @param datasets Selectable datasets
     */
    void datasetsChanged(mv::Datasets datasets);

    /**
     * Signals that the population mode changed from \p previousPopulationMode to \p currentPopulationMode
     * @param previousPopulationMode Previous population mode
     * @param currentPopulationMode Current population mode
     */
    void populationModeChanged(AbstractDatasetsModel::PopulationMode previousPopulationMode, AbstractDatasetsModel::PopulationMode currentPopulationMode);

private:
    AbstractDatasetsModel::PopulationMode   _populationMode;                /** Population mode (e.g. manual or automatic) */
    DatasetsListModel                       _datasetsListModel;             /** Datasets list model for manual population (mv::data().getDatasetsListModel() otherwise) */
    DatasetsFilterModel                     _datasetsFilterModel;           /** Filter model for the datasets model above */
    bool                                    _blockDatasetsChangedSignal;    /** Boolean determining whether the DatasetPickerAction::datasetsChanged(...) signal may be engaged in reponse to change in the DatasetPickerAction#_filterModel */
    QStringList                             _currentDatasetsIds;            /** Keep a list of current datasets identifiers so that we can avoid unnecessary emits of the DatasetPickerAction::datasetsChanged(...) signal */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::DatasetPickerAction)

inline const auto datasetPickerActionMetaTypeId = qRegisterMetaType<mv::gui::DatasetPickerAction*>("mv::gui::DatasetPickerAction");
