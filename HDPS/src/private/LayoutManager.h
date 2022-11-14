#pragma once

#include <AbstractLayoutManager.h>

#include "DockManager.h"
#include "VisualizationDockWidget.h"

#include <DockAreaWidget.h>

#include <QSharedPointer>

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
    QSharedPointer<DockManager>     _dockManager;                   /** Dock manager (inherited  from ADS) */
    ads::CDockAreaWidget*           _visualizationDockArea;         /** Visualization docking area for view plugins (non-standard) */
    VisualizationDockWidget         _visualizationDockWidget;       /** Visualization dock widget for view plugins (non-standard) */
};

}
