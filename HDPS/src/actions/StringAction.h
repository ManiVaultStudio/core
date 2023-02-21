#pragma once

#include "WidgetAction.h"

#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

class QWidget;
class QCompleter;

namespace hdps::gui {

/**
 * String widget action class
 *
 * Stores a string creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class StringAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        Label       = 0x00001,      /** Widget includes a label */
        LineEdit    = 0x00002,      /** Widget includes a line edit */
        TextEdit    = 0x00004,      /** Widget includes a text edit */

        Default = LineEdit,
    };

public:

    /** Label widget class for string action */
    class LabelWidget : public QLabel
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param stringAction Pointer to string action
         */
        LabelWidget(QWidget* parent, StringAction* stringAction);

        friend class StringAction;
    };

    /** Line edit widget class for string action */
    class LineEditWidget : public QLineEdit
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param stringAction Pointer to string action
         */
        LineEditWidget(QWidget* parent, StringAction* stringAction);

        friend class StringAction;
    };

    /** Text edit widget class for multi-line strings */
    class TextEditWidget : public QTextEdit
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param stringAction Pointer to string action
         */
        TextEditWidget(QWidget* parent, StringAction* stringAction);

        friend class StringAction;
    };

protected:

    /**
     * Get widget representation of the string action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param string String
     * @param defaultString Default string
     */
    StringAction(QObject* parent, const QString& title = "", const QString& string = "", const QString& defaultString = "");

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

    /**
     * Initialize the string action
     * @param string String
     * @param defaultString Default string
     */
    void initialize(const QString& string = "", const QString& defaultString = "");

    /** Gets the current string */
    QString getString() const;

    /**
     * Sets the current string
     * @param string Current string
     */
    void setString(const QString& string);

    /** Gets the default string */
    QString getDefaultString() const;

    /**
     * Sets the default string
     * @param defaultString Default string
     */
    void setDefaultString(const QString& defaultString);

    /** Get placeholder text */
    QString getPlaceholderString() const;

    /**
     * Set placeholder text
     * @param placeholderText Placeholder text
     */
    void setPlaceHolderString(const QString& placeholderString);

    /**
     * Get action at leading position
     * @return Reference to action at leading position
     */
    QAction& getLeadingAction();

    /**
     * Get action at trailing position
     * @return Reference to action at trailing position
     */
    QAction& getTrailingAction();

    /**
     * Get completer
     * @return Pointer to completer
     */
    QCompleter* getCompleter();

    /**
     * Set completer
     * @param completer Pointer to completer
     */
    void setCompleter(QCompleter* completer);

    /**
     * Get search mode (if on, there will be a leading action with a search icon and a trailing action to clear the string)
     * @return Search mode
     */
    bool getSearchMode() const;

    /**
     * Set search mode (if on, there will be a leading action with a search icon and a trailing action to clear the string)
     * @param searchMode Whether search mode is on/off
     */
    void setSearchMode(bool searchMode);

    /**
     * Get whether the string can be cleared by clicking the trailing action
     * @return Boolean indicating whether the string can be cleared
     */
    bool isClearable() const;

    /**
     * Set whether the string can be cleared by clicking the trailing action
     * @param clearable Boolean indicating whether the string can be cleared
     */
    void setClearable(bool clearable);

public: // Settings

    /**
     * Determines whether the action can be reset to its default
     * @param recursive Check recursively
     * @return Whether the action can be reset to its default
     */
    bool isResettable() override final;

    /**
     * Reset to factory default
     * @param recursive Reset to factory default recursively
     */
    void reset() override final;

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
    WidgetAction* getPublicCopy() const override;

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

signals:

    /**
     * Signals that the current string changed
     * @param string Current string that changed
     */
    void stringChanged(const QString& string);

    /**
     * Signals that the default string changed
     * @param defaultString Default string that changed
     */
    void defaultStringChanged(const QString& defaultString);

    /**
     * Signals that the placeholder string changed
     * @param placeholderString Placeholder string that changed
     */
    void placeholderStringChanged(const QString& placeholderString);

    /**
     * Signals that the completer changed
     * @param completer Pointer to completer
     */
    void completerChanged(QCompleter* completer);

protected:
    QString         _string;                /** Current string */
    QString         _defaultString;         /** Default string */
    QString         _placeholderString;     /** Place holder string */
    QAction         _leadingAction;         /** Action at the leading position */
    QAction         _trailingAction;        /** Action at the trailing position */
    QCompleter*     _completer;             /** Pointer to completer */
    bool            _searchMode;            /** Whether the string action is in search mode */
    bool            _clearable;             /** Whether the string can be cleared by clicking the trailing action */
};

}
