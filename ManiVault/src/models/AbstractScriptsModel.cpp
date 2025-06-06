// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractScriptsModel.h"

using namespace mv;

#ifdef _DEBUG
    #define SCRIPTS_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

AbstractScriptsModel::Item::Item(const QString& type, Script* script) :
    _script(script)
{
    setEditable(false);
}

Script* AbstractScriptsModel::Item::getScript() const
{
    return _script;
}

QVariant AbstractScriptsModel::TypeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
		case Qt::EditRole:
            return static_cast<std::int32_t>(getScript()->getType());

		case Qt::DisplayRole:
            return Script::getTypeName(getScript()->getType());

        case Qt::ToolTipRole:
            return QString("%1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractScriptsModel::LanguageItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
	    case Qt::EditRole:
	        return static_cast<std::int32_t>(getScript()->getLanguage());

	    case Qt::DisplayRole:
	        return Script::getLanguageName(getScript()->getLanguage());

	    case Qt::ToolTipRole:
	        return QString("%1").arg(data(Qt::DisplayRole).toString());

	    case Qt::DecorationRole:
        {
            switch (getScript()->getLanguage()) {
	            case Script::Language::Python:
	                return StyledIcon("python");

	            case Script::Language::R:
                    return StyledIcon("r");

	            default:
	                return QIcon();
            }
		}

	    default:
	        break;
    }

    return Item::data(role);
}

QVariant AbstractScriptsModel::LocationItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
	    case Qt::EditRole:
	        return getScript()->getLocation();

	    case Qt::DisplayRole:
	        return getScript()->getLocation().toString();

	    case Qt::ToolTipRole:
	        return QString("%1").arg(data(Qt::DisplayRole).toString());

	    default:
	        break;
    }

    return Item::data(role);
}

AbstractScriptsModel::AbstractScriptsModel(QObject* parent) :
    StandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

QVariant AbstractScriptsModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
	    case Column::Type:
	        return TypeItem::headerData(orientation, role);

	    case Column::Language:
	        return LanguageItem::headerData(orientation, role);

	    case Column::Location:
	        return LocationItem::headerData(orientation, role);

	    default:
	        break;
    }

    return {};
}

}