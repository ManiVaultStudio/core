// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageActionsTreeModel.h"

#include "PageAction.h"

#include <QList>
#include <QStandardItem>

#ifdef _DEBUG
    //#define PAGE_ACTIONS_TREE_MODEL_VERBOSE
#endif

void PageActionsTreeModel::add(const PageActionSharedPtr& pageAction)
{
#ifdef PAGE_ACTIONS_TREE_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    /*QList<QStandardItem*> pageActionRow = {
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

    appendRow(pageActionRow);*/
}

void PageActionsTreeModel::remove(const PageActionSharedPtr& pageAction)
{

}
