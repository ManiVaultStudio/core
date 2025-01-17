// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageActionsModel.h"

#include "PageAction.h"

#include <QList>
#include <QStandardItem>

#ifdef _DEBUG
    //#define PAGE_ACTIONS_MODEL_VERBOSE
#endif

QMap<PageActionsModel::Column, QPair<QString, QString>> PageActionsModel::columns = QMap<Column, QPair<QString, QString>>({
    { Column::Icon, { "Icon", "Icon" }},
    { Column::Title, { "Title", "Title" }},
    { Column::Description, { "Description", "Description" }},
    { Column::Comments, { "Comments", "Comments" }},
    { Column::Tags, { "Tags", "Tags" }},
    { Column::MetaData, { "Meta Data", "Meta Data" }},
    { Column::PreviewImage, { "Preview", "Preview image" }},
    { Column::Tooltip, { "Tooltip", "Tooltip" }},
    { Column::DownloadUrls, { "Download URLs", "Download URLs" }},
    { Column::Contributors, { "Contributors", "Contributors" }},
    { Column::ClickedCallback, { "Clicked Callback", "Callback which is called when the action is clicked" }},
    { Column::SummaryDelegate, { "Summary", "Delegate item with title and subtitle" }}
});

PageActionsModel::PageActionsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    for (const auto& column : columns.keys()) {
        auto columnItem = new QStandardItem(columns[column].first);
        
        columnItem->setToolTip(columns[column].second);

        setHorizontalHeaderItem(static_cast<int>(column), columnItem);
    }
}

void PageActionsModel::add(const PageAction& pageAction)
{
#ifdef PAGE_ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QList<QStandardItem*> pageActionRow = {
        new QStandardItem(),
        new QStandardItem(pageAction.getTitle()),
        new QStandardItem(pageAction.getDescription()),
        new QStandardItem(pageAction.getComments()),
        new QStandardItem(),
        new QStandardItem(pageAction.getSubtitle()),
        new QStandardItem(pageAction.getMetaData()),
        new QStandardItem(),
        new QStandardItem(pageAction.getTooltip()),
        new QStandardItem(pageAction.getDownloadUrls().join(", ")),
        new QStandardItem(),
        new QStandardItem(),
        new QStandardItem()
    };

    pageActionRow[static_cast<int>(Column::Icon)]->setData(QVariant::fromValue(pageAction.getIcon()));
    pageActionRow[static_cast<int>(Column::Tags)]->setData(QVariant::fromValue(pageAction.getTags()), Qt::EditRole);
    pageActionRow[static_cast<int>(Column::PreviewImage)]->setData(QVariant::fromValue(pageAction.getPreviewImage()));
    pageActionRow[static_cast<int>(Column::Contributors)]->setData(QVariant::fromValue(pageAction.getContributors()));
    pageActionRow[static_cast<int>(Column::DownloadUrls)]->setData(QVariant::fromValue(pageAction.getDownloadUrls()), Qt::EditRole);
    pageActionRow[static_cast<int>(Column::ClickedCallback)]->setData(QVariant::fromValue(pageAction.getClickedCallback()));

    for (auto item : pageActionRow)
        item->setEditable(false);

    appendRow(pageActionRow);
}

void PageActionsModel::reset()
{
    removeRows(0, rowCount());

    emit layoutChanged();
}
