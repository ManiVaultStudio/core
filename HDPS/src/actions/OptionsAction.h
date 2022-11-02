#pragma once

#include "WidgetAction.h"
#include "TriggerAction.h"

#include <QStandardItemModel>
#include <QComboBox>

namespace hdps {

namespace gui {

/**
 * Options widget action class
 *
 * Action for selecting multiple options at once
 *
 * @author Thomas Kroes
 */
class OptionsAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {
        ComboBox        = 0x00001,      /** The widget includes a combobox widget */
        Selection       = 0x00004,      /** The widget includes a selection control */
        File            = 0x00008,      /** The widget includes a file control */

        Default = ComboBox
    };

public: // Widgets

    /** Combobox widget class for options action */
    class ComboBoxWidget : public QComboBox {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param optionsAction Pointer to options action
         */
        ComboBoxWidget(QWidget* parent, OptionsAction* optionsAction);

        /**
         * Paint event (overridden to show placeholder text)
         * @param paintEvent Pointer to paint event
         */
        void paintEvent(QPaintEvent* paintEvent) override;

    protected:
        OptionsAction*   _optionsAction;  /** Pointer to owning options action */

        friend class OptionsAction;
    };

    /** Line edit widget (with auto completion) class for option action */
    //class LineEditWidget : public QLineEdit {
    //protected:

    //    /**
    //     * Constructor
    //     * @param parent Pointer to parent widget
    //     * @param optionAction Pointer to option action
    //     */
    //    LineEditWidget(QWidget* parent, OptionAction* optionAction);

    //protected:
    //    OptionAction*   _optionAction;  /** Pointer to owning option action */
    //    QCompleter      _completer;     /** Completer for searching and filtering */

    //    friend class OptionAction;
    //};

protected:

    /**
     * Get widget representation of the options action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

protected:

    /**
     * Selection action class
     *
     * Action class for manipulating selections
     *
     * @author Thomas Kroes
     */
    class SelectionAction : public WidgetAction
    {
    protected:

        /** Widget class for selection action */
        class Widget : public WidgetActionWidget
        {
        protected:

            /**
             * Constructor
             * @param parent Pointer to parent widget
             * @param selectionAction Pointer to selection action
             */
            Widget(QWidget* parent, SelectionAction* selectionAction);

        protected:
            friend class SelectionAction;
        };

    public:

        /**
         * Constructor
         * @param optionsAction Reference to owning options action
         */
        SelectionAction(OptionsAction& optionsAction);

    protected:

        /**
         * Get widget representation of the selection action
         * @param parent Pointer to parent widget
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
            return new Widget(parent, this);
        };

    public: // Action getters

        TriggerAction& getSelectAllAction() { return _selectAllAction; }
        TriggerAction& getClearSelectionAction() { return _clearSelectionAction; }
        TriggerAction& getInvertSelectionAction() { return _invertSelectionAction; }

    protected:
        OptionsAction&      _optionsAction;             /** Reference to owning options action */
        TriggerAction       _selectAllAction;           /** Select all action */
        TriggerAction       _clearSelectionAction;      /** Clear selection action */
        TriggerAction       _invertSelectionAction;     /** Invert selection action */
    };

    /**
     * File action class
     *
     * Action class for loading/saving selections
     *
     * @author Thomas Kroes
     */
    class FileAction : public WidgetAction
    {
    protected:

        /** Widget class for file action */
        class Widget : public WidgetActionWidget
        {
        protected:

            /**
             * Constructor
             * @param parent Pointer to parent widget
             * @param fileAction Pointer to file action
             */
            Widget(QWidget* parent, FileAction* fileAction);

        protected:
            friend class FileAction;
        };

    public:

        /**
         * Constructor
         * @param optionsAction Reference to owning options action
         */
        FileAction(OptionsAction& optionsAction);

    protected:

        /**
         * Get widget representation of the file action
         * @param parent Pointer to parent widget
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
            return new Widget(parent, this);
        };

    public: // Action getters

        TriggerAction& getLoadSelectionAction() { return _loadSelectionAction; }
        TriggerAction& getSaveSelectionAction() { return _saveSelectionAction; }

    protected:
        OptionsAction&      _optionsAction;             /** Reference to owning options action */
        TriggerAction       _loadSelectionAction;       /** Load selection action */
        TriggerAction       _saveSelectionAction;       /** Save selection action */
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param options Options to select from
     * @param selectedOptions Initial selected options
     */
    OptionsAction(QObject* parent, const QString& title = "", const QStringList& options = QStringList(), const QStringList& selectedOptions = QStringList(), const QStringList& defaultSelectedOptions = QStringList());

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

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
     * @return Options model
     */
    const QStandardItemModel& getOptionsModel() const;

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
     */
    void setOptions(const QStringList& options);

    /**
     * Get selected options
     * @return Selected options
     */
    QStringList getSelectedOptions() const;

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
     * @param replaceSelection Whether to clear the current option selection
     */
    void selectOption(const QString& option, const bool& replaceSelection = false);

    /**
     * Set the selected options
     * @param selectedOptions Selected options
     */
    void setSelectedOptions(const QStringList& selectedOptions);

    /** Reset to default */
    void reset() override;

public: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    void connectToPublicAction(WidgetAction* publicAction) override;

    /** Disconnect this action from a public action */
    void disconnectFromPublicAction() override;

protected:  // Linking

    /**
     * Get public copy of the action (other compatible actions can connect to it)
     * @return Pointer to public copy of the action
     */
    virtual WidgetAction* getPublicCopy() const override;

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
    QStandardItemModel      _optionsModel;          /** Options model */
    SelectionAction         _selectionAction;       /** Selection action */
    FileAction              _fileAction;            /** File action */

};

}   
}
