#pragma once

#include "WidgetAction.h"

#include <QLineEdit>

class QWidget;
class QPushButton;

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

    /** Describes the widget configurations (a color map combobox always exists) */
    enum WidgetFlag {
        LineEdit    = 0x00001,      /** Widget includes a line edit */
        ResetButton = 0x00002,      /** There is a button to reset the string action */

        Basic   = LineEdit,
        All     = LineEdit | ResetButton
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
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override;;

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

    /** Determines whether the current string can be reset to its default string */
    bool isResettable() const override;

    /** Reset the current string to the default string */
    void reset() override;

    /** Get placeholder text */
    QString getPlaceholderString() const;

    /**
     * Set placeholder text
     * @param placeholderText Placeholder text
     */
    void setPlaceHolderString(const QString& placeholderString);

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

protected:
    QString     _string;                /** Current string */
    QString     _defaultString;         /** Default string */
    QString     _placeholderString;     /** Place holder string */
};

}
}
