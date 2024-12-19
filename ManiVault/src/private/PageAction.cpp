// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageAction.h"
#include "PageActionsModel.h"

bool PageAction::compactView = false;

PageAction::PageAction(const QIcon& icon, const QString& title, const QString& subtitle, const QString& description, const QString& tooltip, const ClickedCallback& clickedCallback) :
    _icon(icon),
    _title(title),
    _subtitle(subtitle),
    _description(description),
    _tooltip(tooltip),
    _clickedCallback(clickedCallback)
{
}

PageAction::PageAction(const QModelIndex& index)
{
    setEditorData(index);
}

void PageAction::setEditorData(const QModelIndex& index)
{
    setIcon(index.siblingAtColumn(static_cast<int>(PageActionsModel::Column::Icon)).data(Qt::UserRole + 1).value<QIcon>());
    setTitle(index.siblingAtColumn(static_cast<int>(PageActionsModel::Column::Title)).data(Qt::EditRole).toString());
    setDescription(index.siblingAtColumn(static_cast<int>(PageActionsModel::Column::Description)).data(Qt::EditRole).toString());
    setComments(index.siblingAtColumn(static_cast<int>(PageActionsModel::Column::Comments)).data(Qt::EditRole).toString());
    setTags(index.siblingAtColumn(static_cast<int>(PageActionsModel::Column::Tags)).data(Qt::EditRole).toStringList());
    setSubtitle(index.siblingAtColumn(static_cast<int>(PageActionsModel::Column::Subtitle)).data(Qt::EditRole).toString());
    setMetaData(index.siblingAtColumn(static_cast<int>(PageActionsModel::Column::MetaData)).data(Qt::EditRole).toString());
    setPreviewImage(index.siblingAtColumn(static_cast<int>(PageActionsModel::Column::PreviewImage)).data(Qt::UserRole + 1).value<QImage>());
    setDownloadUrls(index.siblingAtColumn(static_cast<int>(PageActionsModel::Column::DownloadUrls)).data(Qt::EditRole).toStringList());
    setContributors(index.siblingAtColumn(static_cast<int>(PageActionsModel::Column::Contributors)).data(Qt::UserRole + 1).toStringList());
}

bool PageAction::isCompactView()
{
    return PageAction::compactView;
}

void PageAction::setCompactView(bool compactView)
{
    PageAction::compactView = compactView;
}
