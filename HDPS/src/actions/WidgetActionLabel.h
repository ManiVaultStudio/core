#pragma once

#include "actions/TriggerAction.h"

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
class WidgetActionLabel : public QLabel {
public:
    explicit WidgetActionLabel(WidgetAction* widgetAction, QWidget* parent = nullptr, Qt::WindowFlags windowFlags = Qt::WindowFlags());

public: // Mouse events

    /**
     * Invoked when a mouse button is pressed
     * @param mouseEvent Pointer to mouse event
     */
    void mousePressEvent(QMouseEvent* mouseEvent) override;

    /**
     * Invoked when the mouse enters the label
     * @param event Pointer to event
     *
     */
    void enterEvent(QEvent* event) override;

    /**
     * Invoked when the mouse leaves the label
     * @param event Pointer to event
     *
     */
    void leaveEvent(QEvent* event) override;

protected:
    WidgetAction*   _widgetAction;          /** Pointer to widget action */
    QPoint          _dragStartPosition;     /** Start position of the drag operation */
    TriggerAction   _loadDefaultAction;     /** Load default action */
    TriggerAction   _saveDefaultAction;     /** Save default action */
};

}
}
