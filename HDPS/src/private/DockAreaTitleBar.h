#pragma once

#include <DockAreaTitleBar.h>

/**
 * Dock area title bar
 * 
 * Extends the ADS dock area title bar in order to add a settings hamburger menu tot the title bar.
 * 
 * @author Thomas Kroes
 */
class DockAreaTitleBar : public ads::CDockAreaTitleBar
{
public:

    /**
     * Construct with \p dockAreaWidget
     * @param dockAreaWidget Pointer to owning dock area widget
     */
    DockAreaTitleBar(ads::CDockAreaWidget* dockAreaWidget);
};