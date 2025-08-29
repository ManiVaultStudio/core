// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "StandardItemModel.h"

#include "util/HeadsUpDisplayItem.h"

namespace mv {

/**
 * Heads-up display model class
 *
 * Contains information that is displayed in the heads-up display
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractHeadsUpDisplayModel : public StandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        Id,             /** Item global unique identifier */  
        Title,          /** Item title */
        Value,          /** Item value */
        Description,    /** Item description */

        Count
    };

    /** Base standard model item class for heads-up display item  */
    class CORE_EXPORT Item : public QStandardItem {
    public:

        /**
         * Construct with shared pointer to \p headsUpDisplayItem
         * @param headsUpDisplayItem Shared pointer to heads-up display item
         */
        Item(const util::HeadsUpDisplayItemSharedPtr& headsUpDisplayItem);

        /**
         * Get heads-up display item
         * return Shared pointer to heads-up display item
         */
        util::HeadsUpDisplayItemSharedPtr getHeadsupDisplayItem() const;

    private:
        util::HeadsUpDisplayItemSharedPtr   _headsUpDisplayItem;  /** Pointer to heads-up display item */
    };

    /** Item class for displaying the item ID */
    class CORE_EXPORT IdItem final : public Item {
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
                    return "ID";

                case Qt::ToolTipRole:
                    return "Globally unique identifier";
            }

            return {};
        }
    };

    /** Item class for displaying the title */
    class CORE_EXPORT TitleItem final : public Item {
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
	                return "Title";

	            case Qt::ToolTipRole:
	                return "Item title";
            }

            return {};
        }
    };

    /** Item class for displaying the value */
    class CORE_EXPORT ValueItem final : public Item {
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
	                return "Value";

	            case Qt::ToolTipRole:
	                return "Item value";
            }

            return {};
        }
    };

    /** Item class for displaying the description */
    class CORE_EXPORT DescriptionItem final : public Item {
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
	                return "Description";

	            case Qt::ToolTipRole:
	                return "Item description";
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
         * Construct with shared pointer to \p headsUpDisplayItem
         * @param headsUpDisplayItem Shared pointer to heads-up display item
         */
        Row(const util::HeadsUpDisplayItemSharedPtr& headsUpDisplayItem) :
    		QList<QStandardItem*>()
        {
            append(new IdItem(headsUpDisplayItem));
            append(new TitleItem(headsUpDisplayItem));
            append(new ValueItem(headsUpDisplayItem));
            append(new DescriptionItem(headsUpDisplayItem));
        }
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractHeadsUpDisplayModel(QObject* parent = nullptr);

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public: // Add/remove items

    /**
     * Add \p headsUpDisplayItem to the model
     * @param headsUpDisplayItem Heads-up display item to add
     */
    void addHeadsUpDisplayItem(const util::HeadsUpDisplayItemSharedPtr& headsUpDisplayItem);

    /**
     * Remove \p headsUpDisplayItem
     * @param headsUpDisplayItem Heads-up display item to remove
     */
    void removeHeadsUpDisplayItem(const util::HeadsUpDisplayItemSharedPtr& headsUpDisplayItem);

private: // Utility functions

    /**
     * Get model index from \p headsUpDisplayItem
     * @param headsUpDisplayItem Heads-up display item
     * @return Model index corresponding to the heads-up display item
     */
    QModelIndex indexFromHeadsUpDisplayItem(const util::HeadsUpDisplayItemSharedPtr& headsUpDisplayItem) const;
};

}
