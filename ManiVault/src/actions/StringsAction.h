// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include "actions/HorizontalGroupAction.h"

#include "widgets/HierarchyWidget.h"

#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

namespace mv::gui {

/**
 * Strings widget action class
 *
 * Stores a string list and creates widgets to interact with it (add/remove/rename)
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT StringsAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        MayEdit             = 0x00001,              /** Strings may be added and removed from the user interface */
        ListView            = 0x00002,              /** Widget includes a list view */
        EditableListView    = ListView | MayEdit,   /** Widget includes a list view with which strings may be added and removed from the user interface */

        Default = EditableListView
    };

public:

    /** Widget class for strings action */
    class CORE_EXPORT ListWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param stringsAction Pointer to strings action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        ListWidget(QWidget* parent, StringsAction* stringsAction, const std::int32_t& widgetFlags);

    private:
        QStandardItemModel      _model;             /** Strings model */
        QSortFilterProxyModel   _filterModel;       /** Strings filter model */
        HierarchyWidget         _hierarchyWidget;   /** Hierarchy widget for show the strings */

        friend class StringsAction;
    };

protected:

    /**
     * Get widget representation of the strings action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param strings Strings
     */
    Q_INVOKABLE explicit StringsAction(QObject* parent, const QString& title, const QStringList& strings = QStringList());

    /**
     * Get string category
     * @return String category
     */
    QString getCategory() const;

    /**
     * Set string category
     * @param category Category
     */
    void setCategory(const QString& category);

    /**
     * Get strings
     * @return Strings as string list
     */
    QStringList getStrings() const;

    /**
     * Set strings
     * @param strings Strings
     */
    void setStrings(const QStringList& strings);

    /**
     * Add string
     * @param string String to add
     */
    void addString(const QString& string);

    /**
     * Remove string
     * @param string String to remove
     */
    void removeString(const QString& string);

    /**
     * Remove strings
     * @param strings Strings to remove
     */
    void removeStrings(const QStringList& strings);

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    StringAction& getNameAction() { return _nameAction; }
    TriggerAction& getAddAction() { return _addAction; }
    TriggerAction& getRemoveAction() { return _removeAction; }
    HorizontalGroupAction& getToolbarAction() { return _toolbarAction; }

signals:

    /**
     * Signals that the strings changed
     * @param strings Updated strings
     */
    void stringsChanged(const QStringList& strings);

    /**
     * Signals that \p strings were added
     * @param strings Added strings
     */
    void stringsAdded(const QStringList& strings);

    /**
     * Signals that \p strings were removed
     * @param strings Removed strings
     */
    void stringsRemoved(const QStringList& strings);

protected:
    QString                 _category;          /** Type of string */
    QStringList             _strings;           /** Current strings */
    HorizontalGroupAction   _toolbarAction;     /** Toolbar action */
    StringAction            _nameAction;        /** String name action */
    TriggerAction           _addAction;         /** Add string action */
    TriggerAction           _removeAction;      /** Remove string action */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::StringsAction)

inline const auto stringsActionMetaTypeId = qRegisterMetaType<mv::gui::StringsAction*>("mv::gui::StringsAction");
