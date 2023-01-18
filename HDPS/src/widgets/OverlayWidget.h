#pragma once

#include "util/WidgetOverlayer.h"

#include <QWidget>

namespace hdps::gui
{

/**
 * Overlay widget class
 *
 * Overlays the parent widget with a custom widget (and synchronizes with its geometry) .
 *  
 * @author Thomas Kroes
 */
class OverlayWidget : public QWidget
{
public:

    /**
     * Construct with \p parent
     * @param parent Pointer to parent widget
     */
    OverlayWidget(QWidget* parent);

private:
    hdps::util::WidgetOverlayer     _widgetOverlayer;      /** Utility for layering on top of the target widget */
};

}
