// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageActionsModel.h"

#include "StartPageAction.h"

#include <QList>
#include <QStandardItem>

#ifdef _DEBUG
    //#define START_PAGE_ACTIONS_MODEL_VERBOSE
#endif

QMap<StartPageActionsModel::Column, QPair<QString, QString>> StartPageActionsModel::columns = QMap<Column, QPair<QString, QString>>({
    { Column::Icon, { "Icon", "Icon" }},
    { Column::Title, { "Title", "Title" }},
    { Column::Description, { "Description", "Description" }},
    { Column::Comments, { "Comments", "Comments" }},
    { Column::Tags, { "Tags", "Tags" }},
    { Column::MetaData, { "Meta Data", "Meta Data" }},
    { Column::PreviewImage, { "Preview", "Preview image" }},
    { Column::Tooltip, { "Tooltip", "Tooltip" }},
    { Column::Contributors, { "Contributors", "Contributors" }},
    { Column::ClickedCallback, { "Clicked Callback", "Callback which is called when the action is clicked" }},
    { Column::SummaryDelegate, { "Summary", "Delegate item with title and subtitle" }}
});

StartPageActionsModel::StartPageActionsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    for (const auto& column : columns.keys()) {
        auto columnItem = new QStandardItem(columns[column].first);
        
        columnItem->setToolTip(columns[column].second);

        setHorizontalHeaderItem(static_cast<int>(column), columnItem);
    }
}

void StartPageActionsModel::add(const StartPageAction& startPageAction)
{
#ifdef START_PAGE_ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QList<QStandardItem*> startPageActionRow = {
        new QStandardItem(),
        new QStandardItem(startPageAction.getTitle()),
        new QStandardItem(startPageAction.getDescription()),
        new QStandardItem(startPageAction.getComments()),
        new QStandardItem(),
        new QStandardItem(startPageAction.getSubtitle()),
        new QStandardItem(startPageAction.getMetaData()),
        new QStandardItem(),
        new QStandardItem(startPageAction.getTooltip()),
        new QStandardItem(),
        new QStandardItem(),
        new QStandardItem()
    };

    startPageActionRow[static_cast<int>(Column::Icon)]->setData(QVariant::fromValue(startPageAction.getIcon()));
    startPageActionRow[static_cast<int>(Column::Tags)]->setData(QVariant::fromValue(startPageAction.getTags()), Qt::EditRole);
    startPageActionRow[static_cast<int>(Column::PreviewImage)]->setData(QVariant::fromValue(startPageAction.getPreviewImage()));
    startPageActionRow[static_cast<int>(Column::Contributors)]->setData(QVariant::fromValue(startPageAction.getContributors()));
    startPageActionRow[static_cast<int>(Column::ClickedCallback)]->setData(QVariant::fromValue(startPageAction.getClickedCallback()));

    for (auto item : startPageActionRow)
        item->setEditable(false);

    appendRow(startPageActionRow);
}

void StartPageActionsModel::reset()
{
    removeRows(0, rowCount());

    emit layoutChanged();
}
