#pragma once

#include <QWidget>

namespace hdps {

namespace gui {

class WidgetAction;
class OverlayWidget;

/**
 * Widget action widget class
 *
 * Base class for widgets that interact with a widget action
 * 
 * @author Thomas Kroes
 */
class WidgetActionWidget : public QWidget
{
public:

    /** Reserved widget settings */
    enum WidgetFlag {
        PopupLayout = 0x00100,      /** Widget with popup layout */
    };

public:

    /** Widget states */
    enum State {
        Standard,   /** Standard representation of the action (default) */
        Collapsed,  /** The widget is in a collapsed state and represented by a popup button */
        Popup       /** The widget representation of the action in a popup */
    };

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param widgetAction Pointer to the widget action that will be displayed
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    WidgetActionWidget(QWidget* parent, WidgetAction* widgetAction, const std::int32_t& widgetFlags = 0);

    /**
     * Get the widget action
     * @return Pointer to widget action
     */
    WidgetAction* getWidgetAction();

    /**
     * Set the widget action
     * @param widgetAction Pointer to widget action
     */
    virtual void setWidgetAction(WidgetAction* widgetAction);

    QSize sizeHint() const override;

protected: // Miscellaneous

    /**
     * Sets a popup layout
     * @param popupLayout Pointer to popup layout
     */
    void setPopupLayout(QLayout* popupLayout);

protected:
    WidgetAction*       _widgetAction;      /** Pointer to widget action that will be displayed */
    std::int32_t        _widgetFlags;       /** Widget creation flags */
    OverlayWidget*      _highlightWidget;   /** Pointer to highlight widget (if any) */
};

}
}
