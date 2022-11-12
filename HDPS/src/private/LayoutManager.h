#pragma once

#include <AbstractLayoutManager.h>

#include <QSharedPointer>

#include "DockManager.h"
#include "CentralWidget.h"

#include <DockAreaWidget.h>

class QMainWindow;

namespace hdps::gui
{

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

    /**
     * Resets the layout to defaults
     */
    void reset() override;

    /**
     * Add a view plugin to the layout
     * @param viewPlugin Pointer to view plugin
     */
    void addViewPlugin(plugin::ViewPlugin* viewPlugin) override;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:

    /**
     * Convert dock widget to variant map
     * @param dockWidget Pointer to dock widget
     * @return Dock widget variant map
     */
    QVariantMap dockWidgetToVariant(ads::CDockWidget* dockWidget) const;

    /**
     * Convert dock area widget to variant map
     * @param dockAreaWidget Pointer to dock area widget
     * @return Dock area widget variant map
     */
    QVariantMap dockAreaWidgetToVariant(ads::CDockAreaWidget* dockAreaWidget) const;

    /**
     * Convert dock container widget to variant map
     * @param dockContainerWidget Pointer to dock container widget
     * @return Dock container widget variant map
     */
    QVariantMap dockContainerWidgetToVariant(ads::CDockContainerWidget* dockContainerWidget) const;

    /**
     * Updates the central widget in response to changes in docking and dock widgets visibility
     */
    void updateCentralWidget();

private:
    QSharedPointer<DockManager>     _dockManager;               /** ADS inherited dock manager */
    ads::CDockAreaWidget*           _centralDockArea;           /** Docking area for view plugins */
    ads::CDockAreaWidget*           _lastDockAreaWidget;        /** Last docking area widget (if any) */
    ads::CDockWidget                _centralDockWidget;         /** Central dock widget */
    CentralWidget                   _centralWidget;             /** Central widget which is loaded into the central dock widget */
};

}
