#pragma once

#include "actions/TriggerAction.h"

namespace hdps {

namespace gui {

class WidgetAction;

/**
 * Widget action options class
 *
 * Action class which adds action options to a label 
 * 
 * @author Thomas Kroes
 */
class WidgetActionOptions : public QObject {
public:

    /**
     * Constructor
     * @param widgetAction Pointer to widget action
     * @param label Pointer to label widget, an options context menu will be added to this widget
     */
    explicit WidgetActionOptions(WidgetAction* widgetAction, QLabel* label);

    /**
     * Respond to target events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

protected:
    WidgetAction*   _widgetAction;                  /** Pointer to widget action */
    QLabel*         _label;                         /** Pointer to label widget, an options context menu will be added to this widget */
    TriggerAction   _loadDefaultAction;             /** Load default action */
    TriggerAction   _saveDefaultAction;             /** Save default action */
    TriggerAction   _loadFactoryDefaultAction;      /** Load factory default action */
};

}
}
