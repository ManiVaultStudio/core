// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QIdentityProxyModel>

namespace mv
{

/**
 * Text only proxy model class
 *
 * Forwards only the display role for the specified completion column.
 *
 * Note: This is primarily used for use cases where performance in large models is needed.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT TextOnlyProxyModel : public QIdentityProxyModel {

    Q_OBJECT

public:

    /** No need for custom constructor s*/
    using QIdentityProxyModel::QIdentityProxyModel;

    /**
     * Get the data for the model \p index and \p role
     * @param index Index of the data to retrieve
     * @param role Role of the data to retrieve
     * @return Data for the model index and role
     */
    QVariant data(const QModelIndex& index, int role) const override;

    /**
     * Get the completion column
     * @return Column index of the completion column
     */
    std::int32_t getCompletionColumn() const;

    /**
     * Set the completion column to \p completionColumn
     * @param completionColumn Column index of the completion column
     */
    void setCompletionColumn(std::int32_t completionColumn);

signals:

    /**
     * Signals that the completion column changed to \p column
     * @param column Column index of the completion column
     */
    void completionColumnChanged(std::int32_t column);

private:
    int      _completionColumn = 0;
};


}
