// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageAction.h"
#include "StartPageActionsModel.h"

bool StartPageAction::compactView = false;

StartPageAction::StartPageAction(const QIcon& icon, const QString& title, const QString& subtitle, const QString& description, const QString& tooltip, const ClickedCallback& clickedCallback) :
    _icon(icon),
    _title(title),
    _subtitle(subtitle),
    _description(description),
    _tooltip(tooltip),
    _clickedCallback(clickedCallback)
{
}

StartPageAction::StartPageAction(const QModelIndex& index)
{
    setEditorData(index);
}

void StartPageAction::setEditorData(const QModelIndex& index)
{
    setIcon(index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Icon)).data(Qt::UserRole + 1).value<QIcon>());
    setTitle(index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Title)).data(Qt::EditRole).toString());
    setDescription(index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Description)).data(Qt::EditRole).toString());
    setComments(index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Comments)).data(Qt::EditRole).toString());
    setTags(index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Tags)).data(Qt::EditRole).toStringList());
    setSubtitle(index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Subtitle)).data(Qt::EditRole).toString());
    setMetaData(index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::MetaData)).data(Qt::EditRole).toString());
    setPreviewImage(index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::PreviewImage)).data(Qt::UserRole + 1).value<QImage>());
    setContributors(index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Contributors)).data(Qt::UserRole + 1).toStringList());
}

bool StartPageAction::isCompactView()
{
    return StartPageAction::compactView;
}

void StartPageAction::setCompactView(bool compactView)
{
    StartPageAction::compactView = compactView;
}
