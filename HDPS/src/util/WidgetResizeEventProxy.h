#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QResizeEvent>

namespace hdps
{

namespace util
{

/**
 * Widget resize event proxy class
 *
 * @author Thomas Kroes
 */
class WidgetResizeEventProxy : public QObject
{
public:

    /**
     * Callback function which is called when the size of the source widget changes
     * @param size New size of the source widget
     */
    typedef std::function<void(const QSize& size)> Callback;

public:

    /**
     * Constructor
     * @param parent Parent widget
     */
    WidgetResizeEventProxy(QWidget* parent);

    /**
     * Respond to changes of the source widget
     * @param target Source widget
     * @param event Event which occurred
     */
    bool eventFilter(QObject* target, QEvent* event);

public: // Getters/setters

    /**
     * Initialize the proxy
     * @param sourceWidget Source widget of which to respond to size changes
     * @param callback Callback function which is called when the size of the source widget changes
     */
    void initialize(QWidget* sourceWidget, Callback callback);

private:
    bool        _initialized;       /** Whether the proxy is initialized or not */
    QWidget*    _sourceWidget;      /** Widget that governs the current page */
    Callback    _callback;          /** Function which is called when the size of the source widget changes */
};

}
}