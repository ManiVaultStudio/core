#pragma once

#include <util/Logger.h>

#include <actions/ToggleAction.h>

#include <QAbstractItemModel>

#include <deque>
#include <cstdint>

namespace hdps::util {
    struct MessageRecord;
}

/**
 * Logging model
 *
 * Model item class for log items
 *
 * @author Thomas Kroes
 */
class LoggingModel final : public QAbstractItemModel
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

    static QMap<Column, QString> columnNames;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    LoggingModel(QObject* parent = nullptr);

    /** No need for copy constructor */
    LoggingModel(const LoggingModel&) = delete;
    
    /** No need for destructor */
    ~LoggingModel() override = default;

    /** No need for assignment operator */
    LoggingModel& operator=(const LoggingModel&) = delete;

    /** Synchronizes the model with the log records from the core logger */
    void synchronizeLogRecords();

public: // Action getters

    hdps::gui::ToggleAction& getWordWrapAction() { return _wordWrapAction; }

private:

    /**
     * Get the number of rows for \p parent
     * @return Number of children for \parent
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Get the number of columns for \p parent
     * @return Number of columns for \p parent
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Get model index for \p row and \p column and \p parent
     * @return Model index for \p row and \p column and \p parent
     */
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Get parent model index of \p index
     * @param index Index to get the parent for
     * @return Parent model index
     */
    QModelIndex parent(const QModelIndex& index) const override;

    /**
     * Get data for \p index and \p role
     * @return Variant representation of the data
     */
    QVariant data(const QModelIndex &, int) const override;

    /**
     * Get header data for \p index, \p orientation and \p role
     * @return Variant representation of the data
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    /**
     * Get model item flags for \p index
     * @param index Model index
     * @return Model item flags
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    std::deque<const hdps::util::MessageRecord*>    _messageRecords;    /** Logged message records */
    hdps::gui::ToggleAction                         _wordWrapAction;    /** Action for toggling word wrap */
};
