// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "StandardItemModel.h"

#include <QList>
#include <QStandardItem>

namespace mv {

/**
 * Scripts model class
 *
 * Base standard item model for scripts
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractScriptsModel : public StandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        Type,       /** Script type item */
        Language,   /** Script language item */
        Location,   /** Script location item */

        Count
    };

    /** Base standard model item class for script item  */
    class CORE_EXPORT Item : public QStandardItem {
    public:

        /**
         * Construct with \p type and pointer to \p script
         * @param type View type
         * @param script Pointer to script
         */
        Item(const QString& type, util::Script* script);

        /**
         * Get script
         * return Pointer to script
         */
        util::Script* getScript() const;

    private:
        util::Script*   _script;  /** Pointer to script */
    };

    /** Item class for displaying the script type */
    class CORE_EXPORT TypeItem final : public Item {
    public:

        /** No need for specialized constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get header data for \p orientation and \p role
         * @param orientation Horizontal/vertical
         * @param role Data role
         * @return Header data
         */
        static QVariant headerData(Qt::Orientation orientation, int role) {
            switch (role) {
                case Qt::DisplayRole:
                case Qt::EditRole:
                    return "Type";

                case Qt::ToolTipRole:
                    return "Script type";
            }

            return {};
        }
    };

    /** Item class for displaying the script language */
    class CORE_EXPORT LanguageItem final : public Item {
    public:

        /** No need for specialized constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get header data for \p orientation and \p role
         * @param orientation Horizontal/vertical
         * @param role Data role
         * @return Header data
         */
        static QVariant headerData(Qt::Orientation orientation, int role) {
            switch (role) {
	            case Qt::DisplayRole:
	            case Qt::EditRole:
	                return "Language";

	            case Qt::ToolTipRole:
	                return "Script language";
            }

            return {};
        }
    };

    /** Item class for displaying the script location */
    class CORE_EXPORT LocationItem final : public Item {
    public:

        /** No need for specialized constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get header data for \p orientation and \p role
         * @param orientation Horizontal/vertical
         * @param role Data role
         * @return Header data
         */
        static QVariant headerData(Qt::Orientation orientation, int role) {
            switch (role) {
	            case Qt::DisplayRole:
	            case Qt::EditRole:
	                return "Location";

	            case Qt::ToolTipRole:
	                return "Script location";
            }

            return {};
        }
    };

protected:

    /** Convenience class for combining items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with \p type and pointer to \p script
         * @param type View type
         * @param script Pointer to the script
         */
        Row(const QString& type, util::Script* script) : QList<QStandardItem*>()
        {
            append(new TypeItem(type, script));
            append(new LanguageItem(type, script));
            append(new LocationItem(type, script));
        }
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractScriptsModel(QObject* parent = nullptr);

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

}