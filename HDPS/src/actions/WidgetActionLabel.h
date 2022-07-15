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

    /**
     * Invoked when the widget is resized
     * @param resizeEvent Pointer to the resize event
     */
    void resizeEvent(QResizeEvent* resizeEvent) override;

    /** Gets whether label elide is enabled or not */
    bool geElide() const;

    /**
     * Set whether label elide is enabled or not
     * @param labelElide Whether label elide is enabled or not
     */
    void setElide(bool elide);

private:

    /** Update the state of the publish action */
    void updatePublishAction();

    /** Update the text of the the postfix label (depends on whether the action is published or not) */
    void updateLabel();

    /** Publish _widgetAction */
    void publishAction();

    void elide();

protected:
    std::uint32_t       _flags;                 /** Configuration flags */
    WidgetAction*       _widgetAction;          /** Pointer to widget action */
    QLabel              _nameLabel;             /** Action name label */
    bool                _elide;            /** Whether to enable label elide (e.g. whether to truncate text and show an ellipsis when there is insufficient space for the text) */
    TriggerAction       _publishAction;         /** Publish action (so that other actions can connect) */
    TriggerAction       _disconnectAction;      /** Disconnect from public action */
};

}
}
