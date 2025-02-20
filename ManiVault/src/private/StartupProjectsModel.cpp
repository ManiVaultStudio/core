// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartupProjectsModel.h"

#include <ProjectMetaAction.h>

#include <util/StyledIcon.h>

#include <QString>

#ifdef _DEBUG
    #define STARTUP_PROJECTS_MODEL_VERBOSE
#endif

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

StartupProjectsModel::StartupProjectsModel(QObject* parent /*= nullptr*/) :
    StandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

QVariant StartupProjectsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (static_cast<Column>(section))
    {
        case Column::FileName:
            return FileNameItem::headerData(orientation, role);

        case Column::Title:
            return TitleItem::headerData(orientation, role);

        case Column::Description:
            return DescriptionItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

void StartupProjectsModel::initialize(const QVector<QPair<QSharedPointer<mv::ProjectMetaAction>, QString>>& startupProjectsMetaActions)
{
    for (const auto& startupProjectMetaAction : startupProjectsMetaActions)
    {
        auto icon           = StyledIcon(startupProjectMetaAction.first->getApplicationIconAction().getIconPickerAction().getIcon());
        auto fileName       = QFileInfo(startupProjectMetaAction.second).baseName();
        auto title          = startupProjectMetaAction.first->getTitleAction().getString();
        auto description    = startupProjectMetaAction.first->getDescriptionAction().getString();

        if (title.isEmpty())
            title = "NA";

        if (description.isEmpty())
            description = "NA";

        appendRow(Row(icon.isNull() ? createIcon(QPixmap(":/Icons/AppIcon256")) : icon, fileName, title, description));
    }
}
