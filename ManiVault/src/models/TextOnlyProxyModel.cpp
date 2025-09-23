// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TextOnlyProxyModel.h"

#include <QDebug>

namespace mv
{

QVariant TextOnlyProxyModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole && index.column() == _completionColumn)
        return QStringLiteral("%1").arg(QIdentityProxyModel::data(index, role).toString());

    // Hide everything else from the completer (decorations, tooltips, etc.)
    if (role == Qt::DecorationRole || role == Qt::SizeHintRole || role == Qt::ToolTipRole)
        return {};

    // For non-completion columns, only expose display text if needed
    if (role == Qt::DisplayRole)
        return {};

    return QIdentityProxyModel::data(index, role);
}

std::int32_t TextOnlyProxyModel::getCompletionColumn() const
{
    return _completionColumn;
}

void TextOnlyProxyModel::setCompletionColumn(std::int32_t completionColumn)
{
    if (_completionColumn == completionColumn)
        return;

    _completionColumn = completionColumn;

    emit completionColumnChanged(_completionColumn);
}

}
