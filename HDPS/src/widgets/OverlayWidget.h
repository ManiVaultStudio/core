#pragma once

#include "util/WidgetFader.h"

#include <QWidget>

namespace hdps::gui
{

/**
 * Overlay widget class
 *
 * Overlays the parent widget with a widget and synchronizes with the parent widget geometry.
 * It also animates the overlay opacity with a widget fader.
 *  
 * @author Thomas Kroes
 */
class OverlayWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    OverlayWidget(QWidget* parent);

    /**
     * Respond to \p target events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /** Override the show member to fade the opacity over time */
    void show();

    /** Override the hide member to fade the opacity over time */
    void hide();

    /**
     * Get widget fader
     * @return Reference to widget fader
     */
    util::WidgetFader& getWidgetFader();

protected:

    /** Setups the layout etc. */
    virtual void initialize();

signals:

    /** Signals that the widget has become visible (after fading in) */
    void shown();

    /** Signals that the widget has become (partially) hidden (after fading out) */
    void hidden();

private:
    util::WidgetFader   _widgetFader;   /** Widget fader for animating the widget opacity */
};

}
