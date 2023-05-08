#pragma once

#include "CoreInterface.h"

#include "actions/WidgetAction.h"

#include <QStandardItemModel>

namespace hdps
{

/**
 * Abstract actions model class
 *
 * Abstract standard item model class for actions
 *
 * @author Thomas Kroes
 */
class AbstractActionsModel : public QStandardItemModel
{
    Q_OBJECT

public:

    /** Action columns */
    enum class Column {
        Name,                   /** Name of the action */
        Location,               /** Where the action is located in the user interface */
        ID,                     /** Globally unique identifier of the action */
        Type,                   /** Action type string */
        Scope,                  /** Scope of the action (whether the action is public or private) */
        Visible,                /** Whether the action is visible in the GUI */
        MayPublish,             /** Whether the action may be published */
        MayConnect,             /** Whether the action may connect to a public action */
        MayDisconnect,          /** Whether the action may disconnect from a public action */
        SortIndex,              /** The sorting index of the action (its relative position in action groups) */
        ParentActionId,         /** The identifier of the parent action (if not a top-level action) */
        IsConnected,            /** Whether the action is connected or not */
        PublicActionID,         /** The identifier of the public action with which the action is connected */
        IsRootPublicAction,     /** If the action is public whether it is at the root or not */

        Count
    };

    /** Header strings for several data roles */
    struct ColumHeaderInfo {
        QString     _display;   /** Header string for display role */
        QString     _edit;      /** Header string for edit role */
        QString     _tooltip;   /** Header string for tooltip role */
    };

    /** Column name and tooltip */
    static QMap<Column, ColumHeaderInfo> columnInfo;

protected:

    /** Header standard model item class */
    class HeaderItem : public QStandardItem {
    public:

        /**
         * Construct with \p columHeaderInfo
         * @param columHeaderInfo Column header info
         */
        HeaderItem(const ColumHeaderInfo& columHeaderInfo);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

    private:
        ColumHeaderInfo     _columHeaderInfo;   /** Column header info */
    };

public:

    /** Base standard model item class for widget action */
    class Item : public QStandardItem, public QObject {
    public:

        /**
         * Construct with \p action
         * @param action Pointer to action to display item for
         * @param editable Whether the model item is editable or not
         */
        Item(gui::WidgetAction* action, bool editable = false);

        /**
         * Get action
         * return Pointer to action to display item for
         */
        QPointer<gui::WidgetAction> getAction() const;

    private:
        QPointer<gui::WidgetAction>      _action;        /** Pointer to action to display item for */
    };

protected:

    /** Standard model item class for displaying the action name */
    class NameItem final : public Item {
    public:

        /**
         * Construct with \p action
         * @param action Pointer to action to display item for
         */
        NameItem(gui::WidgetAction* action);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;
    };

    /** Standard model item class for displaying the action path */
    class PathItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the action identifier */
    class IdItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the action type */
    class TypeItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the action scope */
    class ScopeItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Model item class for toggling action visibility */
    class VisibilityItem final : public Item {
    public:

        /**
         * Construct with \p action
         * @param action Pointer to action to display item for
         */
        VisibilityItem(gui::WidgetAction* action);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;
    };

    /** Model item class for toggling a permission flag */
    class ConnectionPermissionItem final : public Item {
    public:

        /**
         * Construct with \p action and \p connectionPermissionFlag
         * @param action Pointer to action to display item for
         * @param connectionPermissionFlag Connection permission flag to display
         */
        ConnectionPermissionItem(gui::WidgetAction* action, const gui::WidgetAction::ConnectionPermissionFlag& connectionPermissionFlag);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;

    private:
        gui::WidgetAction::ConnectionPermissionFlag  _connectionPermissionFlag;      /** Connection permission flag to toggle */
    };

    /** Model item class for action sort index */
    class SortIndexItem final : public Item {
    public:

        /**
         * Construct with \p action
         * @param action Pointer to action to display item for
         */
        SortIndexItem(gui::WidgetAction* action);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;
    };

    /** Standard model item class for displaying the parent action identifier */
    class ParentActionIdItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying whether the action is connected or not */
    class IsConnectedItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the public action identifier */
    class PublicActionIdItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying whether the action is a public root action */
    class IsRootPublicActionItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Convenience class for combining action items in a row */
    class Row : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct row with \p action
         * @param action Pointer to row action
         */
        Row(gui::WidgetAction* action);
    };

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractActionsModel(QObject* parent = nullptr);  

    /** Initializes the model from the current state of the actions manager */
    virtual void initialize() = 0;

    /**
     * Get flags for item with \p index
     * @return Item flags
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

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

    /**
     * Get model index for \p action and column
     * @param action Pointer to action to retrieve the model index for
     * @param column Column to retrieve the action index for
     * @return Found model index
     */
    virtual QModelIndex getActionIndex(const gui::WidgetAction* action, const Column& column = Column::Name) const final;

    /**
     * Get action item for \p action
     * @param action Pointer to action to retrieve the model item for
     * @return Pointer to standard item (nullptr if not found)
     */
    virtual QStandardItem* getActionItem(const gui::WidgetAction* action) const final;
    
protected:

    /**
     * Helper method which is invoked when \p action is added to the actions manager
     * @param action Pointer to action that was added
     */
    virtual void actionAddedToManager(gui::WidgetAction* action) {};

    /**
     * Helper method which is invoked when \p action is about to be removed from the actions manager
     * @param action Pointer to action that was removed
     */
    virtual void actionAboutToBeRemovedFromManager(gui::WidgetAction* action) {};

    /**
     * Helper method which is invoked when \p publicAction is added to the actions manager
     * @param publicAction Pointer to public action that was added
     */
    virtual void publicActionAddedToManager(gui::WidgetAction* publicAction) {};

    /**
     * Helper method which is invoked when \p publicAction is about to be removed from the actions manager
     * @param publicAction Pointer to public action to remove
     */
    virtual void publicActionAboutToBeRemovedFromManager(gui::WidgetAction* publicAction) {};

    friend class ActionsFilterModel;
    friend class Item;
};

}
