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
     * Respond to \p target events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

protected:
    WidgetAction*   _widgetAction;                  /** Pointer to widget action */
    QLabel          _label;                         /** Label */
    bool            _isHovering;                    /** Whether the mouse pointer is hovering over the text*/
    TriggerAction   _loadDefaultAction;             /** Load default action */
    TriggerAction   _saveDefaultAction;             /** Save default action */
    TriggerAction   _loadFactoryDefaultAction;      /** Load factory default action */
};

}
}
