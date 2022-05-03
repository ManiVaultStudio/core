#pragma once

#include "actions/TriggerAction.h"

#include <QWidget>
#include <QLabel>

namespace hdps {

namespace gui {

class WidgetAction;

/**
 * Widget action label class
 *
 * Creates an interactive widget action based label 
 * Linkable actions provide a left-lick context menu that provides linking commands:
 * - Publish an action (create a public shared action)
 * - Connect to a shared action
 * - Disconnect from a shared action
 * 
 * @author Thomas Kroes
 */
class WidgetActionLabel : public QWidget
{
public:

    /** Describes the widget flags */
    enum WidgetFlag {
        ColonAfterName  = 0x00001,      /** The name label includes a post-fix colon */
    };

public:

    /**
     * Constructor
     * @param widgetAction Pointer to widget action
     * @param parent Pointer to parent widget
     * @param windowFlags Window flags
     */
    explicit WidgetActionLabel(WidgetAction* widgetAction, QWidget* parent = nullptr, const std::uint32_t& flags = ColonAfterName);

    /**
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:

    /** Update the state of the publish action */
    void updatePublishAction();

    /** Update the text of the the postfix label (depends on whether the action is published or not) */
    void updateLabel();

    /** Publish _widgetAction */
    void publishAction();

protected:
    std::uint32_t       _flags;                 /** Configuration flags */
    WidgetAction*       _widgetAction;          /** Pointer to widget action */
    QLabel              _nameLabel;             /** Action name label */
    TriggerAction       _publishAction;         /** Publish action (so that other actions can connect) */
    TriggerAction       _disconnectAction;      /** Disconnect from public action */
};

}
}
