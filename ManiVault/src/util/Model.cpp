// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Model.h"

#include <QStandardItemModel>

namespace mv::util
{

QList<QPersistentModelIndex> getAllPersistentModelIndexesAtColumn(const QAbstractItemModel* model, std::int32_t columnIndex)
{
    QList<QPersistentModelIndex> results;

    std::function<void(const QPersistentModelIndex&)> recurse = [&](const QPersistentModelIndex& parent) {
        if (!parent.isValid() || parent.column() == columnIndex)
            results.append(model->index(parent.row(), columnIndex, parent.parent()));

        const int rowCount = model->rowCount(parent);

        for (int row = 0; row < rowCount; ++row)
            recurse(model->index(row, columnIndex, parent));
	};

    const int topLevelRows = model->rowCount();

	for (int row = 0; row < topLevelRows; ++row)
        recurse(model->index(row, columnIndex));

    return results;
}

QList<QStandardItem*> getAllStandardItemsAtColumn(const QStandardItemModel* model, std::int32_t columnIndex)
{
    QList<QStandardItem*> results;

    const auto indexes = getAllPersistentModelIndexesAtColumn(model, columnIndex);

    for (const auto& index : indexes) {
        if (index.isValid())
            if (auto item = model->itemFromIndex(index))
                results.append(item);
    }

    return results;
}

}
