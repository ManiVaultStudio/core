#pragma once

#include "WidgetAction.h"

#include <QLineEdit>

namespace hdps {

namespace gui {

/**
 * Status action class
 *
 * Action class for reporting status
 *
 * @author Thomas Kroes
 */
class StatusAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {
        LineEdit    = 0x00001,      /** Status is logged in a line edit */

        Default = LineEdit
    };

    /** Status enumeration */
    enum Status {
        Undefined = -1,     /** There is no status information */
        Info,               /** Normal info */
        Warning,            /** Warning */
        Error               /** An error occurred */
    };

public:

    /** Line edit widget class for status action */
    class LineEditWidget : public QLineEdit {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param statusAction Pointer to status action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        LineEditWidget(QWidget* parent, StatusAction* statusAction, const std::int32_t& widgetFlags);

    protected:
        StatusAction*   _statusAction;      /** Pointer to status action */
        QAction         _trailingAction;    /** Trailing action indicating status type */

        friend class StatusAction;
    };

protected:

    /**
     * Get widget representation of the status action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override
    {
        return new LineEditWidget(parent, this, widgetFlags);
    }

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param status Status type
     * @param message Status message
     */
    StatusAction(QObject* parent, const QString& title = "", const Status& status = Status::Undefined, const QString& message = "");

    /**
     * Initialize the status action
     * @param status Status type
     * @param message Status message
     */
    void initialize(const Status& status = Status::Info, const QString& message = "");

    /**
     * Get status type
     * @return Status type
     */
    Status getStatus() const;

    /**
     * Set status type
     * @param status Status type
     */
    void setStatus(const Status& status);

    /**
     * Get status message
     * @return Status message
     */
    QString getMessage() const;

    /**
     * Set status message
     * @param message Status message
     * @param vanish Whether to reset the message after the disappear message delay
     */
    void setMessage(const QString& message, bool vanish = false);

signals:

    /**
     * Signals that the status changed
     * @param status Status
     */
    void statusChanged(const Status& status);

    /**
     * Signals that the message changed
     * @param message Message that changed
     */
    void messageChanged(const QString& message);

protected:
    Status      _status;    /** Current status enum */
    QString     _message;   /** Status message */

    static constexpr std::int32_t MESSAGE_DISAPPEAR_INTERVAL = 1500;   /** Delay after which a message is reset */

    //friend class Widget;
};

}
}
