#pragma once

#include "util/WidgetFader.h"

#include <QObject>

class QWidget;

namespace hdps::util {

/**
 * Widget overlayer utility class
 *
 * Helper class for layering a widget on top of another widget and synchronizing its geometry.
 *
 * @author Thomas Kroes
 */
class WidgetOverlayer : public QObject
{
    Q_OBJECT

public:

    /**
     * Construct widget overlayer with \p parent object, \p sourceWidget and \p targetWidget
     * @param parent Pointer to parent object
     * @param sourceWidget Pointer to source widget (will be layered on top of the \p targetWidget)
     * @param targetWidget Pointer to target widget
     */
    WidgetOverlayer(QObject* parent, QWidget* sourceWidget, QWidget* targetWidget);

    /**
     * Respond to \p target events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /**
     * Get widget fader
     * @return Reference to widget fader
     */
    util::WidgetFader& getWidgetFader();

private:
    util::WidgetFader   _widgetFader;       /** Widget fader for animating the widget opacity */
    QWidget*            _sourceWidget;      /** Pointer to source widget (will be layered on top of the \p targetWidget) */
    QWidget*            _targetWidget;      /** Pointer to target widget */
};

}
