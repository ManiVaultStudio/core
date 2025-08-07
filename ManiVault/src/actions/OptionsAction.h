// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"
#include "TriggerAction.h"
#include "TableAction.h"
#include "HorizontalGroupAction.h"

#include "models/CheckableStringListModel.h"

#include "widgets/FlowLayout.h"
#include "widgets/MultiSelectComboBox.h"

#include <QComboBox>
#include <QCompleter>
#include <QTableView>
#include <QSortFilterProxyModel>

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

    /** Extends the standard table view to support checking items */
    class CORE_EXPORT CheckableTableView : public QTableView {
    public:

        /**
         * Construct with pointer to \p parent object
         * @param parent Pointer to parent object
         */
        CheckableTableView(QWidget* parent = nullptr);

        /**
         * Invoked when the user presses somewhere in the list view
         * @param event Mouse event that occurred
         */
        void mousePressEvent(QMouseEvent* event) override;

        /**
         * Configure \p tableView such that it looks according to the checkable table view protocol
         * @param tableView Pointer to table view to configure
         */
        static void configure(QTableView* tableView);
    };

public:

    /** Describes the widget flags */
    enum WidgetFlag {
        ComboBox        = 0x00001,      /** The widget includes a combobox widget */
        ListView        = 0x00002,      /** The widget includes a list view widget */
        Selection       = 0x00004,      /** The widget includes a selection control */
        File            = 0x00008,      /** The widget includes a file control */
        Tags            = 0x00010,      /** The widget includes a flow layout with tags */

        Default = ComboBox
    };

public: // Sub actions

protected:

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
         */
        ComboBoxWidget(QWidget* parent, OptionsAction* optionsAction, const std::int32_t& widgetFlags);

    protected:

        /**
         * Respond to \p target events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

        /** Updates the line edit text to the joined selected strings */
        void updateCurrentText() const;

    protected:
        OptionsAction*          _optionsAction;         /** Pointer to owning options action */
        QHBoxLayout             _layout;                /** Horizontal layout */
        MultiSelectComboBox     _comboBox;              /** Combobox for selecting options */
        QCompleter              _completer;             /** For inline searching */
        bool                    _preventPopupClose;     /** Prevent the popup from closing when we do not want it */

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
        OptionsAction*          _optionsAction;     /** Pointer to owning options action */
        QSortFilterProxyModel   _filterModel;       /** For filtering the options */
        TableAction             _tableAction;       /** Table view for showing the data */

        friend class OptionsAction;
    };

    /** Tags view widget class for options action */
    class CORE_EXPORT TagsViewWidget : public QWidget {
    private:

        /** Utility label widget class */
        class TagLabel : public QLabel {
        public:

            /** Type of tag label */
            enum class Type {
                Regular,        /** Clicking on the label toggles the option */
                SelectAll,      /** Clicking on the label selects all options */
                SelectNone,     /** Clicking on the label clears the options selection */
                SelectInvert    /** Clicking on the label inverts the options selection */
            };

        public:

            /**
             * Construct with \p option and pointer to \p parent widget
             * @param option Tag name
             * @param type Tag type
             * @param optionsAction Pointer to options action
             * @param parent Pointer to parent widget
             */
            TagLabel(const QString& option, const Type& type, OptionsAction* optionsAction, QWidget* parent = nullptr);

            /**
             * Invoked when the user presses the mouse
             * @param event Pointer to mouse event
             */
            void mousePressEvent(QMouseEvent* event) override;

            /**
             * Triggered on mouse hover
             * @param enterEvent Pointer to enter event
             */
            void enterEvent(QEnterEvent* enterEvent) override;

            /**
             * Triggered on mouse leave
             * @param leaveEvent Pointer to leave event
             */
            void leaveEvent(QEvent* leaveEvent) override;

        private:

            /** Update the current style */
            void updateStyle();

        private:
            Type            _type;              /** Type of tag label */
            QString         _option;            /** Tag name */
            OptionsAction*  _optionsAction;     /** Pointer to options action */
        };

    protected:

        /**
         * Construct with pointer to \p parent widget, \p optionsAction and \p completer
         * @param parent Pointer to parent widget
         * @param optionsAction Pointer to options action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        TagsViewWidget(QWidget* parent, OptionsAction* optionsAction, const std::int32_t& widgetFlags);

    private:

        /** Updates the flow layout with new tags */
        void updateFlowLayout();

        /**
         * Add \p option tag label to the flow layout
         * @param option Option string
         * @param type Type of tag label
         * @param parent Pointer to parent widget
         * @return Pointer to created tag label
         */
        TagLabel* addOption(const QString& option, const TagLabel::Type& type, QWidget* parent = nullptr);

    protected:
        OptionsAction*              _optionsAction;         /** Pointer to owning options action */
        const bool                  _hasSelectionTags;      /** Whether to show tags for selection */
        QSortFilterProxyModel       _filterModel;           /** For filtering the options */
        FlowLayout                  _flowLayout;            /** Table view for showing the data */
        QMap<QString, QWidget*>     _widgetsMap;            /** Keep track of created push buttons */

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
     * Toggle \p option
     * @param option Option to toggle
     */
    void toggleOption(const QString& option);

    /**
     * Set the selected options
     * @param selectedOptions Selected options
     */
    void setSelectedOptions(const QStringList& selectedOptions);

    /**
     * Set the selected options
     * @param selectedOptions Selected option indices
     */
    void setSelectedOptions(const CheckableStringListModel::CheckedIndicesSet& selectedOptions);

    /** Selects all options */
    void selectAll();

    /** Clears the options selection */
    void selectNone();

    /** Inverts the options selection */
    void selectInvert();

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
     * @param variantMap Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

    /**
     * Set whether all options should be serialized to \p serializeAllOptions
     * By default, only the selected options are serialized
     * Setting this option to true will additionally serialize all options
     * @param serializeAllOptions Boolean determining whether all options should be serialized
     */
    void setSerializeAllOptions(bool serializeAllOptions);

    /**
     * Get whether all options should be serialized
     * @return Boolean determining whether all options should be serialized (by default, only the selected options are serialized)
     */
    bool getSerializeAllOptions() const;

public: // Action getters

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
    FileAction                  _fileAction;        /** File action */

    friend class AbstractActionsManager;
    friend class ComboBox;

private:
    bool    _serializeAllOptions;                   /** Whether all options should be serialzied, by default false */
};

}   

Q_DECLARE_METATYPE(mv::gui::OptionsAction)

inline const auto optionsActionMetaTypeId = qRegisterMetaType<mv::gui::OptionsAction*>("mv::gui::OptionsAction");
