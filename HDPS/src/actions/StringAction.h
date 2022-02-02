#pragma once

#include "WidgetAction.h"

#include <QLineEdit>

class QWidget;
class QCompleter;

namespace hdps {

namespace gui {

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
        LineEdit        = 0x00001,      /** Widget includes a line edit */
        ResetPushButton = 0x00002,      /** There is a button to reset the string action */

        Basic   = LineEdit,
        All     = LineEdit | ResetPushButton
    };

public:

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

public: // Settings

    /**
     * Set value from variant
     * @param value Value
     */
    void setValueFromVariant(const QVariant& value) override final;

    /**
     * Convert value to variant
     * @return Value as variant
     */
    QVariant valueToVariant() const override final;

    /**
     * Convert default value to variant
     * @return Default value as variant
     */
    QVariant defaultValueToVariant() const override final;

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
};

}
}
