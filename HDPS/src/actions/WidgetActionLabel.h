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
 * Label widget that display the widget action text
 * 
 * @author Thomas Kroes
 */
class WidgetActionLabel : public QWidget {
public:

    /**
     * Constructor
     * @param widgetAction Pointer to widget action
     * @param parent Pointer to parent widget
     * @param windowFlags Window flags
     */
    explicit WidgetActionLabel(WidgetAction* widgetAction, QWidget* parent = nullptr, Qt::WindowFlags windowFlags = Qt::WindowFlags());

    /**
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:

    /** Update the text of the the prefix label (depends on whether the action is published or not) */
    void updatePrefixLabel();

protected:
    WidgetAction*       _widgetAction;          /** Pointer to widget action */
    QLabel              _prefixLabel;           /** Prefix label (for linking cue(s)) */
    QLabel              _nameLabel;             /** Action name label */
    TriggerAction       _publishAction;         /** Publish action (so that other actions can connect) */
    TriggerAction       _connectAction;         /** Connect to public action */
    TriggerAction       _disconnectAction;      /** Disconnect from public action */
};

}
}
