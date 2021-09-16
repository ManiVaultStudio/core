#pragma once

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

    /*
    void enterEvent(QEvent *ev) override
    {
        setStyleSheet("QLabel { text-decoration: underline; }");
    }

    void leaveEvent(QEvent *ev) override
    {
        setStyleSheet("QLabel { text-decoration: none; }");
    }
    */

public: // Drag and drop

    void dragEnterEvent(QDragEnterEvent* dragEnterEvent);
    void dragLeaveEvent(QDragLeaveEvent* dragLeaveEvent);
    void dropEvent(QDropEvent* dropEvent);

public: // Mouse events

    void mousePressEvent(QMouseEvent* mouseEvent);
    void mouseMoveEvent(QMouseEvent* mouseEvent);

protected:
    WidgetAction*   _widgetAction;          /** Pointer to widget action */
    QPoint          _dragStartPosition;     /** Start position of the drag operation */
};

}
}
