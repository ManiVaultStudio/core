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

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     * @param title Title of the model
     */
    StandardItemModel(QObject* parent = nullptr, const QString& title = "StandardItemModel");

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

    gui::NumberOfRowsAction& getNumberOfRowsAction() { return _numberOfRowsAction; }
    
    const gui::NumberOfRowsAction& getNumberOfRowsAction() const { return _numberOfRowsAction; }

private:
    gui::NumberOfRowsAction     _numberOfRowsAction;    /** String action for displaying the number of rows */
};

}
