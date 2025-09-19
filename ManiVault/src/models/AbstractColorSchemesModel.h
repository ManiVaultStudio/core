// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "StandardItemModel.h"

#include "util/ColorScheme.h"

#include <QList>
#include <QStandardItem>

namespace mv {

/**
 * Color schemes model class
 *
 * Base standard item model for color schemes (application palette)
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractColorSchemesModel : public StandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        Name,           /** Color scheme name */
        Description,    /** Color scheme description */
        Palette,        /** Color palette */
        Mode,           /** Whether it is a built-in color scheme, or user-added */

        Count
    };

    /** Base standard model item class for the color scheme item */
    class CORE_EXPORT Item : public QStandardItem {
    public:

        /**
         * Construct with reference to the \p colorScheme
         * @param colorScheme Reference to the color scheme
         */
        Item(util::ColorScheme& colorScheme);

        /**
         * Get color scheme
         * @return Reference to the color scheme
         */
        util::ColorScheme& getColorScheme();

        /**
         * Get color scheme
         * @return Reference to the color scheme
         */
        const util::ColorScheme& getColorScheme() const;

    private:
        util::ColorScheme   _colorScheme;   /** Color scheme */
    };

    /** Item class for displaying the color scheme name */
    class CORE_EXPORT NameItem final : public Item {
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
                    return "Name";

                case Qt::ToolTipRole:
                    return "Color scheme name";

                default:
                    break;
            }

            return {};
        }
    };

    /** Item class for displaying the color scheme description version */
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
                    return "Color scheme description";

                default:
                    break;
            }

            return {};
        }
    };

    /** Item class for displaying the color scheme palette */
    class CORE_EXPORT PaletteItem final : public Item {
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
                    return "Palette";

                case Qt::ToolTipRole:
                    return "Color scheme palette";

                default:
                    break;
            }

            return {};
        }
    };

    /** Item class for displaying the color scheme mode */
    class CORE_EXPORT ModeItem final : public Item {
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
                    return "mode";

                case Qt::ToolTipRole:
                    return "Color scheme mode";

                default:
                    break;
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
         * Construct with reference to \p colorScheme
         * @param colorScheme Reference to colorScheme
         */
        Row(util::ColorScheme& colorScheme) : QList<QStandardItem*>()
        {
            append(new NameItem(colorScheme));
            append(new DescriptionItem(colorScheme));
            append(new PaletteItem(colorScheme));
            append(new ModeItem(colorScheme));
        }
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractColorSchemesModel(QObject* parent = nullptr);

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