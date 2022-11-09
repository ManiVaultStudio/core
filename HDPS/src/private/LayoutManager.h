#pragma once

#include <AbstractLayoutManager.h>

#include <QSharedPointer>

#include "DockManager.h"
#include "DockAreaWidget.h"

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
    QSharedPointer<ads::CDockManager>   _dockManager;           /** Manager for docking */
    //ads::CDockAreaWidget    _centralDockArea;           /** Docking area for view plugins */
    //ads::CDockAreaWidget*   _lastDockAreaWidget;        /** Last docking area widget (if any) */
    //ads::CDockWidget        _centralDockWidget;         /** Dock widget for view plugins */
    //ads::CDockWidget        _startPageDockWidget;       /** Dock widget for the start page */
};

}
