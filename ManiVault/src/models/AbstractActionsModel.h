// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "CoreInterface.h"

#include "StandardItemModel.h"

#include "actions/WidgetAction.h"

#include <QList>
#include <QStandardItem>

namespace mv
{

/**
 * Abstract actions model class
 *
 * Abstract standard item model class for actions
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractActionsModel : public StandardItemModel
{
    Q_OBJECT

public:

    /** Action columns */
    enum class Column {
        Name,                       /** Name of the action */
        Location,                   /** Where the action is located in the user interface */
        ID,                         /** Globally unique identifier of the action */
        Type,                       /** Action type string */
        Scope,                      /** Scope of the action (whether the action is public or private) */
        ForceDisabled,              /** Whether the action is force disabled or not */
        ForceHidden,                /** Whether the action is force hidden or not */
        MayPublish,                 /** Whether the action may be published */
        MayConnect,                 /** Whether the action may connect to a public action */
        MayDisconnect,              /** Whether the action may disconnect from a public action */
        SortIndex,                  /** The sorting index of the action (its relative position in action groups) */
        Stretch,                    /** The stretch in action groups */
        ParentActionId,             /** The identifier of the parent action (if not a top-level action) */
        IsConnected,                /** Whether the action is connected or not */
        NumberOfConnectedActions,   /** Number of connected actions (in case the action is public) */
        PublicActionID,             /** The identifier of the public action with which the action is connected */
        IsRoot,                     /** If the action is at the root or not */
        IsLeaf,                     /** If the action is a leaf or not */

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
    class CORE_EXPORT HeaderItem : public QStandardItem {
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
    class CORE_EXPORT Item : public QStandardItem, public QObject {
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
    class CORE_EXPORT NameItem final : public Item {
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

    /** Standard model item class for displaying the action location */
    class CORE_EXPORT LocationItem final : public Item {
    public:

        /**
         * Construct with \p action
         * @param action Pointer to action to display item for
         */
        LocationItem(gui::WidgetAction* action);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the action identifier */
    class CORE_EXPORT IdItem final : public Item {
    public:

        /**
         * Construct with \p action
         * @param action Pointer to action to display item for
         */
        IdItem(gui::WidgetAction* action);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the action type */
    class CORE_EXPORT TypeItem final : public Item {
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
    class CORE_EXPORT ScopeItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Model item class for toggling action force disabled */
    class CORE_EXPORT ForceDisabledItem final : public Item {
    public:

        /**
         * Construct with \p action
         * @param action Pointer to action to display item for
         */
        ForceDisabledItem(gui::WidgetAction* action);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;
    };

    /** Model item class for toggling force hidden */
    class CORE_EXPORT ForceHiddenItem final : public Item {
    public:

        /**
         * Construct with \p action
         * @param action Pointer to action to display item for
         */
        ForceHiddenItem(gui::WidgetAction* action);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;
    };

    /** Model item class for toggling a permission flag */
    class CORE_EXPORT ConnectionPermissionItem final : public Item {
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
    class CORE_EXPORT SortIndexItem final : public Item {
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

    /** Model item class for action stretch */
    class CORE_EXPORT StretchItem final : public Item {
    public:

        /**
         * Construct with \p action
         * @param action Pointer to action to display item for
         */
        StretchItem(gui::WidgetAction* action);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;
    };

    /** Standard model item class for displaying the parent action identifier */
    class CORE_EXPORT ParentActionIdItem final : public Item {
    public:

        /**
         * Construct with \p action
         * @param action Pointer to action to display item for
         */
        ParentActionIdItem(gui::WidgetAction* action);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying whether the action is connected or not */
    class CORE_EXPORT IsConnectedItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the number of connected actions */
    class CORE_EXPORT NumberOfConnectedActionsItem final : public Item {
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
    class CORE_EXPORT PublicActionIdItem final : public Item {
    public:

        /**
         * Construct with \p action
         * @param action Pointer to action to display item for
         */
        PublicActionIdItem(gui::WidgetAction* action);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying whether the action is a root action */
    class CORE_EXPORT IsRootItem final : public Item {
    public:

        /** Use base action item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying whether the action is a leaf action */
    class CORE_EXPORT IsLeafItem final : public Item {
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
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct row with \p action
         * @param action Pointer to row action
         */
        Row(gui::WidgetAction* action) : QList<QStandardItem*>()
        {
            append(new NameItem(action));
            append(new LocationItem(action));
            append(new IdItem(action));
            append(new TypeItem(action));
            append(new ScopeItem(action));
            append(new ForceDisabledItem(action));
            append(new ForceHiddenItem(action));
            append(new ConnectionPermissionItem(action, gui::WidgetAction::ConnectionPermissionFlag::PublishViaGui));
            append(new ConnectionPermissionItem(action, gui::WidgetAction::ConnectionPermissionFlag::ConnectViaGui));
            append(new ConnectionPermissionItem(action, gui::WidgetAction::ConnectionPermissionFlag::DisconnectViaGui));
            append(new SortIndexItem(action));
            append(new StretchItem(action));
            append(new ParentActionIdItem(action));
            append(new IsConnectedItem(action));
            append(new NumberOfConnectedActionsItem(action));
            append(new PublicActionIdItem(action));
            append(new IsRootItem(action));
            append(new IsLeafItem(action));
        }
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
    gui::WidgetAction* getAction(const QModelIndex& index) const;

    /**
     * Get action for model \p rowIndex
     * @param rowIndex Row index to retrieve the action for
     * @return Pointer to action (nullptr if not found)
     */
    gui::WidgetAction* getAction(std::int32_t rowIndex) const;

    /**
     * Get action by \p name
     * @param name Name of the action
     * @return Pointer to action (nullptr if not found)
     */
    gui::WidgetAction* getAction(const QString& name) const;

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

protected: // Drag-and-drop behavior

    /** Get the supported drag actions */
    Qt::DropActions supportedDragActions() const override;

    /** Get the supported drop actions */
    Qt::DropActions supportedDropActions() const override;

    /**
     * Get mime types
     * @return Supported mime types
     */
    QStringList mimeTypes() const override;

    /**
     * Get mime data for model \p indexes
     * @return Pointer to mime data for \p indexes
     */
    QMimeData* mimeData(const QModelIndexList& indexes) const override;

    /**
     * Determines whether \p mimeData with \p action can be dropped on \p row and \p column with \p parent model index
     * @param mimeData Pointer to dropped mime data
     * @param dropAction Drop action
     * @param row Drop row
     * @param column Drop column
     * @param parent Drop parent index
     * @return Boolean whether the drop is permitted
     */
    bool canDropMimeData(const QMimeData* mimeData, Qt::DropAction dropAction, int row, int column, const QModelIndex& parent) const override;

    /**
     * Invoked when \p mimeData is dropped
     * @param mimeData Pointer to dropped mime data
     * @param dropAction Drop action
     * @param row Drop row
     * @param column Drop column
     * @param parent Drop parent index
     * @return Boolean determining whether the drop is performed
     */
    bool dropMimeData(const QMimeData* mimeData, Qt::DropAction dropAction, int row, int column, const QModelIndex& parent) override;

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
