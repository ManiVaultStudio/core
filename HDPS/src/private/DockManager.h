#pragma once

#include "DockArea.h"

#include <util/Serializable.h>

#include <DockManager.h>

#include <QString>

/**
 * Dock manager class
 *
 * ADS inherited dock manager class, primary purpose it to support layout serialization
 *
 * @author Thomas Kroes
 */
class DockManager : public ads::CDockManager, public hdps::util::Serializable
{
    Q_OBJECT

public:

    static QMap<ads::DockWidgetArea, QString> dockWidgetAreaStrings;

public:

    /**
     * Constructs a dock manager derived from the advanced docking system
     * @param parent Pointer to parent widget
     */
    DockManager(QWidget* parent = nullptr);

    /**
     * Find the dock area widget where \p widget resides
     * @param widget Pointer to widget to look for
     * @return Pointer to ADS dock widget area (if found, otherwise nullptr)
     */
    ads::CDockAreaWidget* findDockAreaWidget(QWidget* widget);

    /**
     * Set central widget
     * @param Pointer to central widget
     * @return Pointer to central dock area widget
     */
    ads::CDockAreaWidget* setCentralWidget(QWidget* centralWidget);

    /**
     * Get central dock area widget
     * @return Pointer to central dock area widget
     */
    ads::CDockAreaWidget* getCentralDockAreaWidget();

    /**
     * Get central dock widget
     * @return Pointer to central dock widget
     */
    DockWidget* getCentralDockWidget();

public: // Serialization

    /**
     * Load from variant map
     * @param variantMap Variant map representation of the serializable object
     */
    void fromVariantMap(const QVariantMap& variantMap) override;
    
    /**
     * Save to variant map
     * @return Variant map representation of the serializable object
     */
    QVariantMap toVariantMap() const override;

private:

    /** Resets the docking layout to defaults */
    void reset();

private:
    ads::CDockAreaWidget*   _centralDockAreaWidget;     /** Pointer to central dock area widget */
    DockWidget              _centralDockWidget;         /** Default central dock widget */

    friend class VisualizationDockWidget;
};
