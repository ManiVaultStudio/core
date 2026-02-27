// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QStandardItemModel>

#include "ManiVaultGlobals.h"

#include "util/Serializable.h"

#include "NumberOfRowsAction.h"

namespace mv
{

/**
 * Standard item model class
 *
 * Base MV standard item model class
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT StandardItemModel : public QStandardItemModel, public util::Serializable
{
    Q_OBJECT

public:

    /** Model population modes */
    enum class PopulationMode
    {
        Automatic,              /** Automatically populate the model from the Data Source Names (DSNs) */
        AutomaticSynchronous,   /** Automatically populate the model from the DSNs in a synchronous manner */
        Manual                  /** Manually populate the model from a JSON file or byte array */
    };

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     * @param title Title of the model
     * @param populationMode Population mode of the model (automatic/manual)
     */
    StandardItemModel(QObject* parent = nullptr, const QString& title = "StandardItemModel", const PopulationMode& populationMode = PopulationMode::Automatic);

    /**
     * Get the population mode of the model
     * @return Population mode (automatic/manual) of the model
     */
    PopulationMode getPopulationMode() const;

    /**
     * Set the population mode of the model to \p populationMode
     * @param populationMode Population mode (automatic/manual) of the model
     */
    void setPopulationMode(const PopulationMode& populationMode);

public: // Settings

    /**
     * Get settings prefix (Models/<serialization_name>)
     * @return Settings prefix for the model, used for saving/loading settings
     */
    virtual QString getSettingsPrefix() const { return QString("Models/%1").arg(getSerializationName()); }

public: // Serialization

    /**
     * Load from variant map
     * @param variantMap Variant map
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save to variant map
     * @return Variant map
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    gui::NumberOfRowsAction& getNumberOfRowsAction() { return *_numberOfRowsAction; }
    
    const gui::NumberOfRowsAction& getNumberOfRowsAction() const { return *_numberOfRowsAction; }

signals:

    /**
     * Signals that the population mode changed to \p populationMode
     * @param populationMode New population mode of the model
     */
    void populationModeChanged(const PopulationMode& populationMode);

private:
    PopulationMode                      _populationMode;        /** Population mode of the model */
    QPointer<gui::NumberOfRowsAction>   _numberOfRowsAction;    /** String action for displaying the number of rows */
};

}
