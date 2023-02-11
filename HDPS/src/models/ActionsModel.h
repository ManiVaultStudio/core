#pragma once

#include "actions/WidgetAction.h"

#include <QStandardItemModel>
#include <QPointer>

namespace hdps
{

/**
 * Actions model class
 *
 * Standard item model class for actions
 *
 * @author Thomas Kroes
 */
class ActionsModel final : public QStandardItemModel
{
    Q_OBJECT

public:

    /** Action columns */
    enum class Column {
        Name,       /** Name of the action */
        ID,         /** Globally unique identifier of the action */
        Type,       /** Action type string */
        Scope,      /** Scope of the action (whether the action is public or private) */
        Connected,  /** Whether the action is connected or not */

        Count
    };

    /** Column name and tooltip */
    static QMap<Column, QPair<QString, QString>> columnInfo;

protected:

    /**
     * Item class
     *
     * Standard item class for displaying actions model column
     *
     * @author Thomas Kroes
     */
    class Item final : public QStandardItem, public QObject
    {
    public:

        /**
         * Construct item from \p actionsModel, \p widgetAction and \p column
         * @param actionsModel Pointer to actions model
         * @param widgetAction Pointer to source widget action
         * @param column Column to display
         */
        Item(ActionsModel* actionsModel, gui::WidgetAction* widgetAction, const Column& column);

        /** Destructor  */
        ~Item();

        /**
         * Get pointer to source widget action
         * @return Pointer to source widget action
         */
        gui::WidgetAction* getAction();

    private:
        void updateNameColumn();      /** Update item in case of name column */
        void updateIdColumn();        /** Update item in case of identifier column */
        void updateTypeColumn();      /** Update item in case of type column */
        void updateScopeColumn();     /** Update item in case of scope column */
        void updateConnectedColumn(); /** Update item in case of connected column */

    private:
        ActionsModel*               _actionsModel;  /** Pointer to parent actions model */
        QPointer<gui::WidgetAction> _widgetAction;  /** Pointer to source widget action */
        Column                      _column;        /** Column to display */
    };

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    ActionsModel(QObject* parent = nullptr);

    /**
     * Add \p action to the model
     * @param action Pointer to action to add
     */
    void addAction(gui::WidgetAction* action);

    /**
     * Remove \p action from the model
     * @param action Pointer to action to remove
     */
    void removeAction(gui::WidgetAction* action);

    /**
     * Get all actions in the manager
     * @return List of all actions in the manager
     */
    const gui::WidgetActions& getActions() const;

    /**
     * Get set of action types
     * @return List action types
     */
    const QStringList getActionTypes() const;

    /**
     * Get action for model \p index
     * @param index Model index to retrieve the action for
     * @return Pointer to action (nullptr if not found)
     */
    gui::WidgetAction* getAction(const QModelIndex& index);

    /**
     * Get action for model \p rowIndex
     * @param rowIndex Row index to retrieve the action for
     * @return Pointer to action (nullptr if not found)
     */
    gui::WidgetAction* getAction(std::int32_t rowIndex);

protected:

    /**
     * Add \p actionType
     * @param actionType Action type to add
     */
    void addActionType(const QString& actionType);

    /**
     * Remove \p actionType
     * @param actionType Action type to remove
     */
    void removeActionType(const QString& actionType);

signals:

    /**
     * Signals that the \p actionTypes changed
     * @param actionTypes Action types
     */
    void actionTypesChanged(const QStringList& actionTypes);

private:
    QMap<QString, std::int32_t>   _actionTypes;     /** Number of rows per action type */

    friend class Item;
};

}
