// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"
#include "TriggerAction.h"
#include "TreeAction.h"
#include "HorizontalGroupAction.h"

#include "models/CheckableStringListModel.h"

#include <QSet>
#include <QPersistentModelIndex>
#include <QComboBox>
#include <QLineEdit>
#include <QCompleter>
#include <QListView>

namespace mv::gui {

/**
 * Options widget action class
 *
 * Action for selecting multiple options at once
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT OptionsAction : public WidgetAction
{
    Q_OBJECT

private:

    /** Extends the standard list view to support checking items in the checkable string list model */
    class CORE_EXPORT CheckableItemView : public QListView {
    public:

        /** No need for a custom constructor */
        using QListView::QListView;

        /**
         * Invoked when the user presses somewhere in the list view
         * @param event Mouse event that occurred
         */
        void mousePressEvent(QMouseEvent* event) override;
    };

public:

    /** Describes the widget flags */
    enum WidgetFlag {
        ComboBox        = 0x00001,      /** The widget includes a combobox widget */
        ListView        = 0x00002,      /** The widget includes a list view widget */
        Selection       = 0x00004,      /** The widget includes a selection control */
        File            = 0x00008,      /** The widget includes a file control */

        Default = ComboBox
    };

public: // Sub actions

protected:

    /**
     * Selection action class
     *
     * Action class for manipulating selections
     *
     * @author Thomas Kroes
     */
    class CORE_EXPORT SelectionAction : public HorizontalGroupAction
    {
    public:

        /**
         * Constructor
         * @param optionsAction Reference to owning options action
         */
        SelectionAction(OptionsAction& optionsAction);

    public: // Action getters

        TriggerAction& getSelectAllAction() { return _selectAllAction; }
        TriggerAction& getClearSelectionAction() { return _clearSelectionAction; }
        TriggerAction& getInvertSelectionAction() { return _invertSelectionAction; }

    protected:
        OptionsAction&  _optionsAction;             /** Reference to owning options action */
        TriggerAction   _selectAllAction;           /** Select all action */
        TriggerAction   _clearSelectionAction;      /** Clear selection action */
        TriggerAction   _invertSelectionAction;     /** Invert selection action */
    };

    /**
     * File action class
     *
     * Action class for loading/saving selections
     *
     * @author Thomas Kroes
     */
    class CORE_EXPORT FileAction : public HorizontalGroupAction
    {
    public:

        /**
         * Constructor
         * @param optionsAction Reference to owning options action
         */
        FileAction(OptionsAction& optionsAction);

    public: // Action getters

        TriggerAction& getLoadSelectionAction() { return _loadSelectionAction; }
        TriggerAction& getSaveSelectionAction() { return _saveSelectionAction; }

    protected:
        OptionsAction&  _optionsAction;             /** Reference to owning options action */
        TriggerAction   _loadSelectionAction;       /** Load selection action */
        TriggerAction   _saveSelectionAction;       /** Save selection action */
    };

public: // Widgets

    /** Combobox widget class for options action */
    class CORE_EXPORT ComboBoxWidget : public QWidget {
    protected:

        /**
         * Construct with pointer to \p parent widget, \p optionsAction and \p completer
         * @param parent Pointer to parent widget
         * @param optionsAction Pointer to options action
         * @param widgetFlags Widget flags for the configuration of the widget
         * @param completer Pointer to completer
         */
        ComboBoxWidget(QWidget* parent, OptionsAction* optionsAction, const std::int32_t& widgetFlags, QCompleter* completer);

    protected:

        /**
         * Respond to \p target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event);

        /** Updates the line edit text to the joined selected strings */
        void updateCurrentText();

    protected:
        OptionsAction*          _optionsAction;     /** Pointer to owning options action */
        QHBoxLayout             _layout;            /** Horizontal layout */
        QComboBox               _comboBox;          /** Combobox for selecting options */
        CheckableItemView       _view;              /** View for checking items */

        friend class OptionsAction;
    };

    /** List view widget class for options action */
    class CORE_EXPORT ListViewWidget : public QWidget {
    protected:

        /**
         * Construct with pointer to \p parent widget, \p optionsAction and \p completer
         * @param parent Pointer to parent widget
         * @param optionsAction Pointer to options action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        ListViewWidget(QWidget* parent, OptionsAction* optionsAction, const std::int32_t& widgetFlags);

    protected:
        OptionsAction*  _optionsAction;     /** Pointer to owning options action */
        TreeAction      _treeAction;        /** Tree action */

        friend class OptionsAction;
    };

protected:

    /**
     * Get widget representation of the options action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param options Options to select from
     * @param selectedOptions Initial selected options
     */
    Q_INVOKABLE explicit OptionsAction(QObject* parent, const QString& title, const QStringList& options = QStringList(), const QStringList& selectedOptions = QStringList());

    /**
     * Initialize the option action
     * @param options Options to select from
     * @param selectedOptions Initial selected options
     */
    void initialize(const QStringList& options = QStringList(), const QStringList& selectedOptions = QStringList());

    /**
     * Get available options
     * @return Available options
     */
    QStringList getOptions() const;

    /**
     * Get options model
     * @return Checkable string list model
     */
    const CheckableStringListModel& getOptionsModel() const;

protected:

    /**
     * Get options model
     * @return Checkable string list model
     */
    CheckableStringListModel& getOptionsModel();

public:

    /**
     * Get the number of options
     * @return Number of options
     */
    std::uint32_t getNumberOfOptions() const;

    /**
     * Get whether a specific option exists
     * @param option Name of the option to check for
     * @return Boolean indicating whether a specific option exists
     */
    bool hasOption(const QString& option) const;

    /**
     * Get whether there are any options
     * @return Boolean indicating whether there are any options
     */
    bool hasOptions() const;

    /**
     * Set the available options
     * @param options Available Options
     * @param clearSelection Whether to clear the current selection
     */
    void setOptions(const QStringList& options, bool clearSelection = false);

    /**
     * Get selected options
     * @return Selected options
     */
    QStringList getSelectedOptions() const;

    /**
     * Get selected option indices
     * @return Selected options indices
     */
    QList<std::int32_t> getSelectedOptionIndices() const;

    /**
     * Get whether a specific option is selected
     * @param option Name of the option to check for
     * @return Boolean indicating whether the option is selected or not
     */
    bool isOptionSelected(const QString& option) const;

    /**
     * Get whether one or more options are selected
     * @return Boolean indicating whether one or more options are selected
     */
    bool hasSelectedOptions() const;

    /**
     * Select option
     * @param option Option to select
     * @param unselect Whether to select or unselect the option
     */
    void selectOption(const QString& option, bool unselect = false);

    /**
     * Set the selected options
     * @param selectedOptions Selected options
     */
    void setSelectedOptions(const QStringList& selectedOptions);

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
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    SelectionAction& getSelectionAction() { return _selectionAction; }
    FileAction& getFileAction() { return _fileAction; }

signals:

    /**
     * Signals that the options changed
     * @param options Options
     */
    void optionsChanged(const QStringList& options);

    /**
     * Signals that the selected options changed
     * @param selectedOptions Selected options
     */
    void selectedOptionsChanged(const QStringList& selectedOptions);

protected:
    CheckableStringListModel    _optionsModel;      /** Options model */
    SelectionAction             _selectionAction;   /** Selection action */
    FileAction                  _fileAction;        /** File action */

    friend class AbstractActionsManager;
    friend class ComboBox;
};

}   

Q_DECLARE_METATYPE(mv::gui::OptionsAction)

inline const auto optionsActionMetaTypeId = qRegisterMetaType<mv::gui::OptionsAction*>("mv::gui::OptionsAction");
