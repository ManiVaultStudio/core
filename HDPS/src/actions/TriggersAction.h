#pragma once

#include "WidgetAction.h"

#include <QUuid>

class QPushButton;

namespace hdps {

namespace gui {

/**
 * Triggers action class
 *
 * Action class for multiple triggers
 *
 * @author Thomas Kroes
 */
class TriggersAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {
        Horizontal  = 0x00001,      /** Trigger buttons are distributed in a horizontal layout */
        Vertical    = 0x00002,      /** Trigger buttons are distributed in a vertical layout */

        Default = Horizontal
    };

    /** Trigger for push button */
    struct Trigger {

        /**
         * Constructor
         * @param text Push button text
         * @param tooltip Push button tooltip
         * @param enabled Push button enabled
         */
        Trigger(const QString& text, const QString& tooltip, bool enabled = true) :
            _id(QUuid::createUuid().toString()),
            _enabled(enabled),
            _text(text),
            _tooltip(tooltip)
        {
        }

        /**
         * Comparison operator for two triggers (compares the identifiers)
         * @param rhs Right hand sign of the comparison
         */
        bool operator==(const Trigger& rhs) const {
            if (rhs._id == _id)
                return true;

            return false;
        }

        QString     _id;        /** Unique trigger identifier */
        bool        _enabled;   /** Push button read-only */
        QString     _text;      /** Push button text */
        QString     _tooltip;   /** Push button tooltip */
    };

public:

    /** Widget class for triggers action */
    class Widget : public QWidget {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param triggersAction Pointer to triggers action
         * @param widgetFlags Widget flags for the configuration of the widget
         */
        Widget(QWidget* parent, TriggersAction* triggersAction, const std::int32_t& widgetFlags);

    protected:
        TriggersAction*     _triggersAction;        /** Pointer to triggers action */

        friend class TriggersAction;
    };

protected:

    /**
     * Get widget representation of the triggers action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override
    {
        return new Widget(parent, this, widgetFlags);
    }

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param triggers Vector of triggers
     */
    TriggersAction(QObject* parent, const QString& title = "", const QVector<Trigger>& triggers = QVector<Trigger>());

    /**
     * Get triggers
     * @return Vector of triggers
     */
    const QVector<Trigger>& getTriggers() const;

    /**
     * Set triggers
     * @param triggers Vector of triggers
     */
    void setTriggers(const QVector<Trigger>& triggers);

    /**
     * Set trigger text
     * @param triggerIndex Index of the trigger
     * @param text Trigger text
     */
    void setTriggerText(std::int32_t triggerIndex, const QString& text);

    /**
     * Set trigger tooltip
     * @param triggerIndex Index of the trigger
     * @param tooltip Trigger tooltip
     */
    void setTriggerTooltip(std::int32_t triggerIndex, const QString& tooltip);

    /**
     * Set trigger enabled
     * @param triggerIndex Index of the trigger
     * @param enabled Trigger enabled
     */
    void setTriggerEnabled(std::int32_t triggerIndex, const bool& enabled);

protected:

    /**
     * Create trigger push button
     * @param trigger Trigger for the push button
     * @return Pointer to push button
     */
    QPushButton* createTriggerPushButton(const Trigger& trigger);

signals:

    /**
     * Signals that the triggers changed
     * @param triggers Triggers
     */
    void triggersChanged(const QVector<Trigger>& triggers);

    /**
     * Signals that a trigger changed
     * @param triggerIndex Index of the trigger
     * @param trigger Trigger that changed
     */
    void triggerChanged(std::int32_t triggerIndex, const Trigger& trigger);

    /**
     * Signals that a trigger is executed
     * @param triggerIndex Index of the triggered
     */
    void triggered(std::int32_t triggerIndex);

protected:
    QVector<Trigger>     _triggers;      /** Triggers */

    friend class Widget;
};

}
}
