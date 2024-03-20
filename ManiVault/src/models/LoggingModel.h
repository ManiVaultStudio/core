// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/Logger.h"

#include "actions/ToggleAction.h"

#include <QList>
#include <QStandardItem>
#include <QStandardItemModel>

#include <deque>
#include <cstdint>

namespace mv {

/**
 * Logging model
 *
 * Model item class for log items
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT LoggingModel final : public QStandardItemModel
{
public:

    /** Columns */
    enum class Column
    {
        Number,
        Type,
        Message,
        FileAndLine,
        Function,
        Category,

        Count
    };

public:

    /** Base standard model item class for message record */
    class CORE_EXPORT Item : public QStandardItem, public QObject {
    public:

        /**
         * Construct with \p messageRecord
         * @param loggingModel Reference to owning logging model
         * @param messageRecord Reference to message record
         */
        Item(LoggingModel& loggingModel, const util::MessageRecord& messageRecord);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get logging model
         * @return Reference to owning logging model
         */
        LoggingModel& getLoggingModel();

        /**
         * Get logging model
         * @return Reference to owning logging model
         */
        const LoggingModel& getLoggingModel() const;

        /**
         * Get message record
         * @return Reference to message record to display item for
         */
        const util::MessageRecord& getMessageRecord() const;

    private:
        LoggingModel&                   _loggingModel;      /** Reference to owning logging model */
        const util::MessageRecord&      _messageRecord;     /** Reference to message record to display item for */
    };

    /** Standard model item class for interacting with the message number */
    class CORE_EXPORT NumberItem final : public Item {
    public:

        /** No need to override base constructor */
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
                    return "Number";

                case Qt::ToolTipRole:
                    return "Index of log message record";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the message type */
    class CORE_EXPORT TypeItem final : public Item {
    public:

        /** No need to override base constructor */
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
                    return "Type of log message record";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the message type */
    class CORE_EXPORT MessageItem final : public Item {
    public:

        /**
         * Construct with \p messageRecord
         * @param loggingModel Reference to owning logging model
         * @param messageRecord Reference to message record
         */
        MessageItem(LoggingModel& loggingModel, const util::MessageRecord& messageRecord);

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
                    return "Message";

                case Qt::ToolTipRole:
                    return "Message";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the message file and line */
    class CORE_EXPORT FileAndLineItem final : public Item {
    public:

        /** No need to override base constructor */
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
                    return "File and line";

                case Qt::ToolTipRole:
                    return "Message file and line";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the message function */
    class CORE_EXPORT FunctionItem final : public Item {
    public:

        /** No need to override base constructor */
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
                    return "Function";

                case Qt::ToolTipRole:
                    return "Function";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the message category */
    class CORE_EXPORT CategoryItem final : public Item {
    public:

        /** No need to override base constructor */
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
                    return "Category";

                case Qt::ToolTipRole:
                    return "Category";
            }

            return {};
        }
    };

protected:

    /** Convenience class for combining message record items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with reference to owning \p loggingModel and \p messageRecord
         * @param loggingModel Reference to owning logging model
         * @param MessageRecord Reference to message record
         */
        Row(LoggingModel& loggingModel, const util::MessageRecord& messageRecord) : QList<QStandardItem*>()
        {
            append(new NumberItem(loggingModel, messageRecord));
            append(new TypeItem(loggingModel, messageRecord));
            append(new MessageItem(loggingModel, messageRecord));
            append(new FileAndLineItem(loggingModel, messageRecord));
            append(new FunctionItem(loggingModel, messageRecord));
            append(new CategoryItem(loggingModel, messageRecord));
        }

    };

public:

    /**
     * Construct from \p parent
     * @param parent Pointer to parent object
     */
    LoggingModel(QObject* parent = nullptr);

    /** No need for copy constructor */
    LoggingModel(const LoggingModel&) = delete;
    
    /** No need for destructor */
    ~LoggingModel() override = default;

    /** No need for assignment operator */
    LoggingModel& operator=(const LoggingModel&) = delete;

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public: // Action getters

    gui::ToggleAction& getWordWrapAction() { return _wordWrapAction; }
    
    const gui::ToggleAction& getWordWrapAction() const { return _wordWrapAction; }

private:

    /** Synchronizes the model with the log records from the core logger */
    void populateFromLogger();

private:
    std::deque<const mv::util::MessageRecord*>  _messageRecords;            /** Logged message records */
    mv::gui::ToggleAction                       _wordWrapAction;            /** Action for toggling word wrap */
    QMetaObject::Connection                     _idleUpdateConnection;      /** Update the logging widget when idle */
};

}
