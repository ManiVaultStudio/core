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
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractWorkflowMessagesModel : public StandardItemModel
{
public:

    /** Model columns */
    enum class Column {
        Level,
        Text,
        Emitter,
        Location,
        Details,
        TimeStamp,

        Count
    };

    /** Base standard model item class for script item  */
    class CORE_EXPORT Item : public QStandardItem {
    public:

        /**
         * Construct with reference to \p message
         * @param message The workflow message
         */
        Item(util::WorkflowMessage message);

        /**
         * Get workflow message
         * @return Reference to workflow message
         */
        const util::WorkflowMessage& getWorkflowMessage() const;

        /**
         * Get tooltip for item
         * @return Tooltip string
         */
        QString getTooltip() const;

    private:
        util::WorkflowMessage   _workflowMessage;  /** Pointer to script */
    };

    /** Item class for displaying the workflow message type */
    class CORE_EXPORT LevelItem final : public Item {
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
                    return "Level";

                case Qt::ToolTipRole:
                    return "Workflow message level";
            }

            return {};
        }
    };

    /** Item class for displaying the workflow message text */
    class CORE_EXPORT TextItem final : public Item {
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
                return "Text";

            case Qt::ToolTipRole:
                return "Workflow message text";
            }

            return {};
        }
    };

    /** Item class for displaying the workflow message emitter */
    class CORE_EXPORT EmitterItem final : public Item {
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
                    return "Emitter";

                case Qt::ToolTipRole:
                    return "Workflow message emitter";
            }

            return {};
        }
    };

    /** Item class for displaying the workflow message location */
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
	                return "Workflow message location";
            }

            return {};
        }
    };

    /** Item class for displaying the workflow message details */
    class CORE_EXPORT DetailsItem final : public Item {
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
	                return "Details";

	            case Qt::ToolTipRole:
	                return "Workflow message details";
            }

            return {};
        }
    };

    /** Item class for displaying the workflow message timestamp */
    class CORE_EXPORT TimeStampItem final : public Item {
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
	                return "Time";

	            case Qt::ToolTipRole:
	                return "Workflow message timestamp";
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
         * Construct with reference to \p message
         * @param message Reference to the workflow message
         */
        Row(const util::WorkflowMessage& message) : QList<QStandardItem*>()
        {
            append(new LevelItem(message));
            append(new EmitterItem(message));
            append(new LocationItem(message));
            append(new TextItem(message));
            append(new DetailsItem(message));
            append(new TimeStampItem(message));
        }
    };

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractWorkflowMessagesModel(QObject* parent = nullptr);

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
