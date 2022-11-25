#pragma once

#include <AbstractLayoutManager.h>

#include "DockManager.h"
#include "ViewPluginsDockWidget.h"

#include <DockAreaWidget.h>

#include <QSharedPointer>

class QMainWindow;

namespace hdps::gui
{

/**
 * Layout manager class
 *
 * Manages the placement of view plugins on the screen using the Advanced Dock System (ADS) for Qt
 * 
 * It distinguishes between built-in (system) view plugins (data hierarchy, data properties etc.) and other view plugins (scatter plot, image viewer etc.).
 * 
 * It has a central area where non-system view plugins are docked, and areas around it (left, right, top and bottom) where
 * system view plugins are docked.
 * 
 * It uses two ADS dock managers:
 * - One for the main layout (\p _dockManager) 
 * - One for the docking of non-system view plugins in the central area dock widget (\p _viewPluginsDockWidget)
 * 
 * @author Thomas Kroes
 */
class LayoutManager : public AbstractLayoutManager
{
    Q_OBJECT

public:

    /** Default constructor */
    LayoutManager();

    /** Default destructor */
    ~LayoutManager() override;

    /**
     * Initializes the layout manager to work with the main window
     * @param mainWindow Pointer to main window to apply the layout manager to
     */
    void initialize(QMainWindow* mainWindow) override;

    /** Resets the layout to defaults */
    void reset() override;

    /**
     * Add a view plugin to the \p dockArea of \p dockViewPlugin
     * @param viewPlugin Pointer to view plugin to add to layout
     * @param dockToViewPlugin Pointer to view plugin to which the view plugin will be docked (docked top-level if nullptr)
     * @param dockArea Dock area in which \p dockToViewPlugin will be docked
     */
    void addViewPlugin(plugin::ViewPlugin* viewPlugin, plugin::ViewPlugin* dockToViewPlugin = nullptr, gui::DockAreaFlag dockArea = gui::DockAreaFlag::Right) override;

    /**
     * Set whether \p viewPlugin is isolated or not (closes all other view plugins when isolated)
     * @param viewPlugin Pointer to view plugin to add to layout
     * @param isolate Whether to isolate \p viewPlugin or to reset the view layout prior to isolation
     */
    void isolateViewPlugin(plugin::ViewPlugin* viewPlugin, bool isolate) override;

public: // Serialization

    /**
     * Load layout from variant
     * @param Variant representation of the layout
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save layout to variant
     * @return Variant representation of the layout
     */
    QVariantMap toVariantMap() const override;

    /** Get all view plugin dock widgets, both from the main ánd the view plugins dock widget dock managers */
    ViewPluginDockWidgets getViewPluginDockWidgets();

private:
    QSharedPointer<DockManager>     _dockManager;                   /** Main dock manager (inherited  from ADS) */
    bool                            _initialized;                   /** Whether the layout manager is initialized or not */
    ViewPluginsDockWidget           _viewPluginsDockWidget;         /** Dock widget for view plugins */
    QMap<DockWidget*, bool>         _cachedDockWidgetsVisibility;   /** Cached dock widgets visibility for view plugin isolation */
};

}
