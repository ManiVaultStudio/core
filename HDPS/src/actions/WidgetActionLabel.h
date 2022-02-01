#pragma once

#include "actions/TriggerAction.h"
#include "actions/WidgetActionOptions.h"

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

protected:
    WidgetAction*           _widgetAction;      /** Pointer to widget action */
    QLabel                  _label;             /** Label */
    WidgetActionOptions     _options;           /** Widget action options */
};

}
}
