// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractPluginsModel.h"

#include "Plugin.h"

using namespace mv;

#ifdef _DEBUG
    #define ABSTRACT_PLUGINS_MODEL_VERBOSE
#endif

Q_DECLARE_METATYPE(mv::plugin::Plugin*);

namespace mv {

AbstractPluginsModel::Row::Row(const QString& name, const QString& category, const QString& id, const QIcon& icon /*= QIcon()*/)
{
    auto nameItem = new NameItem(name);

    if (!icon.isNull())
        nameItem->setIcon(icon);

    append(nameItem);
    append(new CategoryItem(category));
    append(new IdItem(id));
}

AbstractPluginsModel::AbstractPluginsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
}

QVariant AbstractPluginsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Name:
            return NameItem::headerData(orientation, role);

        case Column::Category:
            return CategoryItem::headerData(orientation, role);

        case Column::ID:
            return IdItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

}