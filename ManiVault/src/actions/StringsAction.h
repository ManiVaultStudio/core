// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include "actions/HorizontalGroupAction.h"

#include "widgets/HierarchyWidget.h"

#include "util/View.h"

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
        StringsAction*              _stringsAction;         /** Pointer to owning strings action */
        QSortFilterProxyModel       _filterModel;           /** Strings filter model */
        HierarchyWidget             _hierarchyWidget;       /** Hierarchy widget for show the strings */
        QModelIndexList             _selectedRows;          /** Selected rows in the hierarchy widget */
        QStringList                 _strings;               /** Cached string values */
        util::TextElideDelegate     _textElideDelegate;     /** Text elide delegate for the hierarchy widget */

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
     * Get locked strings
     * @return Locked strings as string list
     */
    QStringList getLockedStrings() const;

    /**
     * Set locked strings to \p lockedStrings
     * @param lockedStrings Locked strings
     */
    void setLockedStrings(const QStringList& lockedStrings);

    /**
     * Check whether this action allows duplicates
     * @return True if duplicates are allowed, false otherwise
     */
    bool allowsDuplicates() const;

    /**
     * Set whether this action allows duplicates to \p allowDuplicates
     * @param allowDuplicates True to allow duplicates, false otherwise
     */
    void setAllowDuplicates(bool allowDuplicates);

    /**
     * Get text elide mode
     * @return Text elide mode
     */
    Qt::TextElideMode getTextElideMode() const;

    /**
     * Set text elide mode to \p textElideMode
     * @param textElideMode Text elide mode
     */
    void setTextElideMode(const Qt::TextElideMode& textElideMode);

    /**
     * Check whether \p string may be added
     * @param string String to check
     * @return True if the string may be added, false otherwise
     */
    bool mayAddString(const QString& string) const;

    /**
     * Add string
     * @param string String to add
     */
    void addString(const QString& string);

    /**
     * Add /p strings
     * @param strings Strings to add
     * @param allowDuplication Whether to allow duplication of strings, defaults to false
     */
    void addStrings(const QStringList& strings, bool allowDuplication = false);

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

    /**
     * Update string \p oldString to \p newString
     * @param oldString Old string
     * @param newString New string
     */
    void updateString(const QString& oldString, const QString& newString);

    /**
     * Get model for the strings
     * @return Reference to the string list model containing the strings
     */
    const QStringListModel& getStringsModel() const { return _stringsModel; }

protected:

    /**
     * Get model for the strings
     * @return Reference to the string list model containing the strings
     */
    QStringListModel& getStringsModel() { return _stringsModel; };

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
     * @param variantMap Variant representation of the widget action
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
     * Signals that locked string changed to \p lockedStrings
     * @param lockedStrings Locked strings
     */
    void lockedStringsChanged(const QStringList& lockedStrings);

    /**
     * Signals that \p strings were removed
     * @param strings Removed strings
     */
    void stringsRemoved(const QStringList& strings);

    /**
     * Signals that the allow duplicates setting changed to \p allowDuplicates
     * @param allowDuplicates Whether duplicates are allowed
     */
    void allowDuplicatesChanged(bool allowDuplicates);

    /**
     * Signals that a string was updated from \p oldString to \p newString
     * @param oldString Old string
     * @param newString New string
     */
    void stringUpdated(const QString& oldString, const QString& newString);

    /**
     * Signals that the text elide mode changed from \p oldTextElideMode to \p newTextElideMode
     * @param oldTextElideMode Old text elide mode
     * @param newTextElideMode New text elide mode
     */
    void textElideModeChanged(const Qt::TextElideMode& oldTextElideMode, const Qt::TextElideMode& newTextElideMode);

protected:
    QString                 _category;          /** Type of string */
    QStringListModel        _stringsModel;      /** Model for the strings */
    QStringList             _lockedStrings;     /** Strings that are locked and cannot be removed or renamed */
    HorizontalGroupAction   _toolbarAction;     /** Toolbar action */
    StringAction            _nameAction;        /** String name action */
    TriggerAction           _addAction;         /** Add string action */
    TriggerAction           _removeAction;      /** Remove string action */
    QCompleter*             _completer;         /** Pointer to completer for auto-completion, maybe nullptr */
    bool                    _allowDuplicates;   /** Whether to allow string duplicates */
    Qt::TextElideMode       _textElideMode;     /** Text elide mode for the strings */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::StringsAction)

inline const auto stringsActionMetaTypeId = qRegisterMetaType<mv::gui::StringsAction*>("mv::gui::StringsAction");
