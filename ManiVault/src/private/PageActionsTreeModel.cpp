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

    try {
        if (!pageAction)
            throw std::runtime_error("Page action is nullptr");

        if (pageAction->getParentTitle().isEmpty()) {
            appendRow(new Item(pageAction));
        } else {
            const auto matches = match(index(0, static_cast<std::int32_t>(Column::Title)), Qt::DisplayRole, pageAction->getParentTitle(), 1, Qt::MatchExactly | Qt::MatchRecursive);

            if (matches.isEmpty())
                throw std::runtime_error(QString("Parent page action %1 not found").arg(pageAction->getParentTitle()).toStdString());

            itemFromIndex(matches.first())->appendRow(new Item(pageAction));
        }
    }
    catch (std::exception& e)
    {
        qCritical() << QString("Unable to add %1 to the tree model:").arg(pageAction->getTitle()) << e.what();
    }
    catch (...)
    {
        qCritical() << QString("Unable to add %1 to the tree model due to an unhandled exception").arg(pageAction->getTitle());
    }
}

void PageActionsTreeModel::remove(const PageActionSharedPtr& pageAction)
{

}
